/*
 *  SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kemoji as KEmoji

pragma ComponentBehavior: Bound

QQC2.Popup {
    id: root

    property alias model: list.model

    property real delegateSize: Kirigami.Units.gridUnit * 3

    signal clicked(emoji: KEmoji.Emoji)

    signal rightClicked(emoji: KEmoji.Emoji)

    signal pressAndHold(emoji: KEmoji.Emoji)

    padding: 0
    width: delegateSize * Math.min(model.length, 5) + leftPadding + rightPadding
    height: list.contentHeight
    y: -height
    modal: true
    popupType: QQC2.Popup.Native

    GridView {
        id: list
        anchors.fill: parent
        cellWidth: root.delegateSize
        cellHeight: root.delegateSize

        currentIndex: -1
        reuseItems: true

        delegate: KEmoji.EmojiDelegate {
            id: emojiDelegate
            required property KEmoji.Emoji modelData

            width: list.cellWidth
            height: list.cellHeight

            emoji: modelData

            onClicked: {
                root.clicked(modelData)
                root.close()
            }
            onRightClicked: root.rightClicked(modelData)
        }
    }
}
