/*
 *  SPDX-FileCopyrightText: 2019 Aleix Pol Gonzalez <aleixpol@kde.org>
 *  SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

import QtQuick

import org.kde.kirigami as Kirigami
import org.kde.kemoji as KEmoji

pragma ComponentBehavior: Bound

/*!
  \qmltype EmojiGrid
  \inqmlmodule org.kde.kemoji

  \brief A view to visualize a \c KEmoji::Model as a grid.

  By default the grid has a standard \c KEmoji::SortFilterModel with a \c KEmoji::Model
  populated with all known emojis to the \c KEmoji::Dict. This can be replaced with
  any \c KEmoji::Model populated with any \c KEmoji::Emoji the user desires.

  \sa KEmoji::Model, KEmoji::SortFilterModel, KEmoji::Dict, KEmoji::Emoji
 */
GridView {
    id: root

    /*!
     \brief The font to use

     The default uses the "emoji" family and a pixelSize of  \c Kirigami.Units.iconSizes.small.
     The font family should always be one that supports color emojis.
     */
    property font font: ({
        family: "emoji",
        pixelSize: Kirigami.Units.iconSizes.small
    })

    /*!
     \brief Whether unicode sequences not supported by the current font should be shown.

     As new unicode code points are added not all fonts may support them and may be
     missing glyphs, this allows for them not to be shown.
     */
    property alias showUnsupportedEmojis: categoryFilterModel.showUnsupportedEmojis

    /*!
     \brief Emitted when a delegate is clicked.
     */
    signal clicked(emoji: KEmoji.emoji)

    /*!
     \brief Emitted when a delegate is right clicked.
     */
    signal rightClicked(emoji: KEmoji.emoji)

    cellWidth: count > 0 && itemAtIndex(0) ? itemAtIndex(0).implicitWidth : 0
    cellHeight: count > 0 && itemAtIndex(0) ? itemAtIndex(0).implicitHeight : 0

    currentIndex: -1
    reuseItems: true

    model: KEmoji.SortFilterModel {
        id: categoryFilterModel
        sourceModel: KEmoji.Model {
            emojis: KEmoji.Dict.emojis
        }
        currentFont: root.font
    }

    delegate: KEmoji.EmojiDelegate {
        id: emojiDelegate
        property bool subPopupOpen: false
        font: root.font
        highlighted: GridView.isCurrentItem || ListView.isCurrentItem || subPopupOpen

        onClicked: root.clicked(emojiDelegate.emoji)
        onRightClicked: root.rightClicked(emojiDelegate.emoji)
        onPressAndHold: {
            if (emojiDelegate.variantEmojis.size <= 0) {
                return;
            }
            let subPopup = Qt.createComponent('org.kde.kemoji', 'SubEmojiPopup').createObject(emojiDelegate, {
                emojis: emojiDelegate.variantEmojis,
                font: root.font
            }) as KEmoji.SubEmojiPopup;
            subPopup.clicked.connect(emoji => {root.clicked(emoji)});
            subPopup.rightClicked.connect(emoji => root.rightClicked(emoji));
            subPopup.closed.connect(emoji => emojiDelegate.subPopupOpen = false);
            emojiDelegate.subPopupOpen = true;
            subPopup.open();
            subPopup.forceActiveFocus();
        }
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        width: parent.width - (Kirigami.Units.largeSpacing * 8)
        text: categoryFilterModel.currentCategory === KEmoji.Categories.Recent ? i18n("No recent Emojis") : i18n("No favorite Emojis")
        visible: root.count === 0 && (categoryFilterModel.currentCategory === KEmoji.Categories.Recent || categoryFilterModel.currentCategory === KEmoji.Categories.Favorite)
    }
}
