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

    QList<Emoji> emojis;
};

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
    d->emojis += emoji;
}

void Group::remove(const Emoji &emoji)
{
    d->emojis.removeAll(emoji);
}

void Group::remove(const QString &unicodeOrCustomName)
{
    d->emojis.removeAll(unicodeOrCustomName);
}

Emoji Group::at(qsizetype i) const
{
    return d->emojis.at(i);
}

qsizetype Group::indexForEmoji(const Emoji &emoji) const
{
    return d->emojis.indexOf(emoji);
}

bool Group::contains(const Emoji &emoji) const
{
    return d->emojis.contains(emoji.id());
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
    return d->emojis.isEmpty();
}

bool Group::operator==(const Group &right) const
{
    return d->emojis == right.d->emojis;
}

#include "moc_group.cpp"
