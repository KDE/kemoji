/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "model.h"

#include "dict.h"
#include "emoji.h"
#include "group.h"
#include "tones.h"

using namespace KEmoji;

Model::Model(QObject *parent)
    : QAbstractListModel(parent)
{
}

const Group &Model::emojis() const
{
    return m_emojis;
}

void Model::setEmojis(const Group &emojis)
{
    if (emojis == m_emojis) {
        return;
    }
    m_emojis = emojis;
    beginResetModel();
    endResetModel();
    Q_EMIT emojisChanged();
}

QVariant Model::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index,
                    QAbstractItemModel::CheckIndexOption::IndexIsValid | QAbstractItemModel::CheckIndexOption::ParentIsInvalid
                        | QAbstractItemModel::CheckIndexOption::DoNotUseParent)
        || index.column() != 0) {
        return {};
    }

    const auto &emoji = m_emojis.at(index.row());
    switch (role) {
    case EmojiRole:
        return QVariant::fromValue(emoji);
    case RecentIndexRole:
        return Dict::instance().recentEmojiIndex(emoji);
    case TimesUsedRole:
        return Dict::instance().timesEmojiUsed(emoji);
    case VariantEmojisRole: {
        auto familyGroup = Dict::instance().variantGroupForEmoji(emoji);
        if (familyGroup.isEmpty()) {
            familyGroup = Dict::instance().variantGroupForEmoji(Tones::removeTonesFromEmoji(emoji));
        }
        if (familyGroup.isEmpty()) {
            return QVariant::fromValue(emptyGroup);
        }
        return QVariant::fromValue(familyGroup.filtered([emoji](const Emoji &familyEmoji) {
            return emoji != familyEmoji;
        }));
    }
    default:
        return {};
    }
}

int Model::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_emojis.size();
}

QHash<int, QByteArray> Model::roleNames() const
{
    return {
        {EmojiRole, "emoji"},
        {VariantEmojisRole, "variantEmojis"},
        {RecentIndexRole, "recentIndex"},
        {TimesUsedRole, "timesUsed"},
    };
}

#include "moc_model.cpp"
