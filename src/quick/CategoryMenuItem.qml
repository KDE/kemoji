/*
 *  SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kemoji as KEmoji

pragma ComponentBehavior: Bound

/*!
  \qmltype CategoryMenuItem
  \inqmlmodule org.kde.kemoji

  \brief A control to visualize a \c KEmoji::Group in a menu.

  \code
  QQC2.Menu {
      QQC2.MenuItem {
          text: "Menu Item Before"
      }
      KEmoji.CategoryMenuItem {
          category: KEmoji.Categories.Favorite
      }
      QQC2.MenuItem {
          text: "Menu Item After"
      }
  }
  \endcode

  \sa KEmoji::Group
 */
QQC2.Control {
    id: root

    /*!
      \brief The \c KEmoji::Category to visualize.

      \sa KEmoji::Categories::Category
     */
    required property int category

    /*!
      \brief Emitted when a delegate is clicked.
     */
    signal clicked(emoji: KEmoji.emoji)

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    font: ({
        family: "emoji",
        pixelSize: Kirigami.Units.iconSizes.small
    })

    contentItem: ListView {
        implicitHeight: count > 0 && itemAtIndex(0) ? itemAtIndex(0).implicitHeight : 0
        currentIndex: -1
        orientation: ListView.Horizontal
        interactive: false

        model: KEmoji.Model {
            id: emojiModel
            emojis: KEmoji.Dict.categoryGroup(root.category)
        }
        delegate: KEmoji.EmojiDelegate {
            id: emojiDelegate
            font: root.font
            onClicked: root.clicked(emojiDelegate.emoji)
        }

        Connections {
            target: KEmoji.Dict

            function onEmojisChanged(categories): void {
                emojiModel.emojis = KEmoji.Dict.categoryGroup(root.category);
            }
        }
    }
}
