/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QString>
#include <QStringList>

#include "emojicategory.h"
#include "kemoji_export.h"

namespace KEmoji
{
struct KEMOJI_EXPORT Emoji {
    Q_GADGET
    QML_ELEMENT

    Q_PROPERTY(QString unicode MEMBER unicode CONSTANT)
    Q_PROPERTY(QString name MEMBER name CONSTANT)

public:
    QString unicode;
    QString unqualifiedUnicode;
    QString baseUnicode() const;

    QString name;
    QString fallbackName;
    QStringList altNames;

    QList<Emoji> subEmojis;
    bool isSubEmoji() const;

    const Category &category() const;
    void setCategory(const QString &category);

    bool operator==(const Emoji &right) const;
    bool operator==(const QString &right) const;

private:
    QString m_category;
};

static const Emoji emptyEmoji = {};
}
