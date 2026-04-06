/*
    SPDX-FileCopyrightText: 2022 Weng Xuetian <wegnxt@gmail.com>
    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>

#include <KLocalizedString>

#include "kemoji_export.h"

using namespace Qt::Literals::StringLiterals;

namespace KEmoji::Categories
{
Q_NAMESPACE_EXPORT(KEMOJI_EXPORT)
QML_ELEMENT

/*!
 * \enum Categoru
 *
 * Enumeration representing the available emoji categories:
 *
 * \value None
 *        The emoji is uncategorized. Normally for an empty or faulty emoji.
 * \value All
 *        Category for all emojis.
 * \value Recent
 *        Category for recently used emojis.
 * \value Favorite
 *        Category for favorite emoijs.
 * \value Custom
 *        Category for custom emojis.
 * \value Smileys
 *        The emoji is from the Smileys & Emotion group of the unicode standard.
 * \value People
 *        The emoji is from the People & Body group of the unicode standard.
 * \value Animals
 *        The emoji is from the Animals & Nature group of the unicode standard.
 * \value Food
 *        The emoji is from the Food & Drink group of the unicode standard.
 * \value Travel
 *        The emoji is from the Travel & Places group of the unicode standard.
 * \value Activities
 *        The emoji is from the Activities group of the unicode standard.
 * \value Objects
 *        The emoji is from the Objects group of the unicode standard.
 * \value Symbols
 *        The emoji is from the Symbols group of the unicode standard.
 * \value Flags
 *        The emoji is from the Flags group of the unicode standard.
 */
enum Category {
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
Q_ENUM_NS(Category);

/*!
 * \brief Return a string with the name of the given category.
 */
inline QString nameStringForCategory(Category category)
{
    switch (category) {
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

/*!
 * \brief Return a string with the icon of the given category.
 */
inline QString iconNameForCategory(Category category)
{
    switch (category) {
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
}
