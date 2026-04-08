/*
 *  SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kemoji as KEmoji

pragma ComponentBehavior: Bound

QQC2.Popup {
    id: root

    property alias emojis: emojiModel.emojis

    property real emojiPixelSize

    signal clicked(emoji: KEmoji.emoji)

    signal rightClicked(emoji: KEmoji.emoji)

    signal pressAndHold(emoji: KEmoji.emoji)

    padding: 0
    width: list.cellWidth * Math.min(list.count, 5) + leftPadding + rightPadding
    height: list.cellHeight * Math.max(list.count / 5, 1) + topPadding + bottomPadding
    y: -height
    modal: true
    popupType: QQC2.Popup.Native

    GridView {
        id: list
        anchors.fill: parent
        cellWidth: count > 0 ? itemAtIndex(0).implicitWidth : 0
        cellHeight: count > 0 ? itemAtIndex(0).implicitHeight : 0

        currentIndex: -1
        reuseItems: true

        model: KEmoji.EmojiModel {
            id: emojiModel
        }
        delegate: KEmoji.EmojiDelegate {
            emojiPixelSize: root.emojiPixelSize
            showSubEmojis: false

            onClicked: {
                root.clicked(emoji)
                root.close()
            }
            onRightClicked: root.rightClicked(emoji)
        }

        FontMetrics {
            id: metrics
            font.pixelSize: root.emojiPixelSize
        }
    }
}
