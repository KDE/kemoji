/*
 *    SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "group.h"
#include "emoji.h"

using namespace KEmoji;

class Group::GroupPrivate : public QSharedData
{
public:
    using GroupIt = std::vector<Emoji>::const_iterator;

    GroupPrivate()
    {
    }

    GroupPrivate(const GroupPrivate &other)
        : QSharedData(other)
        , emojis(other.emojis)
    {
    }

    void add(const Emoji &emoji);
    void remove(const Emoji &emoji);
    void remove(const QString &unicodeOrCustomName);

    std::vector<Emoji> emojis;
    std::unordered_map<QString, GroupIt> emojiIts;

    void reindex();
};

void Group::GroupPrivate::add(const Emoji &emoji)
{
    bool atCpacity = emojis.capacity() - emojiIts.size() <= 1;
    auto insertIt = emojis.insert(emojis.end(), emoji);
    if (atCpacity) {
        reindex();
    } else {
        emojiIts[emoji.id()] = insertIt;
    }
}

void Group::GroupPrivate::remove(const Emoji &emoji)
{
    if (!emojiIts.contains(emoji.id())) {
        return;
    }
    emojis.erase(emojiIts[emoji.id()]);
    reindex();
}

void Group::GroupPrivate::remove(const QString &unicodeOrCustomName)
{
    if (!emojiIts.contains(unicodeOrCustomName)) {
        return;
    }
    emojis.erase(emojiIts[unicodeOrCustomName]);
    reindex();
}

void Group::GroupPrivate::reindex()
{
    emojiIts.clear();
    auto it = emojis.begin();
    while (it != emojis.end()) {
        emojiIts[(*it).id()] = it;
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

void Group::add(const Emoji &emoji)
{
    d->add(emoji);
}

void Group::remove(const Emoji &emoji)
{
    d->remove(emoji);
}

void Group::remove(const QString &unicodeOrCustomName)
{
    d->remove(unicodeOrCustomName);
}

Emoji Group::at(qsizetype i) const
{
    return d->emojis.at(i);
}

qsizetype Group::indexForEmoji(const Emoji &emoji) const
{
    if (!d->emojiIts.contains(emoji.id())) {
        return -1;
    }
    return std::distance(d->emojis.begin(), d->emojiIts.at(emoji.id()));
}

bool Group::contains(const Emoji &emoji) const
{
    return d->emojiIts.contains(emoji.id());
}

qsizetype Group::size() const
{
    return d->emojis.size();
}

Group Group::filtered(std::function<bool(const Emoji &)> filter) const
{
    Group filteredGroup;
    if (!filter) {
        filter = [](const Emoji &) {
            return true;
        };
    }
    std::ranges::for_each(d->emojis, [&filteredGroup, filter](const Emoji &emoji) {
        if (filter(emoji)) {
            filteredGroup.add(emoji);
        }
    });
    return filteredGroup;
}

bool Group::isEmpty() const
{
    return d->emojis.empty();
}

bool Group::operator==(const Group &right) const
{
    return d->emojis == right.d->emojis;
}

#include "moc_group.cpp"
