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

    readonly property real desiredSize: Kirigami.Units.gridUnit * 3
    readonly property int columnsToHave: Math.ceil(width / desiredSize)

    signal clicked(emoji: string)

    signal rightClicked(emoji: string)

    signal pressAndHold(emoji: string)

    cellWidth: desiredSize
    cellHeight: desiredSize

    currentIndex: -1
    reuseItems: true

    model: KEmoji.EmojiFilterModel {
        id: categoryFilterModel
        sourceModel: KEmoji.EmojiModel {
            id: emojiModel
        }
    }

    delegate: KEmoji.EmojiDelegate {
        id: emojiDelegate
        required property list<KEmoji.Emoji> subEmojis

        hasSubEmojis: subEmojis.length > 0
        width: root.cellWidth
        height: root.cellHeight

        onClicked: root.clicked(emojiDelegate.unicode)
        onRightClicked: root.rightClicked(emojiDelegate.unicode)
        onPressAndHold: {
            if (emojiDelegate.subEmojis.length <= 0) {
                return;
            }
            let subPopup = subEmojiPopup.createObject(emojiDelegate, {
                model: emojiDelegate.subEmojis
            }) as KEmoji.SubEmojiPopup;
            subPopup.open();
            subPopup.forceActiveFocus();
        }
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        width: parent.width - (Kirigami.Units.largeSpacing * 8)
        text: i18n("No recent Emojis")
        visible: root.count === 0 && root.model.category === "recent"
    }

    Component {
        id: subEmojiPopup
        KEmoji.SubEmojiPopup {
            onClicked: emoji => root.clicked(emoji)
            onRightClicked: emoji => root.rightClicked(emoji)
        }
    }
}
