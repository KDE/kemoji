/*
    SPDX-FileCopyrightText: 2022 Weng Xuetian <wegnxt@gmail.com>
    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <QFile>
#include <QSettings>
#include <QStandardPaths>

#include <KLocalizedString>

#include "emojicategory.h"
#include "emojidict.h"
#include "kemoji_logging.h"

using namespace Qt::Literals::StringLiterals;

constexpr inline auto RecentEmojiKey = "recentEmojis"_L1;

inline QDataStream &operator>>(QDataStream &stream, Emoji &emoji)
{
    QByteArray buffer;
    stream >> buffer;
    emoji.unicode = QString::fromUtf8(buffer);
    stream >> buffer;
    emoji.description = QString::fromUtf8(buffer);
    stream >> buffer;
    emoji.setCategory(QString::fromUtf8(buffer));
    QList<QByteArray> annotationBuffers;
    stream >> annotationBuffers;
    for (const auto &annotation : annotationBuffers) {
        emoji.annotations << QString::fromUtf8(annotation);
    }
    return stream;
}

EmojiDict::EmojiDict(QObject *parent)
    : QObject(parent)
{
    load();
    initializeRecentEmojis();
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

void EmojiDict::load()
{
    m_emojis.clear();
    m_categories.clear();
    m_categories += categoryDict.at("recent"_L1);
    m_categories += categoryDict.at("all"_L1);

    const QHash<QString, QString> specialCases{{QLatin1String{"zh-TW"}, QLatin1String{"zh_Hant"}}, {QLatin1String{"zh-HK"}, QLatin1String{"zh_Hant_HK"}}};
    QLocale locale;
    QStringList dicts;
    auto bcp = locale.bcp47Name();
    bcp = specialCases.value(bcp, bcp);
    bcp.replace(QLatin1Char('-'), QLatin1Char('_'));

    const QString dictName = "/kemoji/%1.dict"_L1.arg(bcp);
    const auto path = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, dictName);
    if (!path.isEmpty()) {
        dicts << path;
    }

    for (qsizetype underscoreIndex = -1; (underscoreIndex = bcp.lastIndexOf(QLatin1Char('_'), underscoreIndex)) != -1; --underscoreIndex) {
        const QString genericDictName = "/kemoji/%1.dict"_L1.arg(bcp.left(underscoreIndex));
        const auto genericPath = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, genericDictName);

        if (!genericPath.isEmpty()) {
            dicts << genericPath;
        }
    }

    // Always fallback to en, because some annotation data only have minimum data.
    const auto genericPath = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, "kemoji/en.dict"_L1);
    dicts << genericPath;

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
        if (m_emojis.contains(emoji)) {
            const auto it = std::find(m_emojis.begin(), m_emojis.end(), emoji);
            // Overwrite with new data but keep previous description as fallback.
            auto &foundEmoji = *it;
            const QString fallbackDescription = foundEmoji.description;
            foundEmoji = emoji;
            foundEmoji.fallbackDescription = fallbackDescription;
        } else {
            m_emojis.append(emoji);
        }
    }
}

void EmojiDict::initializeRecentEmojis()
{
    QSettings settings("KDE"_L1, "KEmoji"_L1);
    auto recentEmojis = settings.value(RecentEmojiKey).toStringList();

    for (const auto &emoji : recentEmojis) {
        const auto index = m_emojis.indexOf(emoji);
        m_recentEmojis += m_emojis.value(index).unicode;
    }
}

void EmojiDict::addRecentEmoji(const Emoji &emoji)
{
    addRecentEmoji(emoji.unicode);
}

void EmojiDict::addRecentEmoji(const QString &emoji)
{
    if (!m_emojis.contains(emoji)) {
        return;
    }

    QSettings settings("KDE"_L1, "KEmoji"_L1);
    auto recentEmojis = settings.value(RecentEmojiKey).toStringList();
    const auto index = recentEmojis.indexOf(emoji);

    if (index >= 0) {
        recentEmojis.move(index, 0);
        m_recentEmojis.move(index, 0);
    } else {
        recentEmojis.prepend(emoji);
        m_recentEmojis.prepend(emoji);
    }

    settings.setValue(RecentEmojiKey, recentEmojis);
    settings.sync();

    Q_EMIT recentEmojisChanged();
}

#include "moc_emojidict.cpp"
