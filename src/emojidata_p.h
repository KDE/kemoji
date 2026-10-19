/*
 *    SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QSharedData>
#include <QString>
#include <QUrl>

#include "kemoji/category.h"

namespace KEmoji
{
class EmojiPrivate : public QSharedData
{
public:
    EmojiPrivate()
    {
    }

    EmojiPrivate(const EmojiPrivate &other)
        : QSharedData(other)
        , unicode(other.unicode)
        , source(other.source)
        , unqualifiedUnicode(other.unqualifiedUnicode)
        , name(other.name)
        , fallbackName(other.fallbackName)
        , altNames(other.altNames)
        , category(other.category)
    {
    }

    QString unicode;
    QUrl source;
    QString unqualifiedUnicode;
    QString name;
    QString fallbackName;
    QStringList altNames;
    Categories::Category category;
};

class EmojiData : public QObject
{
    Q_OBJECT

public:
    static EmojiData &instance();

    /*!
     * \brief Whether the unicode emoji data is being loaded.
     */
    bool loading() const;

    /*!
     * \brief Return the \c EmojiPrivate data for the given unicode emoji or custom name.
     *
     * If the unicode or name is valid a \c QExplicitlySharedDataPointer with the
     * \c EmojiPrivate is returned so that all instances link to the same object.
     *
     * If the unicode or name is not vlaid \c std::nullopt is returned.
     */
    std::optional<QExplicitlySharedDataPointer<EmojiPrivate>> dataForEmoji(const QString &unicodeOrCustomName);

    /*!
     * \brief Whether data for the given emoji exists.
     */
    bool hasDataForEmoji(const QString &unicodeOrCustomName);

    /*!
     * \brief Tell the settings the emoji represented by the given string has been used.
     *
     * The string should be the unicode representation of the emoji for a standard
     * unicode emoji. For a custom emoji the name string the emoji is registered
     * under should be passed.
     */
    void emojiUsed(const QString &emoji);

    /*!
     * \brief Clear the emoji usage history.
     *
     * This will leave recent and favorite emojis empty.
     */
    void clearHistory();

    /*!
     * \brief Whether the emoji represented by the given string is a recent emoji.
     *
     * The string should be the unicode representation of the emoji for a standard
     * unicode emoji. For a custom emoji the name string the emoji is registered
     * under should be passed.
     */
    bool isRecent(const QString &emoji) const;

    /*!
     * \brief The index for the emoji represented by the given string in the list of recent emojis.
     *
     * Where 0 is the most recent and increasing indices are older. Returns -1 if
     * the emoji is not in the recent emoji list.
     *
     * The string should be the unicode representation of the emoji for a standard
     * unicode emoji. For a custom emoji the name string the emoji is registered
     * under should be passed.
     */
    int recentIndex(const QString &emoji) const;

    /*!
     * \brief The number of times the emoji represented by the given string has been used.
     *
     * The string should be the unicode representation of the emoji for a standard
     * unicode emoji. For a custom emoji the name string the emoji is registered
     * under should be passed.
     */
    int timesUsed(const QString &emoji) const;

    /*!
     * \brief Register a custom emoji.
     *
     * The source \c QUrl and name must be registered before they will be recognized.
     *
     * \param source The \c QUrl of the image containing the custom emoji. This must
     * be a local file.
     * \param name The name of the custom emoji as a \c QString.
     *
     * \warning Custom Emojis must not share a name with any existing emoji. If they do
     * there will be a clash.
     */
    bool registerCustomEmoji(const QUrl &source, const QString &name);

    /*!
     * \brief Unregister a custom emoji.
     *
     * If the given name is a registered custom emoji it is removed from the list and
     * can no longer be used.
     *
     * \warning All existing custom emojis with the unregistered name will still
     * be valid as they are populated the protections only exist on \c KEmoji::Emoji
     * creation. The user must manage existing custom emoji instances.
     */
    bool unregisterCustomEmoji(const QString &name);

Q_SIGNALS:
    /*!
     * \brief Emitted whenever loading changes.
     *
     * \sa ::loading()
     */
    void loadingChanged();

    /*!
     * \brief Emitted whenever emoji data is added.
     */
    void emojisAdded(QStringList addedEmojis);

    /*!
     * \brief Emitted whenever emoji data is removed.
     */
    void emojisRemoved(QStringList removedEmojis);

    /*!
     * \brief Emitted whenever the emoji is history is changed.
     */
    void emojiHistoryChanged();

private:
    explicit EmojiData(QObject *parent = nullptr);
    bool m_loading = false;

    QHash<QString, QExplicitlySharedDataPointer<EmojiPrivate>> m_data;
    QStringList m_recent;
    QHash<QString, int> m_timesUsed;

    void load();
    void loadDict(const QString &path);
    void loadCustom();

    bool validCustomSource(const QUrl &source);
};
}
