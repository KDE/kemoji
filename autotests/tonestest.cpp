// SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#include <QObject>
#include <QTest>

#include "emoji.h"
#include "tones.h"

using namespace Qt::Literals::StringLiterals;
using namespace KEmoji;

class TonesTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void codePoints();
    void names();
    void emojiTones();
};

void TonesTest::codePoints()
{
    QCOMPARE(Tones::allToneCodePoints(), QStringList({u"🏻"_s, u"🏼"_s, u"🏽"_s, u"🏾"_s, u"🏿"_s}));
    QCOMPARE(Tones::toneForCodePoint({}), Tones::Neutral);
    QCOMPARE(Tones::toneForCodePoint(u"🏻"_s), Tones::Light);
    QCOMPARE(Tones::toneForCodePoint(u"🏼"_s), Tones::MediumLight);
    QCOMPARE(Tones::toneForCodePoint(u"🏽"_s), Tones::Medium);
    QCOMPARE(Tones::toneForCodePoint(u"🏾"_s), Tones::MediumDark);
    QCOMPARE(Tones::toneForCodePoint(u"🏿"_s), Tones::Dark);
    QCOMPARE(Tones::toneForCodePoint(u"😃"_s), Tones::Neutral);
    QCOMPARE(Tones::toneForCodePoint(u"❤️"_s), Tones::Neutral);
    QCOMPARE(Tones::codePointForTone(Tones::Neutral), QString());
    QCOMPARE(Tones::codePointForTone(Tones::LENGTH), QString());
    QCOMPARE(Tones::codePointForTone(Tones::Light), u"🏻"_s);
    QCOMPARE(Tones::codePointForTone(Tones::MediumLight), u"🏼"_s);
    QCOMPARE(Tones::codePointForTone(Tones::Medium), u"🏽"_s);
    QCOMPARE(Tones::codePointForTone(Tones::MediumDark), u"🏾"_s);
    QCOMPARE(Tones::codePointForTone(Tones::Dark), u"🏿"_s);
    QCOMPARE(Tones::exampleEmojiUnicodeForTone(Tones::Neutral), u"🖐️"_s);
    QCOMPARE(Tones::exampleEmojiUnicodeForTone(Tones::LENGTH), QString());
    QCOMPARE(Tones::exampleEmojiUnicodeForTone(Tones::Light), u"🖐🏻"_s);
    QCOMPARE(Tones::exampleEmojiUnicodeForTone(Tones::MediumLight), u"🖐🏼"_s);
    QCOMPARE(Tones::exampleEmojiUnicodeForTone(Tones::Medium), u"🖐🏽"_s);
    QCOMPARE(Tones::exampleEmojiUnicodeForTone(Tones::MediumDark), u"🖐🏾"_s);
    QCOMPARE(Tones::exampleEmojiUnicodeForTone(Tones::Dark), u"🖐🏿"_s);
}

void TonesTest::names()
{
    QCOMPARE(Tones::nameStringForTone(Tones::Neutral), u"Neutral"_s);
    QCOMPARE(Tones::nameStringForTone(Tones::LENGTH), QString());
    QCOMPARE(Tones::nameStringForTone(Tones::Light), u"Light"_s);
    QCOMPARE(Tones::nameStringForTone(Tones::MediumLight), u"Medium-Light"_s);
    QCOMPARE(Tones::nameStringForTone(Tones::Medium), u"Medium"_s);
    QCOMPARE(Tones::nameStringForTone(Tones::MediumDark), u"Medium-Dark"_s);
    QCOMPARE(Tones::nameStringForTone(Tones::Dark), u"Dark"_s);
    QCOMPARE(Tones::exampleEmojiUnicodeNameStringForTone(Tones::Neutral), u"🖐️ Neutral"_s);
    QCOMPARE(Tones::exampleEmojiUnicodeNameStringForTone(Tones::LENGTH), QString());
    QCOMPARE(Tones::exampleEmojiUnicodeNameStringForTone(Tones::Light), u"🖐🏻 Light"_s);
    QCOMPARE(Tones::exampleEmojiUnicodeNameStringForTone(Tones::MediumLight), u"🖐🏼 Medium-Light"_s);
    QCOMPARE(Tones::exampleEmojiUnicodeNameStringForTone(Tones::Medium), u"🖐🏽 Medium"_s);
    QCOMPARE(Tones::exampleEmojiUnicodeNameStringForTone(Tones::MediumDark), u"🖐🏾 Medium-Dark"_s);
    QCOMPARE(Tones::exampleEmojiUnicodeNameStringForTone(Tones::Dark), u"🖐🏿 Dark"_s);
}

