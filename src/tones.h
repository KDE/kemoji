/*
 *    SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QHash>
#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>
#include <qcontainerfwd.h>

#include "emoji.h"
#include "kemoji_export.h"

using namespace Qt::Literals::StringLiterals;

namespace KEmoji
{
class Emoji;
};

namespace KEmoji::Tones
{
Q_NAMESPACE_EXPORT(KEMOJI_EXPORT)

/*!
 * \enum Tone
 *
 * Enumeration representing the available emoji tones:
 *
 * \value Neutral
 *        Default representention, i.e. no tone.
 * \value Light
 *        A light skin tone, 🏻.
 * \value MediumLight
 *        A medium-light skin tone, 🏼.
 * \value Medium
 *        A medium skin tone, 🏽.
 * \value MediumDark
 *        A medium-dark skin tone, 🏾.
 * \value Dark
 *        A dark skin tone, 🏿.
 * \omitvalue LENGTH
 */
enum Tone {
    Neutral,
    Light,
    MediumLight,
    Medium,
    MediumDark,
    Dark,
    LENGTH,
};
Q_ENUM_NS(Tone);

/*!
 * \brief Return a string with the name of the given tone.
 */
inline QString nameStringForTone(Tone tone)
{
    switch (tone) {
    case Neutral:
        return i18nc("Emoji with a neutral skin tone", "Neutral");
    case Light:
        return i18nc("Emoji with a light skin tone", "Light");
    case MediumLight:
        return i18nc("Emoji with a medium-light skin tone", "Medium-Light");
    case Medium:
        return i18nc("Emoji with a medium skin tone", "Medium");
    case MediumDark:
        return i18nc("Emoji with a medium-dark skin tone", "Medium-Dark");
    case Dark:
        return i18nc("Emoji with a dark skin tone", "Dark");
    default:
        return {};
    }
}

/*!
 * \brief Returns all tone code points.
 */
inline QStringList allToneCodePoints()
{
    return {u"🏻"_s, u"🏼"_s, u"🏽"_s, u"🏾"_s, u"🏿"_s};
}

/*!
 * \brief Return the \c Tones::Tone of the given code point.
 *
 * Returns \c Tones::Tone::Neutral for any string that isn't a tone code point.
 */
inline Tone toneForCodePoint(const QString &unicode)
{
    static const QHash<QString, Tones::Tone> _codePointToToneMap = {
        {u"🏻"_s, Tones::Light},
        {u"🏼"_s, Tones::MediumLight},
        {u"🏽"_s, Tones::Medium},
        {u"🏾"_s, Tones::MediumDark},
        {u"🏿"_s, Tones::Dark},
    };
    if (!_codePointToToneMap.contains(unicode)) {
        return Neutral;
    }
    return _codePointToToneMap[unicode];
}

/*!
 * \brief Return the code point of the given \c Tones::Tone.
 *
 * \c Tones::Tone::Neutral returns an empty string.
 */
inline QString codePointForTone(Tone tone)
{
    switch (tone) {
    case Neutral:
    case LENGTH:
        return {};
    case Light:
        return u"🏻"_s;
    case MediumLight:
        return u"🏼"_s;
    case Medium:
        return u"🏽"_s;
    case MediumDark:
        return u"🏾"_s;
    case Dark:
        return u"🏿"_s;
    default:
        return {};
    }
}

/*!
 * \brief Return am example emoji using the given tone.
 *
 * This is useful because Neutral doesn't have it's own tone code point (it's
 * a lack of code point).
 *
 * Currently uses the "hand with fingers splayed emoji", 🖐️.
 */
inline QString exampleEmojiUnicodeForTone(Tone tone)
{
    switch (tone) {
    case Neutral:
        return u"🖐️"_s;
    case Light:
        return u"🖐🏻"_s;
    case MediumLight:
        return u"🖐🏼"_s;
    case Medium:
        return u"🖐🏽"_s;
    case MediumDark:
        return u"🖐🏾"_s;
    case Dark:
        return u"🖐🏿"_s;
    default:
        return {};
    }
}

/*!
 * \brief Return a string with an emoji of the given tone and its name.
 */
inline QString exampleEmojiUnicodeNameStringForTone(Tone tone)
{
    if (tone == LENGTH) {
        return {};
    }
    return u"%1 %2"_s.arg(exampleEmojiUnicodeForTone(tone), nameStringForTone(tone));
}

/*!
 * \brief Whether the given \c KEmoji::Emoji string has the given Tones::Tone.
 *
 * The \c KEmoji::Emoji is \c Tones::Tone::Neutral if it has no other Tones::Tone.
 */
inline bool hasTone(const Emoji &emoji, Tones::Tone tone)
{
    if (!emoji.isValid() || emoji.isCustom() || tone == LENGTH) {
        return false;
    }
    const auto unicode = emoji.unicode();
    if (tone == Neutral) {
        for (const auto &codePoint : allToneCodePoints()) {
            if (unicode.contains(codePoint)) {
                return false;
            }
        }
        return true;
    }
    return unicode.contains(codePointForTone(tone));
}

/*!
 * \brief Return a list of code points for the tones in the given \c KEmoji::Emoji.
 */
inline QStringList toneCodePointsForEmoji(const Emoji &emoji)
{
    if (!emoji.isValid() || emoji.isCustom()) {
        return {};
    }
    const auto unicode = emoji.unicode();
    QList<QString> foundTones;
    for (const auto &tone : Tones::allToneCodePoints()) {
        if (unicode.contains(tone)) {
            foundTones += tone;
        }
    }
    return foundTones;
}

/*!
 * \brief Return a list of \c Tones::Tone for the given \c KEmoji::Emoji.
 *
 * If there are none a list containing only \c Tones::Tone::Neutral will be returned.
 */
inline QList<Tones::Tone> tonesForEmoji(const Emoji &emoji)
{
    const auto foundUnicodeTones = toneCodePointsForEmoji(emoji.unicode());
    QList<Tones::Tone> foundTones;
    for (const auto &tone : foundUnicodeTones) {
        foundTones += Tones::toneForCodePoint(tone);
    }
    if (foundTones.isEmpty()) {
        foundTones += Tones::Neutral;
    }
    return foundTones;
}

// TODO: currently doesn't work perfectly for unqualified emojis. They will look right
// but don't have the full code points.

/*!
 * \brief Return an emoji with any tone code points removed from the given emoji.
 *
 * If the emoji has no tone code points the same emoji is returned.
 */
inline Emoji removeTonesFromEmoji(Emoji emoji)
{
    if (!emoji.isValid() || emoji.isCustom()) {
        return emoji;
    }
    auto unicode = emoji.unicode();
    std::ranges::for_each(allToneCodePoints(), [&unicode](const QString &toneCodePoint) {
        unicode.remove(toneCodePoint);
    });
    unicode.squeeze();

    // The cases where removing the tone code points doesn't result in the correct neutral emoji.
    static const QHash<QString, QString> _specialBaseCases = {
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

    if (_specialBaseCases.contains(unicode)) {
        unicode = _specialBaseCases[unicode];
    }
    emoji.setUnicode(unicode);
    return emoji;
}
};
