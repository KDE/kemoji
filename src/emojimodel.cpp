/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "emojimodel.h"

#include "emoji.h"
#include "emojidict.h"
#include "tones.h"

using namespace KEmoji;

EmojiModel::EmojiModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QList<KEmoji::Emoji> EmojiModel::emojis() const
{
    return m_emojis;
}

void EmojiModel::setEmojis(const QList<KEmoji::Emoji> &emojis)
{
    if (emojis == m_emojis) {
        return;
    }
    m_emojis = emojis;
    beginResetModel();
    endResetModel();
    Q_EMIT emojisChanged();
}

QVariant EmojiModel::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index,
                    QAbstractItemModel::CheckIndexOption::IndexIsValid | QAbstractItemModel::CheckIndexOption::ParentIsInvalid
                        | QAbstractItemModel::CheckIndexOption::DoNotUseParent)
        || index.column() != 0) {
        return {};
    }

    const auto &emoji = m_emojis[index.row()];
    switch (role) {
    case EmojiRole:
        return QVariant::fromValue(emoji);
    case RecentIndexRole:
        return EmojiDict::instance().recentEmojiIndex(emoji);
    case TimesUsedRole:
        return EmojiDict::instance().timesEmojiUsed(emoji);
    case SubEmojisRole: {
        auto familyGroup = EmojiDict::instance().familyGroupForEmoji(emoji);
        if (familyGroup.isEmpty()) {
            familyGroup = EmojiDict::instance().familyGroupForEmoji(Tones::removeTonesFromEmoji(emoji));
        }
        if (familyGroup.isEmpty()) {
            return QVariant::fromValue(QList<Emoji>());
        }
        return QVariant::fromValue(familyGroup.filtered([emoji](const Emoji &familyEmoji) {
            return emoji != familyEmoji;
        }));
    }
    default:
        return {};
    }
}

int EmojiModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_emojis.length();
}

QHash<int, QByteArray> EmojiModel::roleNames() const
{
    return {
        {EmojiRole, "emoji"},
        {SubEmojisRole, "subEmojis"},
    };
}

#include "moc_emojimodel.cpp"
