/*
    SPDX-FileCopyrightText: 2022 Weng Xuetian <wegnxt@gmail.com>
    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

#include "kemoji_export.h"

#include "emoji.h"
#include "group.h"

namespace KEmoji
{
class Category;

/*!
 * \class Dict
 *
 * \brief A class to provide an emoji dictionary.
 *
 * The dictionary manages the list emojis as standardized by unicode (current version
 * is 16.0).
 *
 * \c Dict is available as singleton so that the emoji lists and tracking remain
 * common. To use the \c Dict just obtain the static reference as follows then call
 * the functions as normal:
 *
 * \code
 * auto dict = KEmoji::Dict::instance();
 * auto emojis = dict.emojis();
 * \endcode
 *
 * The dictionary also provides families of related \c KEmoji::Emoji as \c KEmoji::EmojiGroup.
 * Currently the families group an emoji and all its tones.
 *
 * The dictionary also facilitates the tracking of recent and favorite emojis. By
 * calling \c ::emojiUsed() whenever a \c KEmoji::Emoji is used the dict track both
 * the number of times it is used (for favorites) and an ordered list of when they
 * were used for recent emojis.
 *
 * \sa KEmoji::Emoji, KEmoji::EmojiGroup, ::emojiUsed()
 */
class KEMOJI_EXPORT Dict : public QObject
{
    Q_OBJECT

public:
    static Dict &instance();

    /*!
     * \brief Whether the dictionary data has been loaded.
     */
    bool loaded() const;

    /*!
     * \brief Return the full list of \c KEmoji::Emoji.
     *
     * The list is returned as a const reference.
     *
     * This list is completely unfiltered so includes tones.
     *
     * \sa KEmoji::Emoji
     */
    const Group &emojis() const;

    /*!
     * \brief Return the \c KEmoji::EmojiGroup for the given \c KEmoji::Emoji.
     *
     * If the given emoji has no group an empty group will be returned.
     *
     * Currently the groups are only for families made up of a base emoji and its
     * tones. Each group is referenced to the neutral emoji of the group (i.e. the
     * version without any tone).
     *
     * \sa KEmoji::Emoji, KEmoji::EmojiGroup
     */
    const Group &familyGroupForEmoji(const KEmoji::Emoji &emoji) const;

    /*!
     * \brief Return the full list of \c KEmoji::Category in use.
     *
     * The list represents all the categories currently available.
     *
     * \sa KEmoji::Category
     */
    const QList<KEmoji::Category> &categories() const;

    /*!
     * \brief Return a list of \c KEmoji::Emoji for the given \c KEmoji::Category.
     *
     * \sa KEmoji::Emoji, KEmoji::Category
     */
    const QList<KEmoji::Emoji> emojisForCategory(KEmoji::Category category) const;

    /*!
     * \brief Return a list of recent \c KEmoji::Emoji.
     *
     * \sa KEmoji::Emoji
     */
    const QList<KEmoji::Emoji> &recentEmojis() const;

    /*!
     * \brief Return the index of the given \c KEmoji::Emoji in the recent emojis list.
     *
     * Will return -1 if the \c KEmoiji::Emoji is not in the recent emojis list.
     *
     * \sa KEmoji::Emoji
     */
    int recentEmojiIndex(const KEmoji::Emoji &emoji) const;

    /*!
     * \brief Return a list of favorite \c KEmoji::FavoriteEmoji.
     *
     * The \c KEmoji::FavoriteEmoji struct contains both the \c KEmoji::Emoji and
     * the number of times it has been used.
     *
     * \sa KEmoji::FavoriteEmoji, KEmoji::Emoji
     */
    const QList<KEmoji::FavoriteEmoji> &favoriteEmojis() const;

    /*!
     * \brief Return the number of times the given \c KEmoji::Emoji has been used.
     *
     * 0 if never used.
     *
     * \sa KEmoji::Emoji
     */
    int timesEmojiUsed(const KEmoji::Emoji &emoji) const;

    /*!
     * \brief Tell the dictionary the given \c KEmoji::Emoji has been used.
     *
     * Calling this whenever a \c KEmoji::Emoji updates the recent and favorite
     * emojis.
     *
     * The \c KEmoji::Emoji is placed at the start of the recent emoji list.
     *
     * The \c KEmoji::Emoji has the number of times used incremented by 1.
     *
     * \sa KEmoji::Emoji
     */
    void emojiUsed(const KEmoji::Emoji &emoji);

Q_SIGNALS:
    /*!
     * \brief Emitted whenever loaded changes.
     *
     * \sa ::loaded()
     */
    void loadedChanged();

    /*!
     * \brief Emitted whenever the list of recent emojis is changed.
     */
    void recentEmojisChanged();

    /*!
     * \brief Emitted whenever the list of favorite emojis is changed.
     */
    void favoriteEmojisChanged();

private:
    explicit Dict(QObject *parent = nullptr);

    void initialize();
    bool m_loaded = false;

    std::list<KEmoji::Emoji> m_emojis;
    KEmoji::Group m_completeGroup;
    std::unordered_map<QString, KEmoji::Group> m_emojiFamilyGroups;
    QList<KEmoji::Emoji> m_recentEmojis;
    QList<KEmoji::FavoriteEmoji> m_favouriteEmojis;
    QList<KEmoji::Category> m_categories;

    void load();
    void loadDict(const QString &path);
};

}
