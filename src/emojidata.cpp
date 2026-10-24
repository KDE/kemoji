/*
 *    SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "category.h"
#include "emojidata_p.h"

#include <QCoreApplication>
#include <QFile>
#include <QLocale>
#include <QMimeDatabase>
#include <QSettings>
#include <QStandardPaths>
#include <QtConcurrent/QtConcurrentRun>

#include "kemoji_logging.h"

using namespace Qt::Literals::StringLiterals;
using namespace KEmoji;

namespace
{
constexpr quint32 BINARYMAGIC = 0x656D6F6A;
constexpr quint32 MINBINARYVER = 001;
constexpr quint32 MAXBINARYVER = 001;
constexpr QDataStream::Version VERSION1 = QDataStream::Qt_6_10;

constexpr inline auto OrganizationKey = "KDE"_L1;
constexpr inline auto KEmojiKey = "KEmoji"_L1;
constexpr inline auto RecentEmojiKey = "recentEmojis"_L1;
constexpr inline auto CustomEmojiKey = "customEmojis"_L1;
}

EmojiData::EmojiData(QObject *parent)
    : QObject(parent)
{
    QSettings settings(OrganizationKey, KEmojiKey);
    m_recent = settings.value(RecentEmojiKey).toStringList();
    std::ranges::for_each(m_recent, [this](const QString &emoji) {
        if (m_timesUsed.contains(emoji)) {
            ++m_timesUsed[emoji];
            return;
        }
        m_timesUsed[emoji] = 1;
    });
    Q_EMIT emojiHistoryChanged(m_recent);

    loadCustom();
    m_loading = true;
    Q_EMIT loadingChanged();
    QFuture<void> future = QtConcurrent::run(&EmojiData::load, this).then([this]() {
        m_loading = false;
        Q_EMIT loadingChanged();
    });
}

EmojiData &EmojiData::instance()
{
    static EmojiData _instance;
    return _instance;
}

bool EmojiData::loading() const
{
    return m_loading;
}

std::optional<QExplicitlySharedDataPointer<EmojiPrivate>> EmojiData::dataForEmoji(const QString &unicodeOrCustomName)
{
    if (!m_data.contains(unicodeOrCustomName)) {
        return std::nullopt;
    }
    return m_data[unicodeOrCustomName];
}

bool EmojiData::hasDataForEmoji(const QString &unicodeOrCustomName)
{
    return m_data.contains(unicodeOrCustomName);
}

void EmojiData::emojiUsed(const QString &emoji)
{
    QSettings settings(OrganizationKey, KEmojiKey);
    m_recent.prepend(emoji);
    settings.setValue(RecentEmojiKey, m_recent);
    settings.sync();

    if (m_timesUsed.contains(emoji)) {
        ++m_timesUsed[emoji];
    } else {
        m_timesUsed[emoji] = 1;
    }

    Q_EMIT emojiHistoryChanged({emoji});
}

void EmojiData::clearHistory()
{
    QSettings settings(OrganizationKey, KEmojiKey);
    const auto clearedEmojis = m_recent;
    m_recent.clear();
    m_timesUsed.clear();
    settings.remove(RecentEmojiKey);
    settings.sync();

    Q_EMIT emojiHistoryChanged(clearedEmojis);
}

bool EmojiData::isRecent(const QString &emoji) const
{
    return m_recent.contains(emoji);
}

int EmojiData::recentIndex(const QString &emoji) const
{
    return m_recent.indexOf(emoji);
}

int EmojiData::timesUsed(const QString &emoji) const
{
    return m_timesUsed.value(emoji, 0);
}

void EmojiData::load()
{
    const QHash<QString, QString> specialCases{{"zh-TW"_L1, "zh_Hant"_L1}, {"zh-HK"_L1, "zh_Hant_HK"_L1}};
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
    std::ranges::for_each(dicts, [this](const QString &path) {
        loadDict(path);
    });
}

void EmojiData::loadDict(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }
    auto buffer = file.readAll();
    buffer = qUncompress(buffer);
    QDataStream stream(&buffer, QIODevice::ReadOnly);
    quint32 magic;
    stream >> magic;
    if (magic != BINARYMAGIC) {
        qCWarning(KEMOJI) << "Bad dictionary file magic number does not match" << path;
    }
    quint32 version;
    stream >> version;
    if (version < MINBINARYVER) {
        qCWarning(KEMOJI) << "Bad dictionary file magic version too old" << path;
    } else if (version > MAXBINARYVER) {
        qCWarning(KEMOJI) << "Bad dictionary file magic version too new" << path;
    }
    stream.setVersion(VERSION1);

    int length;
    stream >> length;

    QStringList addedEmojis;
    while (length > 0) {
        QString unicode;
        stream >> unicode;
        const auto hasEmoji = m_data.contains(unicode);
        auto foundEmoji = hasEmoji ? m_data[unicode] : QExplicitlySharedDataPointer(new EmojiPrivate);
        if (hasEmoji) {
            foundEmoji->fallbackName = foundEmoji->name;
        }
        foundEmoji->unicode = unicode;
        qint32 categoryInt;
        stream >> foundEmoji->unqualifiedUnicode >> foundEmoji->name >> categoryInt >> foundEmoji->altNames;
        foundEmoji->category = static_cast<Categories::Category>(categoryInt);
        if (!hasEmoji) {
            m_data[foundEmoji->unicode] = foundEmoji;
        }
        addedEmojis += foundEmoji->unicode;
        --length;
    }
    Q_EMIT emojisAdded(addedEmojis);
}

void EmojiData::loadCustom()
{
    QStringList addedEmojis;
    QSettings settings(OrganizationKey, KEmojiKey);
    auto size = settings.beginReadArray("%1-%2"_L1.arg(CustomEmojiKey, QCoreApplication::applicationName()));
    for (qsizetype i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        const auto name = settings.value("name").toString();
        auto newEmoji = m_data.insert(name, QExplicitlySharedDataPointer(new EmojiPrivate));
        newEmoji->data()->source = settings.value("source").toUrl();
        addedEmojis += name;
    }
    settings.endArray();

    Q_EMIT emojisAdded(addedEmojis);
}

bool EmojiData::validCustomSource(const QUrl &source)
{
    if (!source.isValid()) {
        return false;
    }
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForUrl(source);
    if (source.isLocalFile() && (!mime.isValid() || !mime.name().contains(u"image"_s))) {
        return false;
    }

    return true;
}

bool EmojiData::registerCustomEmoji(const QUrl &source, const QString &name)
{
    if (!validCustomSource(source) || name.isEmpty()) {
        return false;
    }

    if (m_data.contains(name) && m_data[name]->source == source) {
        return true;
    }
    auto newEmoji = m_data.insert(name, QExplicitlySharedDataPointer(new EmojiPrivate));
    newEmoji->data()->name = name;
    newEmoji->data()->source = source;
    newEmoji->data()->category = Categories::Custom;

    QSettings settings(OrganizationKey, KEmojiKey);
    const auto setting = "%1-%2"_L1.arg(CustomEmojiKey, QCoreApplication::applicationName());
    auto size = settings.beginReadArray("%1-%2"_L1.arg(CustomEmojiKey, QCoreApplication::applicationName()));
    settings.endArray();
    settings.beginWriteArray(setting);
    settings.setArrayIndex(size);
    settings.setValue("name", name);
    settings.setValue("source", source);
    settings.endArray();
    Q_EMIT emojisAdded({name});
    return true;
}

bool EmojiData::unregisterCustomEmoji(const QString &name)
{
    if (name.isEmpty()) {
        return false;
    }

    if (!m_data.contains(name)) {
        return false;
    }
    m_data.remove(name);

    QHash<QString, QUrl> customData;
    QSettings settings(OrganizationKey, KEmojiKey);
    const auto setting = "%1-%2"_L1.arg(CustomEmojiKey, QCoreApplication::applicationName());
    auto size = settings.beginReadArray(setting);
    for (qsizetype i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        customData[settings.value("name").toString()] = settings.value("source").toUrl();
    }
    settings.endArray();
    settings.remove(setting);

    customData.remove(name);
    settings.beginWriteArray(setting);
    for (qsizetype i = 0; i < customData.keys().size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("name", customData.keys()[i]);
        settings.setValue("source", customData[customData.keys()[i]]);
    }
    settings.endArray();
    Q_EMIT emojisRemoved({name});
    return true;
}

#include "moc_emojidata_p.cpp"
