/*
 *  SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "toneattached.h"

using namespace KEmoji::Tones;

ToneAttached::ToneAttached(QObject *parent)
    : QObject(parent)
{
}

Tone ToneAttached::tone() const
{
    return m_tone;
}

void ToneAttached::setTone(Tone tone)
{
    if (m_tone == tone) {
        return;
    }
    m_tone = tone;
    Q_EMIT toneChanged();
}

QString ToneAttached::unicode() const
{
    return codePointForTone(m_tone);
}

QString ToneAttached::exampleUnicode() const
{
    return exampleEmojiUnicodeForTone(m_tone);
}

QString ToneAttached::name() const
{
    return nameStringForTone(m_tone);
}

QString ToneAttached::exampleUnicodeWithName() const
{
    return exampleEmojiUnicodeNameStringForTone(m_tone);
}

ToneAttached *ToneAttached::qmlAttachedProperties(QObject *object)
{
    return new ToneAttached(object);
}

#include "moc_toneattached.cpp"
