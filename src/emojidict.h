/*
    SPDX-FileCopyrightText: 2022 Weng Xuetian <wegnxt@gmail.com>
    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

#include "kemoji_export.h"

#include "emoji.h"

namespace KEmoji
{
class Category;
}

class KEMOJI_EXPORT EmojiDict : public QObject
{
    Q_OBJECT

public:
    static EmojiDict &instance();

    const QList<KEmoji::Emoji> &emojis() const;

    const QList<KEmoji::Category> &categories() const;
    const QList<KEmoji::Emoji> emojisForCategory(KEmoji::Category category) const;

    const QList<KEmoji::RecentEmoji> &recentEmojis() const;

    int recentEmojiIndex(const KEmoji::Emoji &emoji) const;

    const QList<KEmoji::FavoriteEmoji> &favoriteEmojis() const;

    int timesEmojiUsed(const KEmoji::Emoji &emoji) const;

    void emojiUsed(const KEmoji::Emoji &emoji);

Q_SIGNALS:
    void recentEmojisChanged();
    void favoriteEmojisChanged();

private:
    explicit EmojiDict(QObject *parent = nullptr);

    QList<KEmoji::Emoji> m_emojis;
    QList<KEmoji::RecentEmoji> m_recentEmojis;
    QList<KEmoji::FavoriteEmoji> m_favouriteEmojis;
    QList<KEmoji::Category> m_categories;

    void load();
    void loadDict(const QString &path);

    void initialize();
};
