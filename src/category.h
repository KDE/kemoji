/*
    SPDX-FileCopyrightText: 2022 Weng Xuetian <wegnxt@gmail.com>
    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>

#include "kemoji_export.h"

using namespace Qt::Literals::StringLiterals;

namespace KEmoji
{
class KEMOJI_EXPORT Category
{
    Q_GADGET
    QML_VALUE_TYPE(category)

    Q_PROPERTY(Category::Categories id READ id CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString iconName READ iconName CONSTANT)

public:
    enum Categories {
        None,
        All,
        Recent,
        Favorite,
        Smileys,
        People,
        Animals,
        Food,
        Travel,
        Activities,
        Objects,
        Symbols,
        Flags,
    };
    Q_ENUM(Categories);

    explicit Category(Categories id = {});

    Categories id() const;
    QString name() const;
    QString iconName() const;

    bool operator==(const Category &right) const;
    bool operator==(Categories right) const;

private:
    Categories m_id = None;
};
}
