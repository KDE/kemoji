/*
 *  SPDX-FileCopyrightText: 2019 Aleix Pol Gonzalez <aleixpol@kde.org>
 *  SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kemoji as KEmoji

pragma ComponentBehavior: Bound

GridView {
    id: root

    readonly property real desiredSize: Kirigami.Units.gridUnit * 3
    readonly property int columnsToHave: Math.ceil(width / desiredSize)
    readonly property int delayInterval: Math.min(300, columnsToHave * 10)

    signal clicked(emoji: string)

    signal pressAndHold(emoji: string)

    cellWidth: width / columnsToHave
    cellHeight: desiredSize

    currentIndex: -1
    reuseItems: true

    model: KEmoji.EmojiFilterModel {
        id: categoryFilterModel
        sourceModel: KEmoji.EmojiModel {
            id: emojiModel
        }
    }

    delegate: QQC2.ItemDelegate {
        id: emojiLabel

        required property string unicode
        required property string description

        width: root.cellWidth
        height: root.cellHeight

        highlighted: GridView.isCurrentItem
        contentItem: QQC2.Label {
            font.pointSize: 25
            font.family: 'emoji' // Avoid monochrome fonts like DejaVu Sans
            fontSizeMode: emojiLabel.unicode > 5 ? Text.Fit : Text.FixedSize
            minimumPointSize: 10
            text: emojiLabel.unicode
            textFormat: Text.PlainText
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        Accessible.name: emojiLabel.description
        Accessible.onPressAction: root.clicked(emojiLabel.unicode)

        QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
        QQC2.ToolTip.text: emojiLabel.description
        QQC2.ToolTip.visible: emojiLabel.hovered

        Keys.onMenuPressed: event => root.pressAndHold(emojiLabel.unicode)
        Keys.onReturnPressed: event => root.clicked(emojiLabel.unicode)

        onClicked: root.clicked(emojiLabel.unicode)

        onPressAndHold: root.pressAndHold(emojiLabel.unicode)

        TapHandler {
            acceptedButtons: Qt.RightButton
            onTapped: (eventPoint, button) => root.pressAndHold(emojiLabel.unicode)
        }
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        width: parent.width - (Kirigami.Units.largeSpacing * 8)
        text: i18n("No recent Emojis")
        visible: root.count === 0 && root.model.category === "recent"
    }
}
