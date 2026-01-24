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
/**
 * \class Tones
 *
 * \brief This class is designed to define the Tone enumeration.
 *
 * There are also numerous helper funtions for working with tones and displaying
 * them in a UI.
 */
class KEMOJI_EXPORT Tones : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

public:
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
    Q_ENUM(Tone);

    /*!
     * \brief Returns all tone code points.
     */
    static QStringList allToneCodePoints();

    /*!
     * \brief Return the Tones::Tone of the given code point.
     *
     * returns Tones::Tone::Neutral for any string that isn't a tone code point.
     */
    static Tone toneForCodePoint(const QString &unicode);

    /*!
     * \brief Return the code point of the given Tones::Tone.
     *
     * Tones::Tone::Neutral returns an empty string.
     */
    static QString codePointForTone(Tone tone);

    /*!
     * \brief Return am example emoji using the given tone.
     *
     * This is useful because Neutral doesn't have it's own tone code point (it's
     * a lack of code point).
     *
     * Currently uses the "hand with fingers splayed emoji", 🖐️.
     */
    static QString exampleEmojiUnicodeForTone(Tone tone);

    /*!
     * \brief Return a string with the name of the given tone.
     */
    static QString nameStringForTone(Tone tone);

    /*!
     * \brief Return a string with an emoji of the given tone and its name.
     */
    static QString exampleEmojiUnicodeNameStringForTone(Tone tone);

    /*!
     * \brief Whether the given unicode string has the given Tones::Tone.
     *
     * The unicode is Tones::Tone::Neutral if it has no other Tones::Tone.
     */
    static bool hasTone(const QString &unicode, Tones::Tone tone);

    /*!
     * \brief Return a list of code points for the tones in the given unicode.
     */
    static QList<QString> toneCodePointsForUnicode(const QString &unicode);

    /*!
     * \brief Return a list of Tones::Tone for the given unicode.
     *
     * If there are none a list containing only Tones::Tone::Neutral will be returned.
     */
    static QList<Tones::Tone> tonesForUnicode(const QString &unicode);
};
};
