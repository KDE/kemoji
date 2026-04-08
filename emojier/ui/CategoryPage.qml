/*
    SPDX-FileCopyrightText: 2019 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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

    title: helper.name

    KEmoji.CategoryHelper {
        id: helper
        category: emojiGrid.model.currentCategory
    }

    leftPadding: undefined
    rightPadding: undefined
    horizontalPadding: 0

    Keys.onEscapePressed: Qt.quit()

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

            Component.onCompleted: {
                Qt.callLater(forceActiveFocus);
            }

            Keys.onEscapePressed: event => {
                if (text) {
                    clear()
                } else {
                    event.accepted = false
                }
            }
            Keys.onEnterPressed: event => emojiGrid.currentItem?.Keys.enterPressed(event)
            Keys.onReturnPressed: event => emojiGrid.currentItem?.Keys.returnPressed(event)
            Keys.onDownPressed: event => {
                emojiGrid.currentIndex = Math.max(emojiGrid.currentIndex, 0)
                event.accepted = false
            }
            KeyNavigation.down: emojiGrid
            KeyNavigation.right: clearHistoryButton
            Binding {
                root.Keys.forwardTo: [searchField]
                root.KeyNavigation.up: searchField.KeyNavigation.down // explicitly set as this and clear button point there
            }


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
                    if (root.model.currentCategoryId != KEmoji.category.All) {
                        root.model.currentCategoryId = KEmoji.category.All;
                    }
                }
            }
        }

        QQC2.ToolButton {
            id: clearHistoryButton

            readonly property int effectiveWidth: !visible ? 0 : implicitWidth + titleRowLayout.spacing

            KeyNavigation.right: skinToneButton

            visible: emojiGrid.model.currentCategory === KEmoji.Categories.Recent || emojiGrid.model.currentCategory === KEmoji.Categories.Favorite
            enabled: emojiGrid.count > 0
            text: i18n("Clear History")
            icon.name: "edit-clear-history"
            onClicked: KEmoji.Dict.clearHistory();
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
            visible: emojiGrid.model.currentCategory === KEmoji.Categories.All || emojiGrid.model.currentCategory === KEmoji.Categories.People

            Accessible.name: label
            Accessible.role: Accessible.ButtonMenu

            contentItem: RowLayout {
                spacing: Kirigami.Units.smallSpacing

                QQC2.Label {
                    text: skinToneButton.label
                }
                QQC2.Label {
                    text: emojiGrid.model.defaultToneUnicode
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
                    model: KEmoji.Tones.LENGTH
                    delegate: KEmoji.ToneAction {
                        id: toneAction
                        required property int modelData
                        tone: modelData

                        QQC2.ActionGroup.group: skinToneGroup
                        shortcut: "ctrl+%1".arg(modelData + 1)
                        checkable: true
                        checked: emojiGrid.model.defaultTone == modelData
                        onTriggered: emojiGrid.model.defaultTone = modelData
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
            required property KEmoji.emoji emoji

            onClosed: destroy()

            QQC2.MenuItem {
                icon.name: "edit-copy"
                text: i18nc("@item:inmenu", "Copy Emoji")
                onClicked: Window.window.report(emoji)
            }
            QQC2.MenuItem {
                icon.name: "edit-copy"
                text: i18nc("@item:inmenu", "Copy Name")
                onClicked: {
                    CopyHelper.copyTextToClipboard(emojiMenu.emoji.name);
                    window.showPassiveNotification(i18n("%1 copied to the clipboard", emojiMenu.emoji.name));
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
                emoji: emoji,
            });
            menu.popup();
        }
    }
}
