/*
    SPDX-FileCopyrightText: 2022 Weng Xuetian <wegnxt@gmail.com>
    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "emojidict.h"

#include <QFile>
#include <QLocale>
#include <QSettings>
#include <QStandardPaths>

#include <KLocalizedString>

#include "category.h"
#include "emoji.h"
#include "kemoji_logging.h"
#include "tones.h"

using namespace Qt::Literals::StringLiterals;
using namespace KEmoji;

constexpr inline auto RecentEmojiKey = "recentEmojis"_L1;
constexpr inline auto FavoriteEmojiKey = "favoriteEmojis"_L1;

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

const QList<KEmoji::Emoji> EmojiDict::emojisForCategory(KEmoji::Category category) const
{
    QList<Emoji> emojis;
    std::copy_if(m_emojis.begin(), m_emojis.end(), std::back_inserter(emojis), [category](Emoji emoji) {
        return emoji.category() == category;
    });
    return emojis;
}

const QList<KEmoji::RecentEmoji> &EmojiDict::recentEmojis() const
{
    return m_recentEmojis;
}

int EmojiDict::recentEmojiIndex(const KEmoji::Emoji &emoji) const
{
    auto index = m_recentEmojis.indexOf(emoji);
    if (index == -1) {
        for (const auto &subEmoji : emoji.subEmojis()) {
            index = m_recentEmojis.indexOf(subEmoji);
            if (index != -1) {
                break;
            }
        }
    }
    return index;
}

const QList<KEmoji::FavoriteEmoji> &EmojiDict::favoriteEmojis() const
{
    return m_favouriteEmojis;
}

int EmojiDict::timesEmojiUsed(const KEmoji::Emoji &emoji) const
{
    auto index = m_favouriteEmojis.indexOf(emoji);
    if (index == -1) {
        for (const auto &subEmoji : emoji.subEmojis()) {
            index = m_favouriteEmojis.indexOf(subEmoji);
            if (index != -1) {
                break;
            }
        }
    }
    if (index == -1) {
        return -1;
    }
    return m_favouriteEmojis[index].timesUsed;
}

void EmojiDict::load()
{
    m_emojis.clear();
    m_categories.clear();
    m_categories += categoryDict.value(recentCategoryID);
    m_categories += categoryDict.value(favoriteCategoryID);
    m_categories += categoryDict.value(allCategoryID);

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
                return emoji.baseUnicode() == listEmoji.unicode() || emoji.baseUnicode() == listEmoji.unqualifiedUnicode();
            });
            if (it == m_emojis.end()) {
                qCWarning(KEMOJI) << "Sub emoji parent not found" << emoji.unicode() << emoji.unqualifiedUnicode() << emoji.name() << emoji.baseUnicode();
                continue;
            }
            if (it->subEmojis().contains(emoji)) {
                auto &foundEmoji = it->subEmojis()[it->subEmojis().indexOf(emoji)];
                const QString fallbackName = foundEmoji.name();
                foundEmoji = Emoji(emoji.unicode(), emoji.unqualifiedUnicode(), emoji.name(), emoji.altNames(), emoji.category().name(), fallbackName);
            } else {
                it->addSubEmoji(emoji);
            }
        } else if (m_emojis.contains(emoji)) {
            const auto it = std::find(m_emojis.begin(), m_emojis.end(), emoji);
            // Overwrite with new data but keep previous name as fallback.
            auto &foundEmoji = *it;
            const QString fallbackName = foundEmoji.name();
            foundEmoji = Emoji(emoji.unicode(), emoji.unqualifiedUnicode(), emoji.name(), emoji.altNames(), emoji.category().name(), fallbackName);
        } else {
            m_emojis.append(emoji);
        }
    }
}

void EmojiDict::initialize()
{
    QSettings settings("KDE"_L1, "KEmoji"_L1);

    auto size = settings.beginReadArray(RecentEmojiKey);
    for (qsizetype i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        const auto emoji = settings.value("emoji").value<Emoji>();
        const auto subEmojiIndex = settings.value("subEmojiIndex").toInt();
        m_recentEmojis += RecentEmoji{.emoji = emoji, .subEmojiIndex = subEmojiIndex};
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

void EmojiDict::emojiUsed(const Emoji &emoji)
{
    if (!m_emojis.contains(emoji) && !m_emojis.contains(emoji.baseUnicode())) {
        return;
    }

    QSettings settings("KDE"_L1, "KEmoji"_L1);

    const auto recentIndex = m_recentEmojis.indexOf(emoji);
    if (recentIndex >= 0) {
        m_recentEmojis[recentIndex].subEmojiIndex = m_recentEmojis[recentIndex].emoji.indexForSubEmoji(emoji);
        m_recentEmojis.move(recentIndex, 0);
    } else {
        RecentEmoji newRecent{.emoji = emoji};
        if (emoji.isSubEmoji()) {
            const auto baseIndex = m_emojis.indexOf(emoji.baseUnicode());
            if (baseIndex > -1) {
                newRecent.emoji = m_emojis[baseIndex];
                newRecent.subEmojiIndex = newRecent.emoji.indexForSubEmoji(emoji);
            }
        }
        m_recentEmojis.prepend(newRecent);
    }

    settings.beginWriteArray(RecentEmojiKey);
    for (qsizetype i = 0; i < m_recentEmojis.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("emoji", QVariant::fromValue(m_recentEmojis[i].emoji));
        settings.setValue("subEmojiIndex", m_recentEmojis[i].subEmojiIndex);
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

#include "moc_emojidict.cpp"
