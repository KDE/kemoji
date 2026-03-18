/*
 *    SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>

#include "kemoji_export.h"

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
 * \brief Returns all tone code points.
 */
QStringList allToneCodePoints();

/*!
 * \brief Return the \c Tones::Tone of the given code point.
 *
 * Returns \c Tones::Tone::Neutral for any string that isn't a tone code point.
 */
Tone toneForCodePoint(const QString &unicode);

/*!
 * \brief Return the code point of the given \c Tones::Tone.
 *
 * \c Tones::Tone::Neutral returns an empty string.
 */
QString codePointForTone(Tone tone);

/*!
 * \brief Return am example emoji using the given tone.
 *
 * This is useful because Neutral doesn't have it's own tone code point (it's
 * a lack of code point).
 *
 * Currently uses the "hand with fingers splayed emoji", 🖐️.
 */
QString exampleEmojiUnicodeForTone(Tone tone);

/*!
 * \brief Return a string with the name of the given tone.
 */
QString nameStringForTone(Tone tone);

/*!
 * \brief Return a string with an emoji of the given tone and its name.
 */
QString exampleEmojiUnicodeNameStringForTone(Tone tone);

/*!
 * \brief Whether the given unicode string has the given Tones::Tone.
 *
 * The unicode is \c Tones::Tone::Neutral if it has no other Tones::Tone.
 */
bool hasTone(const QString &unicode, Tones::Tone tone);

/*!
 * \brief Return a list of code points for the tones in the given unicode.
 */
QList<QString> toneCodePointsForUnicode(const QString &unicode);

/*!
 * \brief Return a list of \c Tones::Tone for the given unicode.
 *
 * If there are none a list containing only \c Tones::Tone::Neutral will be returned.
 */
QList<Tones::Tone> tonesForUnicode(const QString &unicode);

/*!
 * \brief Return an emoji with any tone code points removed from the given emoji.
 *
 * If the emoji has no tone code points the same emoji is returned.
 */
Emoji removeTonesFromEmoji(Emoji emoji);
};
