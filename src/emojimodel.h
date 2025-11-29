/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QAbstractListModel>
#include <QQmlEngine>

#include "kemoji_export.h"

class KEMOJI_EXPORT EmojiModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum RoleNames {
        UnicodeRole = Qt::DisplayRole,
        DescriptionRole = Qt::ToolTipRole,
        CategoryRole = Qt::UserRole + 1,
        AnnotationsRole,
        FallbackDescriptionRole,
        RecentIndexRole,
        TimesUsedRole,
    };

    explicit EmojiModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;
};
