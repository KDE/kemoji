/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QString>
#include <QStringList>

#include "emojicategory.h"

using namespace KEmoji;

struct Emoji {
    QString unicode;
    QString description;
    QString fallbackDescription;
    QStringList annotations;

    const Category &category() const
    {
        if (!categoryDict.contains(m_category)) {
            return emptyCategory;
        }
        return categoryDict.at(m_category);
    }
    void setCategory(const QString &category)
    {
        m_category = category;
    }

    bool operator==(const Emoji &right) const
    {
        return unicode == right.unicode;
    }
    bool operator==(const QString &right) const
    {
        return unicode == right;
    }

private:
    QString m_category;
};
