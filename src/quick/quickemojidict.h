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

#include "emojidict.h"

class KEMOJI_EXPORT QuickEmojiDict : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(EmojiDict)
    QML_SINGLETON

public:
    QuickEmojiDict(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

    Q_INVOKABLE void emojiUsed(const KEmoji::Emoji &emoji)
    {
        EmojiDict::instance().emojiUsed(emoji);
    }
};
