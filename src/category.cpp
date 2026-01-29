/*
 *  SPDX-FileCopyrightText: 2022 Weng Xuetian <wegnxt@gmail.com>
 *  SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "category.h"

using namespace KEmoji;

Category::Category(const QString &id, const KLazyLocalizedString &name, const QString &iconName)
    : m_id(id)
    , m_name(name)
    , m_iconName(iconName)
{
}

QString Category::id() const
{
    return m_id;
}

QString Category::name() const
{
    return m_name.toString();
}

QString Category::iconName() const
{
    return m_iconName;
}

bool Category::isEmpty() const
{
    return m_id.isEmpty();
}

bool Category::operator==(const Category &right) const
{
    return m_id == right.id();
}
bool Category::operator==(const QString &right) const
{
    return m_id == right;
}

#include "moc_category.cpp"
