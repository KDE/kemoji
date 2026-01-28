/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "emoji.h"
#include "tones.h"

#include <KLazyLocalizedString>

using namespace KEmoji;

namespace
{
const QHash<QString, QString> _specialBaseCases = {
    {u"🫱\u200D🫲"_s, u"🤝"_s},
    {u"🕵\u200D♂️"_s, u"🕵️‍♂️"_s},
    {u"🕵\u200D♀️"_s, u"🕵️‍♀️"_s},
    {u"🏌\u200D♂️"_s, u"🏌️‍♂️"_s},
    {u"🏌\u200D♀️"_s, u"🏌️‍♀️"_s},
    {u"⛹\u200D♂️"_s, u"⛹️‍♂️"_s},
    {u"⛹\u200D♀️"_s, u"⛹️‍♀️"_s},
    {u"🏋\u200D♂️"_s, u"🏋️‍♂️"_s},
    {u"🏋\u200D♀️"_s, u"🏋️‍♀️"_s},
    {u"👩\u200D🤝\u200D👩"_s, u"👭"_s},
    {u"👩\u200D🤝\u200D👨"_s, u"👫"_s},
    {u"👨\u200D🤝\u200D👨"_s, u"👬"_s},
    {u"🧑\u200D❤️\u200D💋\u200D🧑"_s, u"💏"_s},
    {u"🧑\u200D❤️\u200D🧑"_s, u"💑"_s},
};
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

QString Emoji::unicode(Tones::Tone defaultTone) const
{
    auto toneSubEmojis = subEmojis(defaultTone);
    if (toneSubEmojis.length() > 0 && defaultTone != Tones::Neutral && defaultTone != Tones::LENGTH) {
        for (const auto &emoji : toneSubEmojis) {
            // Where emojis have two tones we want the one where they are the same.
            if (Tones::tonesForUnicode(emoji.unicode()).length() == 1) {
                return emoji.unicode();
            }
        }
    }
    return m_unicode;
}

QString Emoji::unqualifiedUnicode() const
{
    return m_unqualifiedUnicode;
}

QString Emoji::baseUnicode() const
{
    auto baseUnicode = m_unicode;
    for (const auto &tone : Tones::allToneCodePoints()) {
        baseUnicode.remove(tone);
    }
    baseUnicode.squeeze();
    if (_specialBaseCases.contains(baseUnicode)) {
        baseUnicode = _specialBaseCases[baseUnicode];
    }
    return baseUnicode;
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

QList<Emoji> Emoji::subEmojis(Tones::Tone toneFilter) const
{
    if (toneFilter == Tones::Neutral || toneFilter == Tones::LENGTH) {
        return m_subEmojis;
    }
    QList<Emoji> filteredList;
    for (const auto &emoji : m_subEmojis) {
        if (Tones::hasTone(emoji.unicode(), toneFilter)) {
            filteredList += emoji;
        }
    }
    return filteredList;
}

void Emoji::addSubEmoji(const Emoji &emoji)
{
    m_subEmojis += emoji;
}

bool Emoji::isSubEmoji() const
{
    const auto foundTones = Tones::tonesForUnicode(m_unicode);
    return foundTones.length() > 0 && !foundTones.contains(Tones::Neutral);
}

Category Emoji::category() const
{
    if (!categoryDict.contains(m_category)) {
        return emptyCategory;
    }
    return categoryDict.value(m_category);
}

bool Emoji::operator==(const Emoji &right) const
{
    return m_unicode == right.unicode();
}

bool Emoji::operator==(const QString &right) const
{
    return m_unicode == right;
}

bool FavoriteEmoji::operator==(const FavoriteEmoji &right) const
{
    return emoji == right.emoji;
}

bool FavoriteEmoji::operator==(const Emoji &right) const
{
    return emoji == right;
}

bool FavoriteEmoji::operator==(const QString &right) const
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
    if (emoji.baseUnicode() != emoji.unicode()) {
        debug.nospace() << ", Base Unicode: " << emoji.baseUnicode();
    }
    debug.nospace() << ", Name: " << emoji.name() << ')';
    if (emoji.subEmojis().length() > 0) {
        for (const auto &subEmoji : emoji.subEmojis()) {
            debug.nospace() << ", Sub-Emoji: " << subEmoji;
        }
    }

    return debug;
}

#include "moc_emoji.cpp"
