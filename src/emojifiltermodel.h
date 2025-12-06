/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QQmlEngine>
#include <QSortFilterProxyModel>

#include "kemoji_export.h"

#include "emojicategory.h"
#include "emojicategorymodel.h"

class KEMOJI_EXPORT EmojiFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString searchText READ searchText WRITE setSearchText NOTIFY searchTextChanged)

    Q_PROPERTY(QString currentCategoryId READ currentCategoryId WRITE setCurrentCategoryId NOTIFY categoryChanged)

    Q_PROPERTY(KEmoji::Category currentCategory READ currentCategory NOTIFY categoryChanged)

    Q_PROPERTY(EmojiCategoryModel *categoryModel READ categoryModel CONSTANT)

public:
    explicit EmojiFilterModel(QObject *parent = nullptr);

    QString searchText() const;
    void setSearchText(const QString &searchText);

    QString currentCategoryId() const;
    void setCurrentCategoryId(QString currentCategoryId);
    KEmoji::Category currentCategory() const;

    EmojiCategoryModel *categoryModel();

    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

Q_SIGNALS:
    void searchTextChanged();
    void categoryChanged();

private:
    QString m_searchText = {};

    QString m_currentCategoryId;

    QPointer<EmojiCategoryModel> m_categoryModel;

    bool nameContainsSearch(const QModelIndex &index) const;
    int exactNameMatch(const QModelIndex &source_left, const QModelIndex &source_right) const;
    int exactAltNameMatch(const QModelIndex &source_left, const QModelIndex &source_right) const;
    int isRecentMatch(const QModelIndex &source_left, const QModelIndex &source_right) const;
    int isFavoriteMatch(const QModelIndex &source_left, const QModelIndex &source_right) const;
};
