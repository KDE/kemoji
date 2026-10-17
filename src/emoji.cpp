/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "emoji.h"

#include <QFont>
#include <QRawFont>
#include <QTextBoundaryFinder>
#include <QTextLayout>

#include <KLazyLocalizedString>

#include "category.h"
#include "emojidata_p.h"

using namespace KEmoji;

namespace
{
const QString _invalidUnicode = u"�"_s;
};

Emoji::Emoji()
{
}

Emoji::Emoji(const QString &unicodeOrCustomName)
{
    const auto ePrivate = EmojiData::instance().dataForEmoji(unicodeOrCustomName);
    if (ePrivate) {
        d = *ePrivate;
    }
}

Emoji::Emoji(const Emoji &other)
    : d(other.d)
{
}

Emoji::~Emoji()
{
}

QString Emoji::id() const
{
    return isCustom() ? name() : unicode();
}

QString Emoji::unicode() const
{
    return isValid() && isUnicode() ? d->unicode : _invalidUnicode;
}

QUrl Emoji::source() const
{
    return isValid() ? d->source : QUrl();
}

QString Emoji::unqualifiedUnicode() const
{
    return isValid() ? d->unqualifiedUnicode : QString();
}

bool Emoji::isValid() const
{
    return d != nullptr && (isUnicode() || isCustom());
}

bool Emoji::isUnicode() const
{
    return d != nullptr && !d->unicode.isEmpty();
}

bool Emoji::isCustom() const
{
    return d != nullptr && d->source.isValid();
}

QString Emoji::name() const
{
    return isValid() ? d->name : QString();
}

QString Emoji::fallbackName() const
{
    return isValid() ? d->fallbackName : QString();
}

QStringList Emoji::altNames() const
{
    return isValid() ? d->altNames : QStringList();
}

Categories::Category Emoji::category() const
{
    return isValid() ? d->category : Categories::None;
}

QString Emoji::toString(Qt::TextFormat textFormat) const
{
    if (!isValid() || (textFormat == Qt::PlainText && isCustom())) {
        return _invalidUnicode;
    }
    if (!d->unicode.isEmpty() || textFormat == Qt::PlainText) {
        return d->unicode;
    }
    return u"<img title=\"%2\" src=\"%1\" />"_s.arg(d->source.toString(), d->name);
}

Emoji &Emoji::operator=(const Emoji &other)
{
    d = other.d;
    return *this;
}

bool Emoji::operator==(const Emoji &right) const
{
    return isCustom() ? name() == right.name() : unicode() == right.unicode() || unqualifiedUnicode() == right.unicode();
}

bool Emoji::operator==(const QString &right) const
{
    return isCustom() ? name() == right : unicode() == right || unqualifiedUnicode() == right;
}

bool FavoriteEmoji::operator==(const FavoriteEmoji &right) const
{
    return emoji == right.emoji;
}

bool FavoriteEmoji::operator==(const Emoji &right) const
{
    return emoji == right;
}

QDebug operator<<(QDebug debug, const KEmoji::Emoji &emoji)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "Emoji(Unicode: " << emoji.unicode();
    debug.nospace() << ", Name: " << emoji.name() << ')';

    return debug;
}

#include "moc_emoji.cpp"
