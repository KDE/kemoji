/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QPointer>
#include <QSortFilterProxyModel>
#include <QtQmlIntegration/qqmlintegration.h>

#include "kemoji_export.h"

#include "category.h"

class KEMOJI_EXPORT EmojiFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString searchText READ searchText WRITE setSearchText NOTIFY searchTextChanged)

    Q_PROPERTY(KEmoji::Category currentCategory READ currentCategory WRITE setCurrentCategory NOTIFY categoryChanged)

public:
    explicit EmojiFilterModel(QObject *parent = nullptr);

    QString searchText() const;
    void setSearchText(const QString &searchText);

    KEmoji::Category currentCategory() const;
    Q_INVOKABLE void setCurrentCategory(const QString &category);
    void setCurrentCategory(const KEmoji::Category &category);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

Q_SIGNALS:
    void searchTextChanged();
    void categoryChanged();

private:
    QString m_searchText = {};

    KEmoji::Category m_currentCategory;

    bool nameContainsSearch(const QModelIndex &index) const;
    int exactNameMatch(const QModelIndex &source_left, const QModelIndex &source_right) const;
    int exactAltNameMatch(const QModelIndex &source_left, const QModelIndex &source_right) const;
    int isRecentMatch(const QModelIndex &source_left, const QModelIndex &source_right) const;
    int isFavoriteMatch(const QModelIndex &source_left, const QModelIndex &source_right) const;
};
