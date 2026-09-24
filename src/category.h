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

/*!
 * \namespace KEmoji::Categories
 * \inmodule KEmoji
 */
namespace KEmoji::Categories
{
Q_NAMESPACE_EXPORT(KEMOJI_EXPORT)
QML_ELEMENT

/*!
 * \enum KEmoji::Categories::Category
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
KEMOJI_EXPORT QString nameStringForCategory(Category category);

/*!
 * \brief Return a string with the icon of the given category.
 */
KEMOJI_EXPORT QString iconNameForCategory(Category category);
}
