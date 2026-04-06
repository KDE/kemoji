/*
 *  SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.kemoji as KEmoji

/*!
 \qmltype Action
 \inqmlmodule org.kde.kemoji

 \brief An Action to represent an emoji category.
 */
QQC2.Action {
    id: root

    /*!
     \brief This property holds the \c KEmoji::Categories::Category the action represents.

     default: \c KEmoji.Categories.Category.None
    */
    property int category: KEmoji.Categories.None

    // Unfortunately there is nowhere to hide this because Action is based on QtObject.
    readonly property KEmoji.CategoryHelper helper: KEmoji.CategoryHelper {
        category: root.category
    }

    text: helper.name
    icon.name: helper.iconName
}
