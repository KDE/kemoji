/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "emoji.h"

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

bool Emoji::isSubEmoji() const
{
    for (const auto &subEmojiPart : subEmojiParts) {
        if (unicode.contains(subEmojiPart)) {
            return true;
        }
    }
    return false;
}

QString Emoji::baseUnicode() const
{
    auto baseUnicode = unicode;
    for (const auto &subEmojiPart : subEmojiParts) {
        baseUnicode.remove(subEmojiPart);
    }
    baseUnicode.squeeze();
    if (specialBaseCases.contains(baseUnicode)) {
        baseUnicode = specialBaseCases[baseUnicode];
    }
    return baseUnicode;
}

const Category &Emoji::category() const
{
    if (!categoryDict.contains(m_category)) {
        return emptyCategory;
    }
    return categoryDict.at(m_category);
}

void Emoji::setCategory(const QString &category)
{
    m_category = category;
}

bool Emoji::operator==(const Emoji &right) const
{
    return unicode == right.unicode;
}
bool Emoji::operator==(const QString &right) const
{
    return unicode == right;
}

#include "moc_emoji.cpp"
