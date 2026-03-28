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
/*!
 * \class Category
 *
 * \brief A class to represent an emoji category.
 *
 * Categories are defined by an id of type \c Category::Categories. All other values
 * are derived from the ID.
 *
 * \note While this class can have its values modified it is designed to act as a
 * value type from QML so only supports reading in QML.
 */
class KEMOJI_EXPORT Category
{
    Q_GADGET
    QML_VALUE_TYPE(category)

    /*!
     * \brief The ID of the \c Category as a \c Category::Categories.
     *
     * \sa Category::Categories
     */
    Q_PROPERTY(Category::Categories id READ id)

    /*!
     * \brief The name of the \c Category as a \c QString.
     */
    Q_PROPERTY(QString name READ name)

    /*!
     * \brief The icon name of the \c Category as a \c QString.
     */
    Q_PROPERTY(QString iconName READ iconName)

public:
    enum Categories {
        None,
        All,
        Recent,
        Favorite,
        Custom,
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

    /*!
     * \brief Set the ID of the \c Category to the given \c Category::Categories.
     */
    void setId(Categories id);

    QString name() const;
    QString iconName() const;

    bool operator==(const Category &right) const;
    bool operator==(Categories right) const;

private:
    Categories m_id = None;
};
}
