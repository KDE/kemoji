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
constexpr inline auto CustomEmojiKey = "customEmojis"_L1;
};

Settings::Settings(QObject *parent)
    : QObject(parent)
{
    m_customEmojis = readCustomEmojis();
}

Settings &Settings::instance()
{
    static Settings _instance;
    return _instance;
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
