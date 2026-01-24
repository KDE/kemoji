/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "emoji.h"

#include "kemoji_logging.h"

using namespace KEmoji;

namespace
{
const QList<QString> subEmojiParts = {u"🏻"_s, u"🏼"_s, u"🏽"_s, u"🏾"_s, u"🏿"_s};
const QHash<QString, QString> specialBaseCases = {
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

QString Emoji::unicode(const QString &defaultTone) const
{
    if (auto emoji = subEmojiForTone(defaultTone)) {
        return emoji->unicode();
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
    for (const auto &subEmojiPart : subEmojiParts) {
        baseUnicode.remove(subEmojiPart);
    }
    baseUnicode.squeeze();
    if (specialBaseCases.contains(baseUnicode)) {
        baseUnicode = specialBaseCases[baseUnicode];
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

QList<Emoji> Emoji::subEmojis(const QString &toneFilter) const
{
    if (!tones().contains(toneFilter)) {
        return m_subEmojis;
    }
    QList<Emoji> filteredList;
    for (const auto &emoji : m_subEmojis) {
        if (emoji.isTone(toneFilter)) {
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
    for (const auto &subEmojiPart : subEmojiParts) {
        if (m_unicode.contains(subEmojiPart)) {
            return true;
        }
    }
    return false;
}

std::optional<Emoji> Emoji::subEmojiForTone(const QString &tone) const
{
    if (tone.isEmpty()) {
        return std::nullopt;
    }
    if (m_subEmojis.isEmpty() || !tones().contains(tone)) {
        qCWarning(KEMOJI) << Q_FUNC_INFO << "No sub emojis or invalid tone (" << tone << ")";
        return std::nullopt;
    }
    for (auto &emoji : m_subEmojis) {
        if (emoji.unicode().contains(tone)) {
            return emoji;
        }
    }
    return std::nullopt;
}

Category Emoji::category() const
{
    if (!categoryDict.contains(m_category)) {
        return emptyCategory;
    }
    return categoryDict.at(m_category);
}

QStringList Emoji::tones()
{
    static QStringList toneList = {u"🏻"_s, u"🏼"_s, u"🏽"_s, u"🏾"_s, u"🏿"_s};
    return toneList;
}

bool Emoji::isTone(const QString &tone) const
{
    if (!tones().contains(tone)) {
        qCWarning(KEMOJI) << Q_FUNC_INFO << "Invalid tone: " << tone;
        return false;
    }
    return m_unicode.contains(tone);
}

bool Emoji::operator==(const Emoji &right) const
{
    return m_unicode == right.unicode();
}

bool Emoji::operator==(const QString &right) const
{
    return m_unicode == right;
}

#include "moc_emoji.cpp"
