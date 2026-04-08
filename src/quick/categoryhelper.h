/*
 *  SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>

#include "category.h"

class CategoryHelper : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    /*!
     * \brief The \c KEmoji::Categories::Category the helper is for.
     *
     * \sa KEmoji::Categories::Category
     */
    Q_PROPERTY(KEmoji::Categories::Category category READ category WRITE setCategory NOTIFY categoryChanged)

    /*!
     * \brief The name of the \c KEmoji::Categories::Category as a \c QString.
     */
    Q_PROPERTY(QString name READ name NOTIFY categoryChanged)

    /*!
     * \brief The icon name of the \c KEmoji::Categories::Category as a \c QString.
     */
    Q_PROPERTY(QString iconName READ iconName NOTIFY categoryChanged)

public:
    CategoryHelper(QObject *parent)
        : QObject(parent)
    {
    }

    KEmoji::Categories::Category category() const
    {
        return m_category;
    }

    void setCategory(KEmoji::Categories::Category category)
    {
        if (category == m_category) {
            return;
        }
        m_category = category;
        Q_EMIT categoryChanged();
    }

    QString name() const
    {
        return KEmoji::Categories::nameStringForCategory(m_category);
    }

    QString iconName() const
    {
        return KEmoji::Categories::iconNameForCategory(m_category);
    }

Q_SIGNALS:
    void categoryChanged();

private:
    KEmoji::Categories::Category m_category;
};
