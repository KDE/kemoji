/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QAbstractListModel>
#include <QtQmlIntegration/qqmlintegration.h>

#include "kemoji_export.h"

class KEMOJI_EXPORT EmojiCategoryModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum RoleNames {
        NameRole = Qt::DisplayRole,
        IdRole = Qt::UserRole + 1,
        IconNameRole,
        CategoryRole,
    };

    explicit EmojiCategoryModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;
};
