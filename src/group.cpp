/*
 *    SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "group.h"

using namespace KEmoji;

class Group::GroupPrivate : public QSharedData
{
public:
    using GroupIt = std::vector<std::list<KEmoji::Emoji>::iterator>::const_iterator;

    GroupPrivate()
    {
    }

    GroupPrivate(const GroupPrivate &other)
        : QSharedData(other)
        , emojiRefs(other.emojiRefs)
        , emojiIts(other.emojiIts)
    {
    }

    void add(std::list<KEmoji::Emoji>::iterator it);
    void remove(std::list<KEmoji::Emoji>::iterator it);

    std::vector<std::list<KEmoji::Emoji>::iterator> emojiRefs;
    std::unordered_map<QString, GroupIt> emojiIts;

    void reindex();
};

void Group::GroupPrivate::add(EmojiIt it)
{
    bool atCpacity = emojiRefs.capacity() - emojiIts.size() <= 1;
    auto insertIt = emojiRefs.insert(emojiRefs.end(), it);
    if (atCpacity) {
        reindex();
    } else {
        emojiIts[it->id()] = insertIt;
    }
}

void Group::GroupPrivate::remove(EmojiIt it)
{
    if (!emojiIts.contains(it->id())) {
        return;
    }
    emojiRefs.erase(emojiIts[it->id()]);
    reindex();
}

void Group::GroupPrivate::reindex()
{
    emojiIts.clear();
    auto it = emojiRefs.begin();
    while (it != emojiRefs.end()) {
        emojiIts[(*it)->id()] = it;
        ++it;
    }
}

Group::Group()
    : d(new GroupPrivate)
{
}

Group::Group(const Group &other)
    : d(other.d)
{
}

Group::~Group()
{
}

Group &Group::operator=(const Group &other)
{
    d = other.d;
    return *this;
}

std::vector<EmojiIt> &Group::emojiRefs()
{
    return d->emojiRefs;
}

void Group::add(EmojiIt it)
{
    d->add(it);
}

void Group::remove(EmojiIt it)
{
    d->remove(it);
}

const Emoji &Group::at(qsizetype i) const
{
    return *d->emojiRefs.at(i);
}

qsizetype Group::indexForEmoji(const Emoji &emoji) const
{
    if (!d->emojiIts.contains(emoji.id())) {
        return -1;
    }
    return std::distance(d->emojiRefs.begin(), d->emojiIts.at(emoji.id()));
}

bool Group::contains(const Emoji &emoji) const
{
    return d->emojiIts.contains(emoji.id());
}

qsizetype Group::size() const
{
    return d->emojiRefs.size();
}

Group Group::filtered(std::function<bool(const Emoji &)> filter) const
{
    Group filteredGroup;
    if (!filter) {
        filter = [](const Emoji &) {
            return true;
        };
    }
    std::ranges::for_each(d->emojiRefs, [&filteredGroup, filter](EmojiIt it) {
        if (filter(*it)) {
            filteredGroup.add(it);
        }
    });
    return filteredGroup;
}

bool Group::isEmpty() const
{
    return d->emojiRefs.empty();
}

bool Group::operator==(const Group &right) const
{
    return d->emojiRefs == right.d->emojiRefs;
}

#include "moc_group.cpp"
