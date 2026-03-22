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

    required property KEmoji.emoji emoji
    required property KEmoji.group subEmojis
    property alias emojiPixelSize: innerLabel.font.pixelSize
    property bool showSubEmojis: true

    signal rightClicked

    implicitWidth: metrics.height + leftPadding + rightPadding
    implicitHeight: metrics.height + leftPadding + rightPadding
    leftPadding: Kirigami.Units.mediumSpacing
    rightPadding: Kirigami.Units.mediumSpacing
    topPadding: Kirigami.Units.mediumSpacing
    bottomPadding: Kirigami.Units.mediumSpacing
    spacing: 0
    highlighted: GridView.isCurrentItem || ListView.isCurrentItem

    FontMetrics {
        id: metrics
        font.pixelSize: root.emojiPixelSize
    }

    contentItem: QQC2.Label {
        id: innerLabel
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.family: 'emoji' // Avoid monochrome fonts like DejaVu Sans
        text: root.emoji.toString(Qt.RichText)
        textFormat: Text.PlainText

        Kirigami.Icon {
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            visible: root.subEmojis.size > 0 && root.showSubEmojis
            width: Kirigami.Units.gridUnit * 0.5
            height: Kirigami.Units.gridUnit * 0.5
            source: "arrow-down-symbolic"
        }
    }

    Accessible.name: root.emoji.name
    Accessible.onPressAction: root.clicked()

    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
    QQC2.ToolTip.text: root.emoji.name
    QQC2.ToolTip.visible: hovered

    Keys.onMenuPressed: event => root.rightClicked()
    Keys.onReturnPressed: event => root.clicked()

    TapHandler {
        acceptedButtons: Qt.RightButton
        onTapped: (eventPoint, button) => root.rightClicked()
    }
}
