/*
 *    SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QSharedDataPointer>
#include <QtQmlIntegration/qqmlintegration.h>

#include "emoji.h"
#include "kemoji_export.h"

namespace KEmoji
{
/*!
 * \class KEmoji::Group
 * \inmodule KEmoji
 *
 * \brief A class to hold a group of emojis.
 *
 * Emojis can be grouped for any reason but a common examples include:
 *  - supporting the relationship between an emoji and its tones.
 *  - grouping emojis by category.
 */
class KEMOJI_EXPORT Group
{
    Q_GADGET
    QML_VALUE_TYPE(group)

    /*!
     * \property KEmoji::Group::size
     * \brief Return the number of \c KEmoji::Emoji in the group.
     *
     * \sa KEmoji::Emoji
     */
    Q_PROPERTY(qsizetype size READ size)

public:
    Group();
    Group(const Group &other);
    ~Group();

    Group &operator=(const Group &);

    /*!
     * \brief Add the given \c KEmoji::Emoji to the group.
     *
     * \sa KEmoji::Emoji
     */
    void add(const Emoji &emoji);

    /*!
     * \brief Add the given \c KEmoji::Emoji from the group.
     *
     * \sa KEmoji::Emoji
     */
    void remove(const Emoji &emoji);

    /*!
     * \brief Add the given unicode or custom emoji from the group.
     *
     * \sa KEmoji::Emoji
     */
    void remove(const QString &unicodeOrCustomName);

    /*!
     * \brief Return a reference to the \c KEmoji::Emoji at position i in the \c KEmoji::Group.
     *
     * i must be in range otherwise a \c std::out_of_range will be thrown.
     *
     * \sa KEmoji::Emoji
     */
    Emoji at(qsizetype i) const;

    /*!
     * \brief Return the index of the given \c KEmoji::Emoji in the group.
     *
     * Will return -1 id the \c KEmoji::Emoji is not in the group
     *
     * \sa KEmoji::Emoji
     */
    qsizetype indexForEmoji(const Emoji &emoji) const;

    /*!
     * \brief Whether the group contains the given \c KEmoji::Emoji.
     *
     * \sa KEmoji::Emoji
     */
    bool contains(const Emoji &emoji) const;

    qsizetype size() const;

    /*!
     * \brief Return a group with the emojis filtered by \c filter.
     *
     * If no valid filter is set a list containing all emojis is returned.
     */
    Group filtered(std::function<bool(const Emoji &)> filter = {}) const;

    /*!
     * \brief Whether the \c KEmoji::EmojiGroup is empty.
     */
    bool isEmpty() const;

    bool operator==(const Group &right) const;

private:
    class GroupPrivate;
    QSharedDataPointer<GroupPrivate> d;
};

static Group emptyGroup;
}
