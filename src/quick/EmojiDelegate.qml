/*
 *  SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami

import org.kde.kemoji as KEmoji

QQC2.ItemDelegate {
    id: root

    required property KEmoji.Emoji emoji
    property alias emojiPointSize: innerLabel.font.pointSize
    property bool hasSubEmojis: false

    signal rightClicked

    leftPadding: Kirigami.Units.mediumSpacing
    rightPadding: Kirigami.Units.mediumSpacing
    topPadding: Kirigami.Units.mediumSpacing
    bottomPadding: Kirigami.Units.mediumSpacing
    leftInset: Kirigami.Units.smallSpacing
    rightInset: Kirigami.Units.smallSpacing
    topInset: Kirigami.Units.smallSpacing
    bottomInset: Kirigami.Units.smallSpacing
    highlighted: GridView.isCurrentItem

    contentItem: QQC2.Label {
        id: innerLabel
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.family: 'emoji' // Avoid monochrome fonts like DejaVu Sans
        font.pointSize: 25
        text: root.emoji.unicode
        textFormat: Text.PlainText

        Kirigami.Icon {
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            visible: root.hasSubEmojis
            width: Kirigami.Units.gridUnit * 0.5
            height: Kirigami.Units.gridUnit * 0.5
            source: "arrow-down-symbolic"
        }
    }

    Accessible.name: emoji.name
    Accessible.onPressAction: root.clicked()

    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
    QQC2.ToolTip.text: emoji.name
    QQC2.ToolTip.visible: hovered

    Keys.onMenuPressed: event => root.rightClicked()
    Keys.onReturnPressed: event => root.clicked()

    TapHandler {
        acceptedButtons: Qt.RightButton
        onTapped: (eventPoint, button) => root.rightClicked()
    }
}