void TonesTest::emojiTones()
{
    const auto neturalEmoji = Emoji(u"👋"_s);
    QCOMPARE(Tones::hasTone(neturalEmoji, Tones::Neutral), true);
    QCOMPARE(Tones::hasTone(neturalEmoji, Tones::Light), false);
    QCOMPARE(Tones::hasTone(neturalEmoji, Tones::MediumLight), false);
    QCOMPARE(Tones::hasTone(neturalEmoji, Tones::Medium), false);
    QCOMPARE(Tones::hasTone(neturalEmoji, Tones::MediumDark), false);
    QCOMPARE(Tones::hasTone(neturalEmoji, Tones::Dark), false);
    QCOMPARE(Tones::hasTone(neturalEmoji, Tones::LENGTH), false);
    QCOMPARE(Tones::toneCodePointsForEmoji(neturalEmoji), QStringList());
    QCOMPARE(Tones::tonesForEmoji(neturalEmoji), QList<Tones::Tone>({Tones::Neutral}));

    const auto darkEmoji = Emoji(u"👋🏿"_s);
    QCOMPARE(Tones::hasTone(darkEmoji, Tones::Neutral), false);
    QCOMPARE(Tones::hasTone(darkEmoji, Tones::Light), false);
    QCOMPARE(Tones::hasTone(darkEmoji, Tones::MediumLight), false);
    QCOMPARE(Tones::hasTone(darkEmoji, Tones::Medium), false);
    QCOMPARE(Tones::hasTone(darkEmoji, Tones::MediumDark), false);
    QCOMPARE(Tones::hasTone(darkEmoji, Tones::Dark), true);
    QCOMPARE(Tones::hasTone(darkEmoji, Tones::LENGTH), false);
    QCOMPARE(Tones::toneCodePointsForEmoji(darkEmoji), QStringList({u"🏿"_s}));
    QCOMPARE(Tones::tonesForEmoji(darkEmoji), QList<Tones::Tone>({Tones::Dark}));

    const auto multiToneEmoji = Emoji(u"🫱🏾‍🫲🏼"_s);
    QCOMPARE(Tones::hasTone(multiToneEmoji, Tones::Neutral), false);
    QCOMPARE(Tones::hasTone(multiToneEmoji, Tones::Light), false);
    QCOMPARE(Tones::hasTone(multiToneEmoji, Tones::MediumLight), true);
    QCOMPARE(Tones::hasTone(multiToneEmoji, Tones::Medium), false);
    QCOMPARE(Tones::hasTone(multiToneEmoji, Tones::MediumDark), true);
    QCOMPARE(Tones::hasTone(multiToneEmoji, Tones::Dark), false);
    QCOMPARE(Tones::hasTone(multiToneEmoji, Tones::LENGTH), false);
    QCOMPARE(Tones::toneCodePointsForEmoji(multiToneEmoji), QStringList({u"🏼"_s, u"🏾"_s}));
    QCOMPARE(Tones::tonesForEmoji(multiToneEmoji), QList<Tones::Tone>({Tones::MediumLight, Tones::MediumDark}));

    QCOMPARE(Tones::removeTonesFromEmoji(neturalEmoji), neturalEmoji);
    QCOMPARE(Tones::removeTonesFromEmoji(darkEmoji), neturalEmoji);
    QCOMPARE(Tones::removeTonesFromEmoji(multiToneEmoji), Emoji(u"🤝"_s));
}

QTEST_GUILESS_MAIN(TonesTest)
#include "tonestest.moc"
