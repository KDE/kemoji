/*
    SPDX-FileCopyrightText: 2022 Weng Xuetian <wegnxt@gmail.com>
    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>
#include <QQmlEngine>

#include "kemoji_export.h"

#include "emoji.h"

namespace KEmoji
{
struct Category;
}

class KEMOJI_EXPORT EmojiDict : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    static EmojiDict &instance();
    static EmojiDict *create(QQmlEngine *engine, QJSEngine *)
    {
        engine->setObjectOwnership(&instance(), QQmlEngine::CppOwnership);
        return &instance();
    }

    const QList<Emoji> &emojis() const;

    const QList<Category> &categories() const;

    const QStringList &recentEmojis() const;

    void addRecentEmoji(const Emoji &emoji);

    Q_INVOKABLE void addRecentEmoji(const QString &emoji);

Q_SIGNALS:
    void recentEmojisChanged();

private:
    explicit EmojiDict(QObject *parent = nullptr);

    QList<Emoji> m_emojis;
    QList<Category> m_categories;
    QStringList m_recentEmojis;

    void load();
    void loadDict(const QString &path);

    void initializeRecentEmojis();
};
