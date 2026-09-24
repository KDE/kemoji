/*
 *    SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QHash>
#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>

#include <KLocalizedString>

#include "emoji.h"
#include "kemoji_export.h"

using namespace Qt::Literals::StringLiterals;

namespace KEmoji
{
class Emoji;
};

/*!
 * \namespace KEmoji::Tones
 * \inmodule KEmoji
 */
namespace KEmoji::Tones
{
Q_NAMESPACE_EXPORT(KEMOJI_EXPORT)
QML_ELEMENT

/*!
 * \enum KEmoji::Tones::Tone
 *
 * Enumeration representing the available emoji tones:
 *
 * \value Neutral
 *        Default representation, i.e. no tone.
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
KEMOJI_EXPORT QString nameStringForTone(Tone tone);

/*!
 * \brief Returns all tone code points.
 */
KEMOJI_EXPORT const QStringList &allToneCodePoints();

/*!
 * \brief Return the \c Tones::Tone of the given code point.
 *
 * Returns \c Tones::Tone::Neutral for any string that isn't a tone code point.
 */
KEMOJI_EXPORT Tone toneForCodePoint(const QString &unicode);

/*!
 * \brief Return the code point of the given \c Tones::Tone.
 *
 * \c Tones::Tone::Neutral returns an empty string.
 */
KEMOJI_EXPORT QString codePointForTone(Tone tone);

/*!
 * \brief Return am example emoji using the given tone.
 *
 * This is useful because Neutral doesn't have it's own tone code point (it's
 * a lack of code point).
 *
 * Currently uses the "hand with fingers splayed emoji", 🖐️.
 */
KEMOJI_EXPORT QString exampleEmojiUnicodeForTone(Tone tone);

/*!
 * \brief Return a string with an emoji of the given tone and its name.
 */
KEMOJI_EXPORT QString exampleEmojiUnicodeNameStringForTone(Tone tone);

/*!
 * \brief Whether the given \c KEmoji::Emoji string has the given Tones::Tone.
 *
 * The \c KEmoji::Emoji is \c Tones::Tone::Neutral if it has no other Tones::Tone.
 */
KEMOJI_EXPORT bool hasTone(const Emoji &emoji, Tones::Tone tone);

/*!
 * \brief Return a list of code points for the tones in the given \c KEmoji::Emoji.
 */
KEMOJI_EXPORT QStringList toneCodePointsForEmoji(const Emoji &emoji);

/*!
 * \brief Return a list of \c Tones::Tone for the given \c KEmoji::Emoji.
 *
 * If there are none a list containing only \c Tones::Tone::Neutral will be returned.
 */
KEMOJI_EXPORT QList<Tones::Tone> tonesForEmoji(const Emoji &emoji);

// TODO: currently doesn't work perfectly for unqualified emojis. They will look right
// but don't have the full code points.

/*!
 * \brief Return an emoji with any tone code points removed from the given emoji.
 *
 * If the emoji has no tone code points the same emoji is returned.
 */
KEMOJI_EXPORT Emoji removeTonesFromEmoji(Emoji emoji);
};
