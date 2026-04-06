/*
 *    SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "settings.h"

#include <QCoreApplication>
#include <QMimeDatabase>
#include <QSettings>

using namespace Qt::Literals::StringLiterals;
using namespace KEmoji;

namespace
{
constexpr inline auto OrganizationKey = "KDE"_L1;
constexpr inline auto KEmojiKey = "KEmoji"_L1;
constexpr inline auto RecentEmojiKey = "recentEmojis"_L1;
constexpr inline auto CustomEmojiKey = "customEmojis"_L1;
};

Settings::Settings(QObject *parent)
    : QObject(parent)
{
    initialize();
}

Settings &Settings::instance()
{
    static Settings _instance;
    return _instance;
}

void Settings::initialize()
{
    m_customEmojis = readCustomEmojis();

    QSettings settings(OrganizationKey, KEmojiKey);
    m_recent = settings.value(RecentEmojiKey).toStringList();

    std::ranges::for_each(m_recent, [this](const QString &emoji) {
        if (m_timesUsed.contains(emoji)) {
            ++m_timesUsed[emoji];
            return;
        }
        m_timesUsed[emoji] = 1;
    });

    Q_EMIT recentEmojisChanged();
    Q_EMIT favoriteEmojisChanged();
}

void Settings::emojiUsed(const QString &emoji)
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

    Q_EMIT recentEmojisChanged();
    Q_EMIT favoriteEmojisChanged();
}

void Settings::clearUsedEmojis()
{
    QSettings settings(OrganizationKey, KEmojiKey);
    m_recent.clear();
    m_timesUsed.clear();
    settings.remove(RecentEmojiKey);
    settings.sync();

    Q_EMIT recentEmojisChanged();
    Q_EMIT favoriteEmojisChanged();
}

bool Settings::isRecent(const QString &emoji) const
{
    return m_recent.contains(emoji);
}

int Settings::recentIndex(const QString &emoji) const
{
    return m_recent.indexOf(emoji);
}

int Settings::timesUsed(const QString &emoji) const
{
    return m_timesUsed.value(emoji, 0);
}

bool Settings::validSource(const QUrl &source)
{
    if (!source.isValid() || !source.isLocalFile()) {
        return false;
    }
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForUrl(source);
    if (!mime.isValid() || !mime.name().contains(u"image"_s)) {
        return false;
    }

    return true;
}

QHash<QString, QUrl> Settings::readCustomEmojis()
{
    QHash<QString, QUrl> customEmojis;
    QSettings settings("KDE"_L1, "KEmoji"_L1);
    auto size = settings.beginReadArray("%1-%2"_L1.arg(CustomEmojiKey, QCoreApplication::applicationName()));
    for (qsizetype i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        customEmojis[settings.value("name").toString()] = settings.value("source").toUrl();
    }
    settings.endArray();
    return customEmojis;
}

void Settings::writeCustomEmojis(const QHash<QString, QUrl> &customEmojis)
{
    QSettings settings("KDE"_L1, "KEmoji"_L1);
    const auto setting = "%1-%2"_L1.arg(CustomEmojiKey, QCoreApplication::applicationName());
    settings.remove(setting);
    settings.beginWriteArray(setting);
    for (qsizetype i = 0; i < customEmojis.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("name", customEmojis.keys()[i]);
        settings.setValue("source", customEmojis[customEmojis.keys()[i]]);
    }
    settings.endArray();
}

bool Settings::registerCustomEmoji(const QUrl &source, const QString &name)
{
    if (!validSource(source) || name.isEmpty()) {
        return false;
    }

    if (m_customEmojis.contains(name) && m_customEmojis[name] == source) {
        return true;
    }
    m_customEmojis[name] = source;
    writeCustomEmojis(m_customEmojis);
    return true;
}

bool Settings::unregisterCustomEmoji(const QString &name)
{
    if (name.isEmpty()) {
        return false;
    }

    if (!m_customEmojis.contains(name)) {
        return false;
    }
    m_customEmojis.remove(name);
    writeCustomEmojis(m_customEmojis);
    return true;
}

const QHash<QString, QUrl> &Settings::customEmojis() const
{
    return m_customEmojis;
}

bool Settings::isCustomEmoji(const QString &name) const
{
    return m_customEmojis.contains(name);
}

QUrl Settings::customEmojiSource(const QString &name) const
{
    return m_customEmojis.value(name, {});
}

#include "moc_settings.cpp"
