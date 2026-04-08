/*
 *  SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.kemoji as KEmoji

/*!
 \qmltype ToneAction
 \inqmlmodule org.kde.kemoji

 \brief An Action to represent an emoji tone.
 */
QQC2.Action {
    id: root

    /*!
      \brief This property holds the \c KEmoji::Categories::Category the action represents.

      default: \c KEmoji.Categories.Category.None
     */
    property int tone: KEmoji.Tones.Neutral

    // Unfortunately there is nowhere to hide this because Action is based on QtObject.
    readonly property KEmoji.ToneHelper helper: KEmoji.ToneHelper {
        tone: root.tone
    }

    text: helper.exampleUnicodeWithName
    Accessible.name: helper.name
}
