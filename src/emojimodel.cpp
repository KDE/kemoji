/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "emojimodel.h"

#include "emoji.h"
#include "emojidict.h"

EmojiModel::EmojiModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QVariant EmojiModel::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index,
                    QAbstractItemModel::CheckIndexOption::IndexIsValid | QAbstractItemModel::CheckIndexOption::ParentIsInvalid
                        | QAbstractItemModel::CheckIndexOption::DoNotUseParent)
        || index.column() != 0) {
        return {};
    }

    const auto &emoji = EmojiDict::instance().emojis()[index.row()];
    switch (role) {
    case UnicodeRole:
        return emoji.unicode;
    case DescriptionRole:
        return emoji.description;
    case CategoryRole:
        return QVariant::fromValue(emoji.category());
    case AnnotationsRole:
        return emoji.annotations;
    case FallbackDescriptionRole:
        return emoji.fallbackDescription;
    case RecentIndexRole:
        return EmojiDict::instance().recentEmojiIndex(emoji);
    case TimesUsedRole:
        return EmojiDict::instance().timesEmojiUsed(emoji);
    default:
        return {};
    }
}

int EmojiModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : EmojiDict::instance().emojis().size();
}

QHash<int, QByteArray> EmojiModel::roleNames() const
{
    return {
        {UnicodeRole, "unicode"},
        {DescriptionRole, "description"},
        {CategoryRole, "category"},
        {AnnotationsRole, "annotations"},
        {FallbackDescriptionRole, "fallbackDescription"},
    };
}

#include "moc_emojimodel.cpp"
