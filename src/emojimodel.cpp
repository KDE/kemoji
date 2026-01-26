/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "emojimodel.h"

#include "emoji.h"
#include "emojidict.h"

using namespace KEmoji;

EmojiModel::EmojiModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

Tones::Tone EmojiModel::defaultTone() const
{
    return m_defaultTone;
}

QString EmojiModel::defaultToneUnicode() const
{
    return Tones::exampleEmojiUnicodeForTone(m_defaultTone);
}

void EmojiModel::setDefaultTone(Tones::Tone defaultTone)
{
    if (defaultTone == m_defaultTone) {
        return;
    }
    m_defaultTone = defaultTone;
    Q_EMIT dataChanged(index(0), index(rowCount() - 1), {UnicodeRole, SubEmojisRole});
    Q_EMIT defaultToneChanged();
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
        return emoji.unicode(m_defaultTone);
    case NameRole:
        return emoji.name();
    case CategoryRole:
        return QVariant::fromValue(emoji.category());
    case AltNamesRole:
        return emoji.altNames();
    case FallbackNameRole:
        return emoji.fallbackName();
    case RecentIndexRole:
        return EmojiDict::instance().recentEmojiIndex(emoji);
    case TimesUsedRole:
        return EmojiDict::instance().timesEmojiUsed(emoji);
    case SubEmojisRole: {
        const auto defaultEmoji = emoji.unicode(m_defaultTone);
        auto subEmojis = emoji.subEmojis();
        subEmojis.removeAll(defaultEmoji);
        if (emoji.unicode() != defaultEmoji) {
            subEmojis.prepend(emoji);
        }
        return QVariant::fromValue(subEmojis);
    }
    case EmojiRole:
        return QVariant::fromValue(emoji);
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
        {NameRole, "name"},
        {CategoryRole, "category"},
        {AltNamesRole, "altNames"},
        {FallbackNameRole, "fallbackName"},
        {SubEmojisRole, "subEmojis"},
        {EmojiRole, "emoji"},
    };
}

#include "moc_emojimodel.cpp"
