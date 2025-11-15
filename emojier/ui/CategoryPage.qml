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

Kirigami.ScrollablePage {
    id: view

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
            category = ""
        }
    }

    titleDelegate: RowLayout {
        id: titleRowLayout

        Layout.fillWidth: true
        Layout.preferredWidth: view.width

        spacing: Kirigami.Units.smallSpacing

        Kirigami.Heading {
            text: view.title
            textFormat: Text.PlainText
            elide: Text.ElideRight
            Layout.fillWidth: true
        }

        Kirigami.SearchField {
            id: searchField
            Layout.fillWidth: true
            Layout.minimumWidth: Kirigami.Units.gridUnit * 5
            Layout.maximumWidth: (Kirigami.Units.gridUnit * 15) - clearHistoryButton.effectiveWidth
            text: view.searchText
            inputMethodHints: Qt.ImhNoPredictiveText

            onTextChanged: {
                forceActiveFocus()
                view.model.searchText = text

                // Always focus the first item if there is one
                if (emojiGrid.count === 0) {
                    emojiGrid.currentIndex = -1;
                } else {
                    emojiGrid.currentIndex = 0;
                }

                // If nothing was found, try again with all emojis
                if (emojiGrid.currentIndex < 0) {
                    if (view.model.category != "all") {
                        view.model.category = "all";
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

            visible: view.showClearHistoryButton
            enabled: emojiGrid.count > 0
            text: i18n("Clear History")
            icon.name: "edit-clear-history"
            onClicked: {
                recentEmojiModel.clearHistory();
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
            required property string label

            onClosed: destroy()

            QQC2.MenuItem {
                icon.name: "edit-copy"
                text: i18nc("@item:inmenu", "Copy Character")
                onClicked: {
                    CopyHelper.copyTextToClipboard(label);
                    window.showPassiveNotification(i18n("%1 copied to the clipboard", label));
                }
            }
            QQC2.MenuItem {
                icon.name: "edit-copy"
                text: i18nc("@item:inmenu", "Copy Description")
                onClicked: {
                    CopyHelper.copyTextToClipboard(label);
                    window.showPassiveNotification(i18n("%1 copied to the clipboard", label));
                }
            }
        }
    }

    KEmoji.EmojiGrid {
        id: emojiGrid
        onClicked: emoji => window.report(emoji);
        onPressAndHold: emoji => {
            const menu = menuComponent.createObject(emojiGrid, {
                label: emoji,
            });
            menu.popup();
        }
    }
}
