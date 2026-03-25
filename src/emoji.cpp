/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "emoji.h"
#include "category.h"

#include <QMimeDatabase>
#include <QTextBoundaryFinder>

#include <KLazyLocalizedString>

using namespace KEmoji;

namespace
{
const QString _invalidUnicode = u"�"_s;
};

Emoji::Emoji(const QString &unicode)
    : m_category(Category::None)
{
    setUnicode(unicode);
}

Emoji::Emoji(const QString &unicode,
             const QString &unqualifiedUnicode,
             const QString &name,
             const QStringList &altNames,
             Category::Categories category,
             const QString &fallbackName)
    : m_unqualifiedUnicode(unqualifiedUnicode)
    , m_name(name)
    , m_fallbackName(fallbackName)
    , m_altNames(altNames)
    , m_category(category)
{
    setUnicode(unicode);
}

Emoji::Emoji(const QUrl &source)
    : m_category(Category::None)
{
    setSource(source);
}

Emoji::Emoji(const QUrl &source,
             const QString &unqualifiedUnicode,
             const QString &name,
             const QStringList &altNames,
             Category::Categories category,
             const QString &fallbackName)
    : m_unqualifiedUnicode(unqualifiedUnicode)
    , m_name(name)
    , m_fallbackName(fallbackName)
    , m_altNames(altNames)
    , m_category(category)
{
    setSource(source);
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
        m_unicode = _invalidUnicode;
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

void Emoji::setSource(const QUrl &source)
{
    if (!m_unicode.isEmpty()) {
        m_unicode.clear();
    }

    if (!source.isValid() || !source.isLocalFile()) {
        m_unicode.clear();
        m_source.clear();
        return;
    }
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForUrl(source);
    if (!mime.isValid() || !mime.name().contains(u"image"_s)) {
        m_unicode.clear();
        m_source.clear();
        return;
    }

    m_source = source;
}

QString Emoji::unqualifiedUnicode() const
{
    return m_unqualifiedUnicode;
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

QString Emoji::fallbackName() const
{
    return m_fallbackName;
}

QStringList Emoji::altNames() const
{
    return m_altNames;
}

Category Emoji::category() const
{
    return Category(m_category);
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
    emoji = Emoji(unicode, unqualifiedUnicode, name, altNames, category);
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
