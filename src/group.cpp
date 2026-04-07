/*
 *    SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "group.h"

using namespace KEmoji;

void Group::add(EmojiIt it)
{
    auto insertIt = m_emojiRefs.insert(m_emojiRefs.end(), it);
    m_emojiIts[it->id()] = insertIt;
}

const Emoji &Group::at(qsizetype i) const
{
    return *m_emojiRefs.at(i);
}

qsizetype Group::indexForEmoji(const Emoji &emoji) const
{
    if (!m_emojiIts.contains(emoji.id())) {
        return -1;
    }
    return std::distance(m_emojiRefs.begin(), m_emojiIts.at(emoji.id()));
}

bool Group::contains(const Emoji &emoji) const
{
    return m_emojiIts.contains(emoji.id());
}

qsizetype Group::size() const
{
    return m_emojiRefs.size();
}

Group Group::filtered(std::function<bool(const Emoji &)> filter) const
{
    Group filteredGroup;
    if (!filter) {
        filter = [](const Emoji &) {
            return true;
        };
    }
    std::ranges::for_each(m_emojiRefs, [&filteredGroup, filter](EmojiIt it) {
        if (filter(*it)) {
            filteredGroup.add(it);
        }
    });
    return filteredGroup;
}

bool Group::isEmpty() const
{
    return m_emojiRefs.empty();
}

bool Group::operator==(const Group &right) const
{
    return m_emojiRefs == right.m_emojiRefs;
}

#include "moc_group.cpp"
