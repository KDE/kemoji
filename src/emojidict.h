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
struct Category;
}

class KEMOJI_EXPORT EmojiDict : public QObject
{
    Q_OBJECT

public:
    static EmojiDict &instance();

    const QList<KEmoji::Emoji> &emojis() const;

    const QList<KEmoji::Category> &categories() const;

    const QStringList &recentEmojis() const;

    int recentEmojiIndex(const KEmoji::Emoji &emoji) const;
    int recentEmojiIndex(const QString &emoji) const;

    const QHash<QString, int> &favoriteEmojis() const;

    int timesEmojiUsed(const KEmoji::Emoji &emoji) const;
    int timesEmojiUsed(const QString &emoji) const;

    void emojiUsed(const KEmoji::Emoji &emoji);

    void emojiUsed(const QString &emoji);

Q_SIGNALS:
    void recentEmojisChanged();
    void favoriteEmojisChanged();

private:
    explicit EmojiDict(QObject *parent = nullptr);

    QList<KEmoji::Emoji> m_emojis;
    QStringList m_recentEmojis;
    QHash<QString, int> m_favouriteEmojis;
    QList<KEmoji::Category> m_categories;

    void load();
    void loadDict(const QString &path);

    void initialize();
};
