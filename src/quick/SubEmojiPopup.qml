/*
 *  SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kemoji as KEmoji

QQC2.Popup {
    id: root

    property alias model: list.model

    property real delegateSize: Kirigami.Units.gridUnit * 3

    signal clicked(emoji: string)

    signal rightClicked(emoji: string)

    signal pressAndHold(emoji: string)

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
            width: list.cellWidth
            height: list.cellHeight

            onClicked: {
                root.clicked(emojiDelegate.unicode)
                root.close()
            }
            onRightClicked: root.rightClicked(emojiDelegate.unicode)
        }
    }
}
