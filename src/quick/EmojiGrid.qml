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

    property real emojiPointSize

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
        emojiPointSize: root.emojiPointSize

        onClicked: root.clicked(emojiDelegate.emoji)
        onRightClicked: root.rightClicked(emojiDelegate.emoji)
        onPressAndHold: {
            if (emojiDelegate.subEmojis.length <= 0) {
                return;
            }
            let subPopup = Qt.createComponent('org.kde.kemoji', 'SubEmojiPopup').createObject(emojiDelegate, {
                emojis: emojiDelegate.subEmojis,
                emojiPointSize: root.emojiPointSize
            }) as KEmoji.SubEmojiPopup;
            subPopup.clicked.connect(emoji => root.clicked(emoji));
            subPopup.rightClicked.connect(emoji => root.rightClicked(emoji));
            subPopup.open();
            subPopup.forceActiveFocus();
        }
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        width: parent.width - (Kirigami.Units.largeSpacing * 8)
        text: categoryFilterModel.currentCategory.id === "recent" ? i18n("No recent Emojis") : i18n("No favorite Emojis")
        visible: root.count === 0 && (categoryFilterModel.currentCategory.id === "recent" || categoryFilterModel.currentCategory.id === "favorite")
    }

    Component {
        id: subEmojiPopup
        KEmoji.SubEmojiPopup {
            onClicked: emoji => root.clicked(emoji)
            onRightClicked: emoji => root.rightClicked(emoji)
        }
    }
}
