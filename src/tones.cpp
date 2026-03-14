/*
 *    SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "tones.h"

#include <QHash>

#include <KLazyLocalizedString>

#include "emoji.h"
#include "kemoji_logging.h"

using namespace Qt::Literals::StringLiterals;
using namespace KEmoji;

namespace
{
const QStringList _allTonesCodePoints = {u"🏻"_s, u"🏼"_s, u"🏽"_s, u"🏾"_s, u"🏿"_s};

const QHash<QString, Tones::Tone> _codePointToToneMap = {
    {u"🏻"_s, Tones::Light},
    {u"🏼"_s, Tones::MediumLight},
    {u"🏽"_s, Tones::Medium},
    {u"🏾"_s, Tones::MediumDark},
    {u"🏿"_s, Tones::Dark},
};

const QHash<Tones::Tone, QString> _toneToUnicodeMap = {
    {Tones::Light, u"🏻"_s},
    {Tones::MediumLight, u"🏼"_s},
    {Tones::Medium, u"🏽"_s},
    {Tones::MediumDark, u"🏾"_s},
    {Tones::Dark, u"🏿"_s},
};

const QHash<Tones::Tone, QString> _toneToExampleUnicodeMap = {
    {Tones::Neutral, u"🖐️"_s},
    {Tones::Light, u"🖐🏻"_s},
    {Tones::MediumLight, u"🖐🏼"_s},
    {Tones::Medium, u"🖐🏽"_s},
    {Tones::MediumDark, u"🖐🏾"_s},
    {Tones::Dark, u"🖐🏿"_s},
};

const QHash<Tones::Tone, KLazyLocalizedString> _toneToNameMap = {
    {Tones::Neutral, kli18nc("Emoji with a neutral skin tone", "Neutral")},
    {Tones::Light, kli18nc("Emoji with a light skin tone", "Light")},
    {Tones::MediumLight, kli18nc("Emoji with a medium-light skin tone", "Medium-Light")},
    {Tones::Medium, kli18nc("Emoji with a medium skin tone", "Medium")},
    {Tones::MediumDark, kli18nc("Emoji with a medium-dark skin tone", "Medium-Dark")},
    {Tones::Dark, kli18nc("Emoji with a dark skin tone", "Dark")},
};

// The cases where removing the tone code points doesn't result in the correct neutral emoji.
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

QStringList Tones::allToneCodePoints()
{
    return _allTonesCodePoints;
}

Tones::Tone Tones::toneForCodePoint(const QString &unicode)
{
    if (!_codePointToToneMap.contains(unicode)) {
        return Neutral;
    }
    return _codePointToToneMap[unicode];
}

QString Tones::codePointForTone(Tone tone)
{
    if (tone == Neutral || tone == LENGTH) {
        return {};
    }
    return _toneToUnicodeMap[tone];
}

QString Tones::exampleEmojiUnicodeForTone(Tone tone)
{
    return _toneToExampleUnicodeMap.value(tone, {});
}

QString Tones::nameStringForTone(Tone tone)
{
    return _toneToNameMap.value(tone, {}).toString();
}

QString Tones::exampleEmojiUnicodeNameStringForTone(Tone tone)
{
    if (tone == LENGTH) {
        qCWarning(KEMOJI) << Q_FUNC_INFO << "LENGTH should never be used it exists to track the number of values in the enum only";
        return {};
    }
    return u"%1 %2"_s.arg(exampleEmojiUnicodeForTone(tone), nameStringForTone(tone));
}

bool Tones::hasTone(const QString &unicode, Tones::Tone tone)
{
    if (tone == LENGTH) {
        qCWarning(KEMOJI) << Q_FUNC_INFO << "LENGTH should never be used it exists to track the number of values in the enum only";
        return false;
    }
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

QList<QString> Tones::toneCodePointsForUnicode(const QString &unicode)
{
    QList<QString> foundTones;
    for (const auto &tone : Tones::allToneCodePoints()) {
        if (unicode.contains(tone)) {
            foundTones += tone;
        }
    }
    return foundTones;
}

QList<Tones::Tone> Tones::tonesForUnicode(const QString &unicode)
{
    const auto foundUnicodeTones = toneCodePointsForUnicode(unicode);
    QList<Tones::Tone> foundTones;
    for (const auto &tone : foundUnicodeTones) {
        foundTones += Tones::toneForCodePoint(tone);
    }
    if (foundTones.isEmpty()) {
        foundTones += Tones::Neutral;
    }
    return foundTones;
}

Emoji Tones::removeTonesFromEmoji(Emoji emoji)
{
    if (!emoji.isValid() || emoji.isCustom()) {
        return emoji;
    }
    auto unicode = emoji.unicode();
    std::ranges::for_each(_allTonesCodePoints, [&unicode](const QString &toneCodePoint) {
        unicode.remove(toneCodePoint);
    });
    unicode.squeeze();
    if (_specialBaseCases.contains(unicode)) {
        unicode = _specialBaseCases[unicode];
    }
    emoji.setUnicode(unicode);
    return emoji;
}

#include "moc_tones.cpp"
