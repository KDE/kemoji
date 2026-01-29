/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "emojicategorymodel.h"

#include "emojidict.h"

using namespace Qt::Literals::StringLiterals;

EmojiCategoryModel::EmojiCategoryModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QVariant EmojiCategoryModel::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index,
                    QAbstractItemModel::CheckIndexOption::IndexIsValid | QAbstractItemModel::CheckIndexOption::ParentIsInvalid
                        | QAbstractItemModel::CheckIndexOption::DoNotUseParent)
        || index.column() != 0)
        return {};

    const auto &category = EmojiDict::instance().categories()[index.row()];
    switch (role) {
    case NameRole:
        return category.name();
    case IdRole:
        return category.id();
    case IconNameRole:
        return category.iconName();
    case CategoryRole:
        return QVariant::fromValue(category);
    }
    return {};
}

int EmojiCategoryModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : EmojiDict::instance().categories().size();
}

QHash<int, QByteArray> EmojiCategoryModel::roleNames() const
{
    return {
        {NameRole, "name"},
        {IdRole, "id"},
        {IconNameRole, "iconName"},
        {CategoryRole, "category"},
    };
}

#include "moc_emojicategorymodel.cpp"
