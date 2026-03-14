/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "emoji.h"

#include <KLazyLocalizedString>

using namespace KEmoji;

Emoji::Emoji(const QString &unicode)
    : m_unicode(unicode)
{
}

Emoji::Emoji(const QString &unicode,
             const QString &unqualifiedUnicode,
             const QString &name,
             const QStringList &altNames,
             const QString &category,
             const QString &fallbackName)
    : m_unicode(unicode)
    , m_unqualifiedUnicode(unqualifiedUnicode)
    , m_name(name)
    , m_fallbackName(fallbackName)
    , m_altNames(altNames)
    , m_category(category)
{
}

Emoji::Emoji(const QUrl &source)
    : m_source(source)
{
}

Emoji::Emoji(const QUrl &source,
             const QString &unqualifiedUnicode,
             const QString &name,
             const QStringList &altNames,
             const QString &category,
             const QString &fallbackName)
    : m_source(source)
    , m_unqualifiedUnicode(unqualifiedUnicode)
    , m_name(name)
    , m_fallbackName(fallbackName)
    , m_altNames(altNames)
    , m_category(category)
{
}

QString Emoji::unicode() const
{
    return m_unicode;
}

void Emoji::setUnicode(const QString &unicode)
{
    m_unicode = unicode;
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
    if (!categoryDict.contains(m_category)) {
        return emptyCategory;
    }
    return categoryDict.value(m_category);
}

QString Emoji::toString(Qt::TextFormat textFormat) const
{
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

QList<Emoji> EmojiGroup::filtered(std::function<bool(const Emoji &)> filter) const
{
    QList<Emoji> filteredEmojis;
    if (!filter) {
        std::ranges::for_each(m_emojiRefs, [&filteredEmojis](EmojiRef emojiRef) {
            filteredEmojis += emojiRef.get();
        });
        return filteredEmojis;
    }

    std::copy_if(m_emojiRefs.begin(), m_emojiRefs.end(), std::back_inserter(filteredEmojis), filter);
    return filteredEmojis;
}

bool EmojiGroup::isEmpty() const
{
    return m_emojiRefs.isEmpty();
}

QList<EmojiRef> &EmojiGroup::operator+=(EmojiRef emojiRef)
{
    return m_emojiRefs += emojiRef;
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
    stream << emoji.unicode().toUtf8() << emoji.unqualifiedUnicode().toUtf8() << emoji.name().toUtf8() << emoji.category().name().toUtf8();
    for (const auto &name : emoji.altNames()) {
        stream << name.toUtf8();
    }
    return stream;
}

QDataStream &operator>>(QDataStream &stream, Emoji &emoji)
{
    QByteArray buffer;
    stream >> buffer;
    const auto unicode = QString::fromUtf8(buffer);
    stream >> buffer;
    const auto unqualifiedUnicode = QString::fromUtf8(buffer);
    stream >> buffer;
    const auto name = QString::fromUtf8(buffer);
    stream >> buffer;
    const auto category = QString::fromUtf8(buffer);
    QList<QByteArray> annotationBuffers;
    stream >> annotationBuffers;
    QStringList altNames;
    for (const auto &annotation : annotationBuffers) {
        altNames << QString::fromUtf8(annotation);
    }
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
