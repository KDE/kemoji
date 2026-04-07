/*
 *  SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>

#include "kemoji_export.h"

#include "dict.h"

class KEMOJI_EXPORT QuickDict : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Dict)
    QML_SINGLETON

    Q_PROPERTY(KEmoji::Group emojis READ emojis NOTIFY loaded)

    Q_PROPERTY(QList<KEmoji::Categories::Category> categories READ categories NOTIFY loaded)

public:
    QuickDict(QObject *parent = nullptr)
        : QObject(parent)
    {
        connect(&KEmoji::Dict::instance(), &KEmoji::Dict::loadedChanged, this, &QuickDict::loaded);
    }

    const KEmoji::Group &emojis() const
    {
        return KEmoji::Dict::instance().emojis();
    }

    const QList<KEmoji::Categories::Category> &categories() const
    {
        return KEmoji::Dict::instance().categories();
    }

    Q_INVOKABLE void emojiUsed(const KEmoji::Emoji &emoji)
    {
        KEmoji::Dict::instance().emojiUsed(emoji);
    }

    Q_INVOKABLE void clearHistory()
    {
        KEmoji::Dict::instance().clearHistory();
    }

Q_SIGNALS:
    void loaded();
};
