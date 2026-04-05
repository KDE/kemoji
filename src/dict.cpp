/*
    SPDX-FileCopyrightText: 2022 Weng Xuetian <wegnxt@gmail.com>
    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dict.h"

#include <QDir>
#include <QFile>
#include <QLocale>
#include <QSettings>
#include <QStandardPaths>
#include <QtConcurrent/QtConcurrentRun>

#include <KLocalizedString>
#include <algorithm>

#include "category.h"
#include "emoji.h"
#include "group.h"
#include "kemoji_logging.h"
#include "settings.h"
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
    m_categories += Category(Category::Recent);
    m_categories += Category(Category::Favorite);
    m_categories += Category(Category::All);
    m_categories += Category(Category::Custom);

    connect(&Settings::instance(), &Settings::recentEmojisChanged, this, &Dict::recentEmojisChanged);
    connect(&Settings::instance(), &Settings::favoriteEmojisChanged, this, &Dict::favoriteEmojisChanged);

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
    return m_completeGroup;
}

const Group &Dict::familyGroupForEmoji(const Emoji &emoji) const
{
    if (!m_emojiFamilyGroups.contains(emoji.toString(Qt::RichText))) {
        return emptyGroup;
    }
    return m_emojiFamilyGroups.at(emoji.toString(Qt::RichText));
}

const QList<Category> &Dict::categories() const
{
    return m_categories;
}

const QList<KEmoji::Emoji> Dict::emojisForCategory(KEmoji::Category category) const
{
    QList<Emoji> emojis;
    std::copy_if(m_emojis.begin(), m_emojis.end(), std::back_inserter(emojis), [category](Emoji emoji) {
        return emoji.category() == category;
    });
    return emojis;
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
        if (m_completeGroup.contains(emoji)) {
            it = *m_completeGroup.m_emojiIts[emoji.toString(Qt::RichText)];
            // Overwrite with new data but keep previous name as fallback.
            auto &foundEmoji = *it;
            const QString fallbackName = foundEmoji.name();
            foundEmoji = emoji;
            foundEmoji.setFallbackName(fallbackName);
        } else {
            it = m_emojis.insert(m_emojis.end(), emoji);
            m_completeGroup.add(it);
        }

        const auto tonelessEmoji = Tones::removeTonesFromEmoji(emoji);
        if (tonelessEmoji == emoji) {
            return;
        }
        if (m_emojiFamilyGroups.contains(tonelessEmoji.toString(Qt::RichText))) {
            auto &group = m_emojiFamilyGroups[tonelessEmoji.toString(Qt::RichText)];
            group.add(it);
        } else {
            const auto baseIt = std::find(m_emojis.begin(), m_emojis.end(), tonelessEmoji);
            Group group;
            group.add(baseIt);
            group.add(it);
            m_emojiFamilyGroups[baseIt->toString(Qt::RichText)] = group;
        }
    });
}

void Dict::loadCustom()
{
    const auto customEmojis = Settings::instance().customEmojis().keys();
    std::ranges::for_each(customEmojis, [this](const QString &name) {
        Group::EmojiIt it = m_emojis.insert(m_emojis.end(), name);
        it->setCategory(Category::Custom);
        m_completeGroup.add(it);
    });
}

void Dict::emojiUsed(const Emoji &emoji)
{
    if (!m_completeGroup.contains(emoji)) {
        return;
    }

    Settings::instance().emojiUsed(emoji.id());
}

#include "moc_dict.cpp"
