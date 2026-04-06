/*
 *  SPDX-FileCopyrightText: 2019 Aleix Pol Gonzalez <aleixpol@kde.org>
 *  SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick

import org.kde.kirigami as Kirigami
import org.kde.kemoji as KEmoji

pragma ComponentBehavior: Bound

GridView {
    id: root

    property real emojiPixelSize: Kirigami.Units.iconSizes.small

    signal clicked(emoji: KEmoji.emoji)

    signal rightClicked(emoji: KEmoji.emoji)

    cellWidth: count > 0 && itemAtIndex(0) ? itemAtIndex(0).implicitWidth : 0
    cellHeight: count > 0 && itemAtIndex(0) ? itemAtIndex(0).implicitHeight : 0

    currentIndex: -1
    reuseItems: true

    model: KEmoji.EmojiFilterModel {
        id: categoryFilterModel
        sourceModel: KEmoji.EmojiModel {
            id: emojiModel
            emojis: KEmoji.Dict.emojis
        }
    }

    delegate: KEmoji.EmojiDelegate {
        id: emojiDelegate
        property bool subPopupOpen: false
        emojiPixelSize: root.emojiPixelSize
        highlighted: GridView.isCurrentItem || ListView.isCurrentItem || subPopupOpen

        onClicked: root.clicked(emojiDelegate.emoji)
        onRightClicked: root.rightClicked(emojiDelegate.emoji)
        onPressAndHold: {
            if (emojiDelegate.subEmojis.size <= 0) {
                return;
            }
            let subPopup = Qt.createComponent('org.kde.kemoji', 'SubEmojiPopup').createObject(emojiDelegate, {
                emojis: emojiDelegate.subEmojis,
                emojiPixelSize: root.emojiPixelSize
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
