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

/*!
 \qmltype EmojiDelegate
 \inqmlmodule org.kde.kemoji

 \brief A delegate to visualize a \c KEmoji::Emoji

 The delegate can handle unicode and custom emojis, it grabs the correct parameters
 from the given \c KEmoji::Emoji automatically.

 When clicked the delegate will call \c KEmoji.Dict.emojiUsed so the user does not
 need to do this automatically.

 \sa KEmoji::Emoji, KEmoji::Dict
 */
QQC2.ItemDelegate {
    id: root

    /*!
     \brief The \c KEmoji::Emoji to visualize.

     \sa KEmoji::Emoji
     */
    required property KEmoji.emoji emoji

    /*!
     \brief The \c KEmoji::Group containing the variants of this emoji.

     \sa KEmoji::Group
     */
    required property KEmoji.group variantEmojis

    /*!
     \brief Whether any variant emojis should be shown.
     */
    property bool showVariantEmojis: true

    /*!
     \brief The size of the emoji in pixels.

     The delegate will be a square sized to hold the emoji of this size including
     any padding.
     */
    property int emojiPixelSize

    /*!
     \brief Emitted when the delegate pis right clicked.
     */
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
            visible: root.variantEmojis.size > 0 && root.showVariantEmojis
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

    onClicked: KEmoji.Dict.emojiUsed(root.emoji)

    TapHandler {
        acceptedButtons: Qt.RightButton
        onTapped: (eventPoint, button) => root.rightClicked()
    }
}
