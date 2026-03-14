/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QAbstractListModel>
#include <QtQmlIntegration/qqmlintegration.h>

#include "emoji.h"
#include "kemoji_export.h"

class KEMOJI_EXPORT EmojiModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QList<KEmoji::Emoji> emojis READ emojis WRITE setEmojis NOTIFY emojisChanged)

public:
    enum RoleNames {
        EmojiRole = Qt::UserRole + 1,
        RecentIndexRole,
        TimesUsedRole,
        SubEmojisRole,
    };

    explicit EmojiModel(QObject *parent = nullptr);

    QList<KEmoji::Emoji> emojis() const;
    void setEmojis(const QList<KEmoji::Emoji> &emojis);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void emojisChanged();

private:
    QList<KEmoji::Emoji> m_emojis;
};
