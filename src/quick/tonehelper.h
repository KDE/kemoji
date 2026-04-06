/*
 *  SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>

#include "kemoji_export.h"

#include "tones.h"

class KEMOJI_EXPORT ToneHelper : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    /*!
     * \brief The \c KEmoji::Tones::Tone the helper is for.
     *
     * \sa KEmoji::Tones::Tone
     */
    Q_PROPERTY(KEmoji::Tones::Tone tone READ tone WRITE setTone NOTIFY toneChanged)

    /*!
     * \brief The unicode code point of the \c KEmoji::Tones::Tone as a \c QString.
     *
     * \note Since \c KEmoji::Tones::Tone::Neutral has no code point (it's a lcak of tone)
     * an empty string is returned.
     */
    Q_PROPERTY(QString unicode READ unicode NOTIFY toneChanged)

    /*!
     * \brief An example emoji for the \c KEmoji::Tones::Tone as a \c QString.
     *
     * Currently uses the "hand with fingers splayed emoji" as the example, 🖐️.
     */
    Q_PROPERTY(QString exampleUnicode READ exampleUnicode NOTIFY toneChanged)

    /*!
     * \brief The name of the \c KEmoji::Tones::Tone as a \c QString.
     */
    Q_PROPERTY(QString name READ name NOTIFY toneChanged)

    /*!
     * \brief An example emoji with the tone name for the \c KEmoji::Tones::Tone as a \c QString.
     *
     * Currently uses the "hand with fingers splayed emoji" as the example, 🖐️.
     */
    Q_PROPERTY(QString exampleUnicodeWithName READ exampleUnicodeWithName NOTIFY toneChanged)

public:
    ToneHelper(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

    KEmoji::Tones::Tone tone() const
    {
        return m_tone;
    }

    void setTone(KEmoji::Tones::Tone tone)
    {
        if (tone == m_tone) {
            return;
        }
        m_tone = tone;
        Q_EMIT toneChanged();
    }

    QString unicode() const
    {
        return KEmoji::Tones::codePointForTone(m_tone);
    }

    QString exampleUnicode() const
    {
        return KEmoji::Tones::exampleEmojiUnicodeForTone(m_tone);
    }

    QString name() const
    {
        return KEmoji::Tones::nameStringForTone(m_tone);
    }

    QString exampleUnicodeWithName() const
    {
        return KEmoji::Tones::exampleEmojiUnicodeNameStringForTone(m_tone);
    }

Q_SIGNALS:
    void toneChanged();

private:
    KEmoji::Tones::Tone m_tone;
};
