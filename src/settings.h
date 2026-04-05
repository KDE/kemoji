/*
 *    SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QHash>
#include <QObject>
#include <QUrl>

#include "kemoji_export.h"

namespace KEmoji
{
/*!
 * \class Settings
 *
 * \brief A class to manage the \c KEmoji settings.
 *
 * This currently supports recent, favorite and custom emojis.
 */
class KEMOJI_EXPORT Settings : public QObject
{
    Q_OBJECT

public:
    static Settings &instance();

    /*!
     * \brief Tell the settings the emoji represented by the given string has been used.
     *
     * The string should be the unicode representation of the emoji for a standard
     * unicode emoji. For a custom emoji the name string the emoji is registered
     * under should be passed.
     */
    void emojiUsed(const QString &emoji);

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
     * there will be a clash and anytime the name is set of the unicode emoji it will
     * be overridden by the custom version.
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

    /*!
     * \brief Return a list of registered custom emojis.
     */
    const QHash<QString, QUrl> &customEmojis() const;

    /*!
     * \brief Whether the given name is a registered custom emoji.
     */
    bool isCustomEmoji(const QString &name) const;

    /*!
     * \brief The source \c QUrl for the given name.
     *
     * Returns and invalid \c QUrl if the given name is not a custom emoji.
     */
    QUrl customEmojiSource(const QString &name) const;

Q_SIGNALS:
    /*!
     * \brief Emitted whenever the list of recent emojis is changed.
     */
    void recentEmojisChanged();

    /*!
     * \brief Emitted whenever the list of favorite emojis is changed.
     */
    void favoriteEmojisChanged();

private:
    explicit Settings(QObject *parent = nullptr);
    void initialize();

    QStringList m_recent;
    QHash<QString, int> m_timesUsed;

    QHash<QString, QUrl> m_customEmojis;

    bool validSource(const QUrl &source);
    QHash<QString, QUrl> readCustomEmojis();
    void writeCustomEmojis(const QHash<QString, QUrl> &customEmojis);
};
}
