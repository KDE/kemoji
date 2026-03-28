/*
 *  SPDX-FileCopyrightText: 2022 Weng Xuetian <wegnxt@gmail.com>
 *  SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "category.h"

#include <KLocalizedString>

using namespace KEmoji;

Category::Category(Categories id)
    : m_id(id)
{
}

Category::Categories Category::id() const
{
    return m_id;
}

void Category::setId(Categories id)
{
    m_id = id;
}

QString Category::name() const
{
    switch (m_id) {
    case All:
        return i18nc("Emoji Category", "All");
    case Recent:
        return i18nc("Emoji Category", "Recent");
    case Favorite:
        return i18nc("Emoji Category", "Favorite");
    case Custom:
        return i18nc("Emoji Category", "Custom");
    case Smileys:
        return i18nc("Emoji Category", "Smileys and Emotion");
    case People:
        return i18nc("Emoji Category", "People and Body");
    case Animals:
        return i18nc("Emoji Category", "Animals and Nature");
    case Food:
        return i18nc("Emoji Category", "Food and Drink");
    case Travel:
        return i18nc("Emoji Category", "Travel and Places");
    case Activities:
        return i18nc("Emoji Category", "Activities");
    case Objects:
        return i18nc("Emoji Category", "Objects");
    case Symbols:
        return i18nc("Emoji Category", "Symbols");
    case Flags:
        return i18nc("Emoji Category", "Flags");
    default:
        return {};
    }
}

QString Category::iconName() const
{
    switch (m_id) {
    case All:
        return "view-list-icons"_L1;
    case Recent:
        return "document-open-recent-symbolic"_L1;
    case Favorite:
        return "favorite"_L1;
    case Custom:
        return "edit-image"_L1;
    case Smileys:
        return "smiley"_L1;
    case People:
        return "user"_L1;
    case Animals:
        return "animal"_L1;
    case Food:
        return "food"_L1;
    case Travel:
        return "globe"_L1;
    case Activities:
        return "games-highscores"_L1;
    case Objects:
        return "object"_L1;
    case Symbols:
        return "checkmark-symbolic"_L1;
    case Flags:
        return "flag"_L1;
    default:
        return {};
    }
}

bool Category::operator==(const Category &right) const
{
    return m_id == right.id();
}

bool Category::operator==(Category::Categories right) const
{
    return m_id == right;
}

#include "moc_category.cpp"
