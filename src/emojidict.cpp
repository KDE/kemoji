/*
    SPDX-FileCopyrightText: 2022 Weng Xuetian <wegnxt@gmail.com>
    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <QFile>
#include <QLocale>
#include <QSettings>
#include <QStandardPaths>

#include <KLocalizedString>

#include "emoji.h"
#include "emojicategory.h"
#include "emojidict.h"
#include "kemoji_logging.h"

using namespace Qt::Literals::StringLiterals;
using namespace KEmoji;

constexpr inline auto RecentEmojiKey = "recentEmojis"_L1;
constexpr inline auto FavoriteEmojiKey = "favoriteEmojis"_L1;

inline QDataStream &operator>>(QDataStream &stream, Emoji &emoji)
{
    QByteArray buffer;
    stream >> buffer;
    emoji.unicode = QString::fromUtf8(buffer);
    stream >> buffer;
    emoji.unqualifiedUnicode = QString::fromUtf8(buffer);
    stream >> buffer;
    emoji.name = QString::fromUtf8(buffer);
    stream >> buffer;
    emoji.setCategory(QString::fromUtf8(buffer));
    QList<QByteArray> annotationBuffers;
    stream >> annotationBuffers;
    for (const auto &annotation : annotationBuffers) {
        emoji.altNames << QString::fromUtf8(annotation);
    }
    return stream;
}

EmojiDict::EmojiDict(QObject *parent)
    : QObject(parent)
{
    load();
    initialize();
}

EmojiDict &EmojiDict::instance()
{
    static EmojiDict _instance;
    return _instance;
}

const QList<Emoji> &EmojiDict::emojis() const
{
    return m_emojis;
}

const QList<Category> &EmojiDict::categories() const
{
    return m_categories;
}

const QStringList &EmojiDict::recentEmojis() const
{
    return m_recentEmojis;
}

int EmojiDict::recentEmojiIndex(const KEmoji::Emoji &emoji) const
{
    return recentEmojiIndex(emoji.unicode);
}

int EmojiDict::recentEmojiIndex(const QString &emoji) const
{
    return m_recentEmojis.indexOf(emoji);
}

const QHash<QString, int> &EmojiDict::favoriteEmojis() const
{
    return m_favouriteEmojis;
}

int EmojiDict::timesEmojiUsed(const KEmoji::Emoji &emoji) const
{
    return timesEmojiUsed(emoji.unicode);
}

int EmojiDict::timesEmojiUsed(const QString &emoji) const
{
    return m_favouriteEmojis.value(emoji, -1);
}

void EmojiDict::load()
{
    m_emojis.clear();
    m_categories.clear();
    m_categories += categoryDict.at(recentCategoryID);
    m_categories += categoryDict.at(favoriteCategoryID);
    m_categories += categoryDict.at(allCategoryID);

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
    if (!dicts.contains(genericPath)) {
        dicts << genericPath;
    }

    if (dicts.isEmpty()) {
        qCWarning(KEMOJI) << "could not find emoji dictionaries." << dictName;
        return;
    }

    // We load in reverse order, because we want to preserve the order in en.dict.
    // en.dict almost always gives complete set of data.
    for (auto iter = dicts.crbegin(); iter != dicts.crend(); ++iter) {
        loadDict(*iter);
    }
    for (const auto &emoji : m_emojis) {
        if (!m_categories.contains(emoji.category())) {
            m_categories += emoji.category();
        }
    }
}

void EmojiDict::loadDict(const QString &path)
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
    for (const auto &emoji : emojis) {
        if (emoji.isSubEmoji()) {
            const auto it = std::find_if(m_emojis.begin(), m_emojis.end(), [emoji](const Emoji &listEmoji) {
                return emoji.baseUnicode() == listEmoji.unicode || emoji.baseUnicode() == listEmoji.unqualifiedUnicode;
            });
            if (it == m_emojis.end()) {
                qCWarning(KEMOJI) << "Sub emoji parent not found" << emoji.unicode << emoji.unqualifiedUnicode << emoji.name << emoji.baseUnicode();
                continue;
            }
            if (it->subEmojis.contains(emoji)) {
                auto &foundEmoji = it->subEmojis[it->subEmojis.indexOf(emoji)];
                const QString fallbackName = foundEmoji.name;
                foundEmoji = emoji;
                foundEmoji.fallbackName = fallbackName;
            } else {
                it->subEmojis.append(emoji);
            }
        } else if (m_emojis.contains(emoji)) {
            const auto it = std::find(m_emojis.begin(), m_emojis.end(), emoji);
            // Overwrite with new data but keep previous name as fallback.
            auto &foundEmoji = *it;
            const QString fallbackName = foundEmoji.name;
            foundEmoji = emoji;
            foundEmoji.fallbackName = fallbackName;
        } else {
            m_emojis.append(emoji);
        }
    }
}

void EmojiDict::initialize()
{
    QSettings settings("KDE"_L1, "KEmoji"_L1);
    m_recentEmojis = settings.value(RecentEmojiKey).toStringList();

    auto favoriteEmojis = settings.value(FavoriteEmojiKey).toHash();
    for (const auto &key : favoriteEmojis.keys()) {
        m_favouriteEmojis[key] = favoriteEmojis[key].toInt();
    }
}

void EmojiDict::emojiUsed(const Emoji &emoji)
{
    emojiUsed(emoji.unicode);
}

void EmojiDict::emojiUsed(const QString &emoji)
{
    if (!m_emojis.contains(emoji)) {
        return;
    }

    QSettings settings("KDE"_L1, "KEmoji"_L1);

    auto recentEmojis = settings.value(RecentEmojiKey).toStringList();
    const auto recentIndex = recentEmojis.indexOf(emoji);
    if (recentIndex >= 0) {
        recentEmojis.move(recentIndex, 0);
        m_recentEmojis.move(recentIndex, 0);
    } else {
        recentEmojis.prepend(emoji);
        m_recentEmojis.prepend(emoji);
    }
    settings.setValue(RecentEmojiKey, recentEmojis);

    auto favoriteEmojisVariant = settings.value(FavoriteEmojiKey).toHash();
    QHash<QString, int> favoriteEmojis;
    for (const auto &key : favoriteEmojisVariant.keys()) {
        favoriteEmojis[key] = favoriteEmojisVariant[key].toInt();
    }
    if (favoriteEmojis.contains(emoji)) {
        ++favoriteEmojis[emoji];
        favoriteEmojisVariant[emoji] = favoriteEmojisVariant.value(emoji).toInt() + 1;
        ++m_favouriteEmojis[emoji];
    } else {
        favoriteEmojis[emoji] = 1;
        favoriteEmojisVariant[emoji] = 1;
        m_favouriteEmojis[emoji] = 1;
    }
    settings.setValue(FavoriteEmojiKey, favoriteEmojisVariant);

    settings.sync();

    Q_EMIT recentEmojisChanged();
    Q_EMIT favoriteEmojisChanged();
}

#include "moc_emojidict.cpp"
