/*
 *    SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "tones.h"

QString KEmoji::Tones::nameStringForTone(Tone tone)
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
const QStringList &KEmoji::Tones::allToneCodePoints()
{
    static const QStringList _allTones = {u"🏻"_s, u"🏼"_s, u"🏽"_s, u"🏾"_s, u"🏿"_s};
    return _allTones;
}

KEmoji::Tones::Tone KEmoji::Tones::toneForCodePoint(const QString &unicode)
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

QString KEmoji::Tones::codePointForTone(Tone tone)
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

QString KEmoji::Tones::exampleEmojiUnicodeForTone(Tone tone)
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

QString KEmoji::Tones::exampleEmojiUnicodeNameStringForTone(Tone tone)
{
    if (tone == LENGTH) {
        return {};
    }
    return u"%1 %2"_s.arg(exampleEmojiUnicodeForTone(tone), nameStringForTone(tone));
}

bool KEmoji::Tones::hasTone(const Emoji &emoji, Tones::Tone tone)
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

QStringList KEmoji::Tones::toneCodePointsForEmoji(const Emoji &emoji)
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

QList<KEmoji::Tones::Tone> KEmoji::Tones::tonesForEmoji(const Emoji &emoji)
{
    QList<Tones::Tone> foundTones;
    if (emoji.isValid() && !emoji.isCustom()) {
        const auto unicode = emoji.unicode();
        for (const auto &tone : Tones::allToneCodePoints()) {
            if (unicode.contains(tone)) {
                foundTones += Tones::toneForCodePoint(tone);
            }
        }
    }
    if (foundTones.isEmpty()) {
        foundTones += Tones::Neutral;
    }
    return foundTones;
}

KEmoji::Emoji KEmoji::Tones::removeTonesFromEmoji(Emoji emoji)
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
