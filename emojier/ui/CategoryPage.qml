/*
    SPDX-FileCopyrightText: 2019 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kemoji as KEmoji

import org.kde.emojier

pragma ComponentBehavior: Bound

Kirigami.ScrollablePage {
    id: root

    property alias model: emojiGrid.model
    property string searchText: ""
    property bool showClearHistoryButton: false

    title: model.currentCategory.name

    leftPadding: undefined
    rightPadding: undefined
    horizontalPadding: 0

    Keys.onPressed: event => {
        if (event.key === Qt.Key_Escape) {
            Qt.quit()
        }
        if (event.text.length > 0 && event.modifiers === Qt.NoModifier) {
            // We want to prevent unprintable characters like backspace
            model = emoji
            searchText += /[\x00-\x1F\x7F]/.test(event.text) ? "" : event.text
            text: i18nc("@title:page All emojis", "All")
            root.model.currentCategoryId = ""
        }
    }

    titleDelegate: RowLayout {
        id: titleRowLayout

        Layout.fillWidth: true
        Layout.preferredWidth: root.width

        spacing: Kirigami.Units.smallSpacing

        Kirigami.Heading {
            text: root.title
            textFormat: Text.PlainText
            elide: Text.ElideRight
            Layout.fillWidth: true
        }

        Kirigami.SearchField {
            id: searchField
            Layout.fillWidth: true
            Layout.minimumWidth: Kirigami.Units.gridUnit * 5
            Layout.maximumWidth: (Kirigami.Units.gridUnit * 15) - clearHistoryButton.effectiveWidth - skinToneButton.effectiveWidth
            text: root.searchText
            inputMethodHints: Qt.ImhNoPredictiveText

            onTextChanged: {
                forceActiveFocus()
                root.model.searchText = text

                // Always focus the first item if there is one
                if (emojiGrid.count === 0) {
                    emojiGrid.currentIndex = -1;
                } else {
                    emojiGrid.currentIndex = 0;
                }

                // If nothing was found, try again with all emojis
                if (emojiGrid.currentIndex < 0) {
                    if (root.model.currentCategoryId != "all") {
                        root.model.currentCategoryId = "all";
                    }
                }
            }
            Component.onCompleted: {
                Qt.callLater(forceActiveFocus);
            }
            Keys.onEscapePressed: event => {
                if (text) {
                    clear()
                } else {
                    Qt.quit()
                }
            }
            Keys.forwardTo: emojiGrid
        }

        QQC2.ToolButton {
            id: clearHistoryButton

            readonly property int effectiveWidth: !visible ? 0 : implicitWidth + titleRowLayout.spacing

            visible: root.showClearHistoryButton
            enabled: emojiGrid.count > 0
            text: i18n("Clear History")
            icon.name: "edit-clear-history"
            onClicked: {
                recentEmojiModel.clearHistory();
            }
        }
        QQC2.ToolButton {
            id: skinToneButton

            readonly property int effectiveWidth: !visible ? 0 : implicitWidth + titleRowLayout.spacing
            readonly property string label: i18nc("@action:button Button to open a menu that lets you choose a skin tone", "Skin tone:")

            function openMenu() {
                if (!skinToneMenu.visible) {
                    skinToneMenu.open()
                } else {
                    skinToneMenu.dismiss()
                }
            }

            down: pressed || skinToneMenu.visible
            visible: emojiGrid.model.currentCategory.id === "all" || emojiGrid.model.currentCategory.id === "peopleAndBody"

            Accessible.name: label
            Accessible.role: Accessible.ButtonMenu

            contentItem: RowLayout {
                spacing: Kirigami.Units.smallSpacing

                QQC2.Label {
                    text: skinToneButton.label
                }
                QQC2.Label {
                    text: emojiGrid.model.sourceModel.defaultToneUnicode
                    font.pixelSize: Kirigami.Units.iconSizes.smallMedium
                }
            }

            Layout.preferredHeight: clearHistoryButton.implicitHeight

            leftPadding: LayoutMirroring.enabled ? Kirigami.Units.iconSizes.smallMedium : Kirigami.Units.largeSpacing
            rightPadding: LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : Kirigami.Units.iconSizes.smallMedium

            onPressed: openMenu()
            Keys.onReturnPressed: openMenu()
            Keys.onEnterPressed: openMenu()

            Keys.onDownPressed: event => searchField.Keys.downPressed(event)

            QQC2.Menu {
                id: skinToneMenu
                y: skinToneButton.height
                QQC2.ActionGroup {
                    id: skinToneGroup
                    exclusive: true
                }
                Instantiator {
                    model: KEmoji.ToneModel {}
                    delegate: Kirigami.Action {
                        required property int index
                        required property string name
                        required property int tone
                        required property string unicodeNameString

                        QQC2.ActionGroup.group: skinToneGroup
                        text: unicodeNameString
                        Accessible.name: name
                        shortcut: "ctrl+%1".arg(index + 1)
                        checkable: true
                        checked: emojiGrid.model.sourceModel.defaultTone == tone
                        onTriggered: emojiGrid.model.sourceModel.defaultTone = tone
                    }

                    onObjectAdded: (index, object) => skinToneMenu.contentData.push(object)
                }
            }
        }
    }

    Shortcut {
        sequences: [StandardKey.Copy]
        enabled: emojiGrid.currentItem
        onActivated: {
            emojiGrid.currentItem.Keys.returnPressed(null)
        }
    }

    Component {
        id: menuComponent

        QQC2.Menu {
            id: emojiMenu
            required property string label

            onClosed: destroy()

            QQC2.MenuItem {
                icon.name: "edit-copy"
                text: i18nc("@item:inmenu", "Copy Character")
                onClicked: {
                    CopyHelper.copyTextToClipboard(emojiMenu.label);
                    window.showPassiveNotification(i18n("%1 copied to the clipboard", emojiMenu.label));
                }
            }
            QQC2.MenuItem {
                icon.name: "edit-copy"
                text: i18nc("@item:inmenu", "Copy Name")
                onClicked: {
                    CopyHelper.copyTextToClipboard(emojiMenu.label);
                    window.showPassiveNotification(i18n("%1 copied to the clipboard", emojiMenu.label));
                }
            }
        }
    }

    KEmoji.EmojiGrid {
        id: emojiGrid
        emojiPixelSize: Kirigami.Units.iconSizes.medium
        onClicked: emoji => Window.window.report(emoji);
        onRightClicked: emoji => {
            const menu = menuComponent.createObject(emojiGrid, {
                label: emoji,
            });
            menu.popup();
        }
    }
}
