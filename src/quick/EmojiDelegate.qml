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

    required property string unicode
    required property string name
    required property list<KEmoji.emoji> subEmojis
    required property KEmoji.emoji emoji
    property alias emojiPointSize: innerLabel.font.pointSize
    property bool showSubEmojis: true

    signal rightClicked

    leftPadding: Kirigami.Units.mediumSpacing
    rightPadding: Kirigami.Units.mediumSpacing
    topPadding: Kirigami.Units.mediumSpacing
    bottomPadding: Kirigami.Units.mediumSpacing
    highlighted: GridView.isCurrentItem

    contentItem: QQC2.Label {
        id: innerLabel
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.family: 'emoji' // Avoid monochrome fonts like DejaVu Sans
        text: root.unicode
        textFormat: Text.PlainText

        Kirigami.Icon {
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            visible: root.subEmojis.length > 0 && root.showSubEmojis
            width: Kirigami.Units.gridUnit * 0.5
            height: Kirigami.Units.gridUnit * 0.5
            source: "arrow-down-symbolic"
        }
    }

    Accessible.name: name
    Accessible.onPressAction: root.clicked()

    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
    QQC2.ToolTip.text: name
    QQC2.ToolTip.visible: hovered

    Keys.onMenuPressed: event => root.rightClicked()
    Keys.onReturnPressed: event => root.clicked()

    TapHandler {
        acceptedButtons: Qt.RightButton
        onTapped: (eventPoint, button) => root.rightClicked()
    }
}
