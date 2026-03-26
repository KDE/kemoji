/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "emoji.h"
#include "category.h"

#include <QMimeDatabase>
#include <QSettings>
#include <QTextBoundaryFinder>

#include <KLazyLocalizedString>
#include <qcoreapplication.h>

using namespace KEmoji;

namespace
{
const QString _invalidUnicode = u"�"_s;
constexpr inline auto CustomEmojiKey = "customEmojis"_L1;
};

Emoji::Emoji(const QString &unicodeOrCustomName)
    : m_category(Category::None)
{
    setUnicode(unicodeOrCustomName);

    if (!isValid()) {
        const auto customEmojis = readCustomEmojis();
        if (customEmojis.contains(unicodeOrCustomName)) {
            m_name = unicodeOrCustomName;
            m_source = customEmojis[unicodeOrCustomName];
        }
    }
}

QString Emoji::id() const
{
    return isCustom() ? m_name : m_unicode;
}

QString Emoji::unicode() const
{
    if (!isValid() || !isUnicode()) {
        return _invalidUnicode;
    }
    return m_unicode;
}

void Emoji::setUnicode(const QString &unicode)
{
    if (!m_source.isEmpty()) {
        m_source.clear();
    }

    // If the QString is not a single unicode emoji invalidate the Emoji.
    QTextBoundaryFinder finder(QTextBoundaryFinder::Grapheme, unicode);
    qsizetype nextBoundary = finder.toNextBoundary();
    if (nextBoundary == -1) {
        // If here the string is empty.
        m_unicode.clear();
        m_source.clear();
        return;
    }
    nextBoundary = finder.toNextBoundary();
    if (nextBoundary != -1) {
        // If here there is more than 1 Grapheme.
        m_unicode.clear();
        m_source.clear();
        return;
    }

    m_unicode = unicode;
}

QUrl Emoji::source() const
{
    return m_source;
}

bool Emoji::validSource(const QUrl &source)
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

QHash<QString, QUrl> Emoji::readCustomEmojis()
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

void Emoji::writeCustomEmojis(const QHash<QString, QUrl> &customEmojis)
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

bool Emoji::registerCustomEmoji(const QUrl &source, const QString &name)
{
    if (!validSource(source) || name.isEmpty()) {
        return false;
    }

    auto customEmojis = readCustomEmojis();
    if (customEmojis.contains(name) && customEmojis[name] == source) {
        return true;
    }
    customEmojis[name] = source;
    writeCustomEmojis(customEmojis);
    return true;
}

bool Emoji::unregisterCustomEmoji(const QString &name)
{
    if (name.isEmpty()) {
        return false;
    }

    auto customEmojis = readCustomEmojis();
    if (!customEmojis.contains(name)) {
        return false;
    }
    customEmojis.remove(name);
    writeCustomEmojis(customEmojis);
    return true;
}

QString Emoji::unqualifiedUnicode() const
{
    return m_unqualifiedUnicode;
}

void Emoji::setUnqualifiedUnicode(const QString &unqualifiedUnicode)
{
    m_unqualifiedUnicode = unqualifiedUnicode;
}

bool Emoji::isValid() const
{
    return isUnicode() || isCustom();
}

bool Emoji::isUnicode() const
{
    return !m_unicode.isEmpty();
}

bool Emoji::isCustom() const
{
    return m_source.isValid();
}

QString Emoji::name() const
{
    return m_name;
}

void Emoji::setName(const QString &name)
{
    if (name == m_name) {
        return;
    }
    // If this is a custom emoji name set the source and clear any unicode out.
    const auto customEmojis = readCustomEmojis();
    if (customEmojis.contains(name)) {
        m_source = customEmojis[name];
        m_unicode.clear();
    }

    m_name = name;
    return;
}

QString Emoji::fallbackName() const
{
    return m_fallbackName;
}

void Emoji::setFallbackName(const QString &fallbackName)
{
    m_fallbackName = fallbackName;
}

QStringList Emoji::altNames() const
{
    return m_altNames;
}

void Emoji::setAltNames(const QStringList &altNames)
{
    m_altNames = altNames;
}

Category Emoji::category() const
{
    return Category(m_category);
}

void Emoji::setCategory(Category::Categories category)
{
    m_category = category;
}

QString Emoji::toString(Qt::TextFormat textFormat) const
{
    if (!isValid() || (textFormat == Qt::PlainText && isCustom())) {
        return _invalidUnicode;
    }
    if (!m_unicode.isEmpty() || textFormat == Qt::PlainText) {
        return m_unicode;
    }
    return u"<img title=\"%2\" src=\"%1\">"_s.arg(m_source.toString(), m_name);
}

bool Emoji::operator==(const Emoji &right) const
{
    return m_unicode == right.unicode() || m_unqualifiedUnicode == right.unicode();
}

bool Emoji::operator==(const QString &right) const
{
    return m_unicode == right || m_unqualifiedUnicode == right;
}

bool FavoriteEmoji::operator==(const FavoriteEmoji &right) const
{
    return emoji == right.emoji;
}

bool FavoriteEmoji::operator==(const Emoji &right) const
{
    return emoji == right;
}

QDataStream &operator<<(QDataStream &stream, const KEmoji::Emoji &emoji)
{
    stream << emoji.unicode() << emoji.unqualifiedUnicode() << emoji.name() << (qint32)emoji.category().id();
    for (const auto &name : emoji.altNames()) {
        stream << name;
    }
    return stream;
}

QDataStream &operator>>(QDataStream &stream, Emoji &emoji)
{
    QString unicode;
    QString unqualifiedUnicode;
    QString name;
    qint32 categoryInt;
    stream >> unicode >> unqualifiedUnicode >> name >> categoryInt;
    const auto category = static_cast<Category::Categories>(categoryInt);
    QStringList altNames;
    stream >> altNames;

    emoji = Emoji(unicode);
    if (!emoji.isValid()) {
        return stream;
    }

    emoji.setUnqualifiedUnicode(unqualifiedUnicode);
    emoji.setName(name);
    emoji.setAltNames(altNames);
    emoji.setCategory(category);
    return stream;
}

QDebug operator<<(QDebug debug, const KEmoji::Emoji &emoji)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "Emoji(Unicode: " << emoji.unicode();
    debug.nospace() << ", Name: " << emoji.name() << ')';

    return debug;
}

#include "moc_emoji.cpp"
