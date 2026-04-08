/*
 *  SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami

import org.kde.kemoji as KEmoji

QQC2.ItemDelegate {
    id: root

    required property KEmoji.emoji emoji
    required property KEmoji.group subEmojis
    property int emojiPixelSize
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

    contentItem: Loader {
        sourceComponent: root.emoji.category === KEmoji.Categories.Custom ? innerImage : innerLabel

        Kirigami.Icon {
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            visible: root.subEmojis.size > 0 && root.showSubEmojis
            width: Kirigami.Units.gridUnit * 0.5
            height: Kirigami.Units.gridUnit * 0.5
            source: "arrow-down-symbolic"
        }
    }

    Component {
        id: innerLabel
        QQC2.Label {
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.family: 'emoji' // Avoid monochrome fonts like DejaVu Sans
            font.pixelSize: root.emojiPixelSize
            text: root.emoji.unicode
            textFormat: Text.PlainText
        }
    }

    Component {
        id: innerImage
        Image {
            fillMode: Image.PreserveAspectFit
            horizontalAlignment: Image.AlignHCenter
            verticalAlignment: Image.AlignVCenter
            source: root.emoji.source
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
