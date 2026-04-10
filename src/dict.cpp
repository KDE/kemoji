/*
    SPDX-FileCopyrightText: 2022 Weng Xuetian <wegnxt@gmail.com>
    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dict.h"

#include <QDir>
#include <QFile>
#include <QLocale>
#include <QRegularExpression>
#include <QSettings>
#include <QStandardPaths>
#include <QTextBoundaryFinder>
#include <QtConcurrent/QtConcurrentRun>

#include <KLocalizedString>
#include <algorithm>

#include "category.h"
#include "emoji.h"
#include "group.h"
#include "kemoji_logging.h"
#include "settings_p.h"
#include "tones.h"

using namespace Qt::Literals::StringLiterals;
using namespace KEmoji;

Dict::Dict(QObject *parent)
    : QObject(parent)
{
    initialize();
}

Dict &Dict::instance()
{
    static Dict _instance;
    return _instance;
}

void Dict::initialize()
{
    m_emojis.clear();
    m_categories.clear();
    m_categories += Categories::Recent;
    m_categories += Categories::Favorite;
    m_categories += Categories::All;
    m_categories += Categories::Custom;

    connect(&Settings::instance(), &Settings::emojiHistoryChanged, this, &Dict::emojiHistoryChanged);

    loadCustom();

    QFuture<void> future = QtConcurrent::run(&Dict::load, this).then([this]() {
        m_loaded = true;
        Q_EMIT loadedChanged();
    });
}

bool Dict::loaded() const
{
    return m_loaded;
}

const Group &Dict::emojis() const
{
    return m_categoryGroups.at(Categories::All);
}

const Group &Dict::variantGroupForEmoji(const Emoji &emoji) const
{
    if (!m_variantGroups.contains(emoji.id())) {
        return emptyGroup;
    }
    return m_variantGroups.at(emoji.id());
}

const QList<Categories::Category> &Dict::categories() const
{
    return m_categories;
}

const Group &Dict::categoryGroup(Categories::Category category) const
{
    if (!m_categoryGroups.contains(category)) {
        return emptyGroup;
    }
    return m_categoryGroups.at(category);
}

int Dict::recentEmojiIndex(const KEmoji::Emoji &emoji) const
{
    return Settings::instance().recentIndex(emoji.id());
}

int Dict::timesEmojiUsed(const KEmoji::Emoji &emoji) const
{
    return Settings::instance().timesUsed(emoji.id());
}

void Dict::load()
{
    const QHash<QString, QString> specialCases{{QLatin1String{"zh-TW"}, QLatin1String{"zh_Hant"}}, {QLatin1String{"zh-HK"}, QLatin1String{"zh_Hant_HK"}}};
    QLocale locale;
    QStringList dicts;
    auto bcp = locale.bcp47Name();
    bcp = specialCases.value(bcp, bcp);
    bcp.replace(QLatin1Char('-'), QLatin1Char('_'));

    const QString dictName = "kemoji/%1.dict"_L1.arg(bcp);
    const auto path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, dictName);
    if (!path.isEmpty()) {
        dicts << path;
    }

    for (qsizetype underscoreIndex = -1; (underscoreIndex = bcp.lastIndexOf(QLatin1Char('_'), underscoreIndex)) != -1; --underscoreIndex) {
        const QString genericDictName = "kemoji/%1.dict"_L1.arg(bcp.left(underscoreIndex));
        const auto genericPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, genericDictName);

        if (!genericPath.isEmpty() && !dicts.contains(genericPath)) {
            dicts << genericPath;
        }
    }

    // Always fallback to en, because some annotation data only have minimum data.
    const auto genericPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "kemoji/en.dict"_L1);
    if (!dicts.contains(genericPath) && !genericPath.isEmpty()) {
        dicts << genericPath;
    }

    // In case we're running tests.'
    if (dicts.isEmpty()) {
        dicts << u"../data/en.dict"_s;
    }

    if (dicts.isEmpty()) {
        qCWarning(KEMOJI) << "could not find emoji dictionaries." << dictName;
        return;
    }

    m_categoryGroups[Categories::All] = Group();

    // We load in reverse order, because we want to preserve the order in en.dict.
    // en.dict almost always gives complete set of data.
    std::ranges::for_each(dicts.crbegin(), dicts.crend(), [this](const QString &path) {
        loadDict(path);
    });
    for (const auto &emoji : m_emojis) {
        if (!m_categories.contains(emoji.category())) {
            m_categories += emoji.category();
        }
    }
}

void Dict::loadDict(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }
    auto buffer = file.readAll();
    buffer = qUncompress(buffer);
    QDataStream stream(&buffer, QIODevice::ReadOnly);
    QList<Emoji> emojis;
    stream >> emojis;

    std::ranges::for_each(emojis, [this](const Emoji &emoji) {
        Group::EmojiIt it;
        if (m_categoryGroups[Categories::All].contains(emoji)) {
            it = *m_categoryGroups[Categories::All].m_emojiIts[emoji.id()];
            // Overwrite with new data but keep previous name as fallback.
            auto &foundEmoji = *it;
            const QString fallbackName = foundEmoji.name();
            foundEmoji = emoji;
            foundEmoji.setFallbackName(fallbackName);
        } else {
            it = m_emojis.insert(m_emojis.end(), emoji);
            m_categoryGroups[Categories::All].add(it);
        }

        loadEmojiToCategoryGroup(it);

        auto tonelessEmoji = Tones::removeTonesFromEmoji(emoji);
        if (!m_categoryGroups[Categories::All].contains(tonelessEmoji)) {
            tonelessEmoji.setUnicode(qualify(tonelessEmoji.unicode()));
        }
        if (!m_categoryGroups[Categories::All].contains(tonelessEmoji)) {
            return;
        }
        if (tonelessEmoji == emoji) {
            return;
        }
        if (m_variantGroups.contains(tonelessEmoji.id())) {
            auto &group = m_variantGroups[tonelessEmoji.id()];
            group.add(it);
        } else {
            const auto baseIt = std::find(m_emojis.begin(), m_emojis.end(), tonelessEmoji);
            Group group;
            group.add(baseIt);
            group.add(it);
            m_variantGroups[baseIt->id()] = group;
        }
    });
}

void Dict::loadEmojiToCategoryGroup(Group::EmojiIt it)
{
    if (!m_categoryGroups.contains(it->category())) {
        m_categoryGroups[it->category()] = Group();
    }
    m_categoryGroups[it->category()].add(it);
}

void Dict::loadCustom()
{
    const auto customEmojis = Settings::instance().customEmojis().keys();
    std::ranges::for_each(customEmojis, [this](const QString &name) {
        Group::EmojiIt it = m_emojis.insert(m_emojis.end(), name);
        it->setCategory(Categories::Custom);
        m_categoryGroups[Categories::All].add(it);
    });
}

QString Dict::qualify(QString emoji)
{
    QTextBoundaryFinder finder(QTextBoundaryFinder::Grapheme, emoji);
    int from = 0;
    while (finder.toNextBoundary() != -1) {
        auto to = finder.position();
        const auto currentChar = emoji.mid(from, to - from);
        QString nextChar;
        const auto next = finder.toNextBoundary();
        if (next != -1) {
            nextChar = emoji.mid(to, next - to);
            finder.toPreviousBoundary();
        }
        if (currentChar != u"\u200D"_s && currentChar != u"\uFE0F"_s && nextChar != u"\uFE0F"_s) {
            emoji.replace(from, to - from, u"%1\uFE0F"_s.arg(currentChar));
            to += u"\uFE0F"_s.length();
        }
        from = to;
    }
    return emoji;
}

void Dict::emojiUsed(const Emoji &emoji)
{
    if (!m_categoryGroups[Categories::All].contains(emoji)) {
        return;
    }

    Settings::instance().emojiUsed(emoji.id());
}

bool Dict::registerCustomEmoji(const QUrl &source, const QString &name)
{
    return Settings::instance().registerCustomEmoji(source, name);
}

bool Dict::unregisterCustomEmoji(const QString &name)
{
    return Settings::instance().unregisterCustomEmoji(name);
}

void Dict::clearHistory()
{
    Settings::instance().clearHistory();
}

#include "moc_dict.cpp"
