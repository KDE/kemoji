/*
 *  SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>

#include "emoji.h"
#include "kemoji_export.h"

#include "dict.h"

class KEMOJI_EXPORT QuickDict : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Dict)
    QML_SINGLETON

    Q_PROPERTY(QList<KEmoji::Emoji> emojis READ emojis CONSTANT)

public:
    QuickDict(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

    const QList<KEmoji::Emoji> &emojis() const
    {
        return KEmoji::Dict::instance().emojis();
    }

    Q_INVOKABLE void emojiUsed(const KEmoji::Emoji &emoji)
    {
        KEmoji::Dict::instance().emojiUsed(emoji);
    }
};
