/*
 *  SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "categoryattached.h"

using namespace KEmoji::Categories;

CategoryAttached::CategoryAttached(QObject *parent)
    : QObject(parent)
{
}

Category CategoryAttached::category() const
{
    return m_category;
}

void CategoryAttached::setCategory(Category category)
{
    if (m_category == category) {
        return;
    }
    m_category = category;
    Q_EMIT categoryChanged();
}

QString CategoryAttached::name() const
{
    return nameStringForCategory(m_category);
}

QString CategoryAttached::iconName() const
{
    return iconNameForCategory(m_category);
}

CategoryAttached *CategoryAttached::qmlAttachedProperties(QObject *object)
{
    return new CategoryAttached(object);
}

#include "moc_categoryattached.cpp"
