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
#include <QtConcurrentRun>

#include <KLocalizedString>
#include <qnamespace.h>

#include "category.h"
#include "emoji.h"
#include "kemoji_logging.h"
#include "tones.h"

using namespace Qt::Literals::StringLiterals;
using namespace KEmoji;

constexpr inline auto RecentEmojiKey = "recentEmojis"_L1;
constexpr inline auto FavoriteEmojiKey = "favoriteEmojis"_L1;

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
    m_categories += categoryDict.value(recentCategoryID);
    m_categories += categoryDict.value(favoriteCategoryID);
    m_categories += categoryDict.value(allCategoryID);

    QFuture<void> future = QtConcurrent::run(&Dict::load, this).then([this]() {
        m_loaded = true;
        Q_EMIT loadedChanged();
    });

    QSettings settings("KDE"_L1, "KEmoji"_L1);

    auto size = settings.beginReadArray(RecentEmojiKey);
    for (qsizetype i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        m_recentEmojis += settings.value("emoji").value<Emoji>();
    }
    settings.endArray();

    size = settings.beginReadArray(FavoriteEmojiKey);
    for (qsizetype i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        const auto emoji = settings.value("emoji").value<Emoji>();
        const auto timesUsed = settings.value("timesUsed").toInt();
        m_favouriteEmojis += FavoriteEmoji{.emoji = emoji, .timesUsed = timesUsed};
    }
    settings.endArray();
}

bool Dict::loaded() const
{
    return m_loaded;
}

const QList<Emoji> &Dict::emojis() const
{
    return m_emojis;
}

int Dict::indexForEmoji(const KEmoji::Emoji &emoji) const
{
    return m_emojiIndicies.at(emoji.toString(Qt::RichText));
}

const KEmoji::EmojiGroup &Dict::familyGroupForEmoji(const KEmoji::Emoji &emoji) const
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

const QList<KEmoji::Emoji> &Dict::recentEmojis() const
{
    return m_recentEmojis;
}

int Dict::recentEmojiIndex(const KEmoji::Emoji &emoji) const
{
    return m_recentEmojis.indexOf(emoji);
}

const QList<KEmoji::FavoriteEmoji> &Dict::favoriteEmojis() const
{
    return m_favouriteEmojis;
}

int Dict::timesEmojiUsed(const KEmoji::Emoji &emoji) const
{
    auto index = m_favouriteEmojis.indexOf(emoji);
    if (index == -1) {
        return 0;
    }
    return m_favouriteEmojis[index].timesUsed;
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
    // We use a fixed version to keep it binary compatible.
    // Also we do not use advanced data type so it does not matter.
    stream.setVersion(QDataStream::Qt_5_15);
    // Explicitly set endianness to ensure it's not relevant to architecture.
    stream.setByteOrder(QDataStream::LittleEndian);
    QList<Emoji> emojis;
    stream >> emojis;

    if (m_emojis.capacity() < emojis.size()) {
        m_emojis.reserve(emojis.size());
    }

    std::ranges::for_each(emojis, [this](const Emoji &emoji) {
        QList<Emoji>::iterator it;
        if (m_emojis.contains(emoji)) {
            it = std::find(m_emojis.begin(), m_emojis.end(), emoji);
            // Overwrite with new data but keep previous name as fallback.
            auto &foundEmoji = *it;
            const QString fallbackName = foundEmoji.name();
            foundEmoji = Emoji(emoji.unicode(), emoji.unqualifiedUnicode(), emoji.name(), emoji.altNames(), emoji.category().name(), fallbackName);
        } else {
            it = m_emojis.insert(m_emojis.size(), emoji);
            m_emojiIndicies[emoji.toString(Qt::RichText)] = std::distance(m_emojis.begin(), it);
        }

        const auto tonelessEmoji = Tones::removeTonesFromEmoji(emoji);
        if (tonelessEmoji == emoji) {
            return;
        }
        if (m_emojiFamilyGroups.contains(tonelessEmoji.toString(Qt::RichText))) {
            auto &group = m_emojiFamilyGroups[tonelessEmoji.toString(Qt::RichText)];
            auto e = emoji;
            group += *it;
        } else {
            const auto baseIt = std::find(m_emojis.begin(), m_emojis.end(), tonelessEmoji);
            EmojiGroup group;
            group += *baseIt;
            group += *it;
            m_emojiFamilyGroups[baseIt->toString(Qt::RichText)] = group;
        }
    });
}

void Dict::emojiUsed(const Emoji &emoji)
{
    if (!m_emojis.contains(emoji)) {
        return;
    }

    QSettings settings("KDE"_L1, "KEmoji"_L1);

    const auto recentIndex = m_recentEmojis.indexOf(emoji);
    if (recentIndex >= 0) {
        m_recentEmojis.move(recentIndex, 0);
    } else {
        m_recentEmojis.prepend(emoji);
    }

    settings.beginWriteArray(RecentEmojiKey);
    for (qsizetype i = 0; i < m_recentEmojis.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("emoji", QVariant::fromValue(m_recentEmojis[i]));
    }
    settings.endArray();

    const auto favoriteIndex = m_favouriteEmojis.indexOf(emoji);
    if (favoriteIndex >= 0) {
        ++m_favouriteEmojis[favoriteIndex].timesUsed;
    } else {
        m_favouriteEmojis += FavoriteEmoji{.emoji = emoji, .timesUsed = 1};
    }

    settings.beginWriteArray(FavoriteEmojiKey);
    for (qsizetype i = 0; i < m_favouriteEmojis.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("emoji", QVariant::fromValue(m_favouriteEmojis[i].emoji));
        settings.setValue("timesUsed", m_favouriteEmojis[i].timesUsed);
    }
    settings.endArray();

    settings.sync();

    Q_EMIT recentEmojisChanged();
    Q_EMIT favoriteEmojisChanged();
}

#include "moc_dict.cpp"
