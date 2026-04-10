/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QAbstractListModel>
#include <QtQmlIntegration/qqmlintegration.h>

#include "group.h"
#include "kemoji_export.h"

namespace KEmoji
{
/*!
 * \class Model
 *
 * \brief A model for a list of \c KEmoji::Emoji in a \c KEmoji::Group.
 *
 * The model is initialized by passing a \c KEmoji::Group using \c KEmoji::EmojiModel::setEmojis()
 * (or the \c emojis \c Q_PROPERTY in QML).
 *
 * \sa KEmoji::Emoji, KEmoji::Group
 */
class KEMOJI_EXPORT Model : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    /*!
     * \brief The \c KEmoji::Group contain the \c KEmoji::Emoji that the model is managing.
     *
     * \sa KEmoji::Emoji, KEmoji::Group
     */
    Q_PROPERTY(KEmoji::Group emojis READ emojis WRITE setEmojis NOTIFY emojisChanged)

public:
    /*!
     * \enum RoleNames
     *
     * Enumeration representing the available model roles:
     *
     * \value EmojiRole
     *        The \c KEmoji::Emoji at the given index.
     * \value VariantEmojisRole
     *        A \c KEmoji::Group with any variant emojis for the main \c KEmoji::Emoji.
     * \value RecentIndexRole
     *        The index of the \c KEmoji::Emoji in the recent emoji list. -1 if not
     *        a recent emoji.
     * \value TimesUsedRole
     *        The number of times the \c KEmoji::Emoji has been used.
     */
    enum RoleNames {
        EmojiRole = Qt::UserRole + 1,
        VariantEmojisRole,
        RecentIndexRole,
        TimesUsedRole,
    };

    explicit Model(QObject *parent = nullptr);

    const Group &emojis() const;
    void setEmojis(const Group &emojis);

    /*!
     * \brief Get the given role value at the given index.
     *
     * \sa QAbstractItemModel::data
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /*!
     * \brief Number of rows in the model.
     *
     * \sa QAbstractItemModel::rowCount
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /*!
     * \brief Returns a mapping from Role enum values to role names.
     *
     * \sa RoleNames, QAbstractItemModel::roleNames()
     */
    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void emojisChanged();

private:
    Group m_emojis;
};
}
