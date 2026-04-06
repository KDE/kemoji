/*
    SPDX-FileCopyrightText: 2019 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.config as KConfig

import org.kde.kemoji as KEmoji
import org.kde.emojier

pragma ComponentBehavior: Bound

Kirigami.ApplicationWindow {
    id: window

    minimumWidth: Math.round(minimumHeight * 1.25)
    // Correct height required for no scrolling — drawer's header's
    // implicit height is used instead of height, so add the difference
    minimumHeight: drawer.contentHeight + (drawer.header.height - drawer.header.implicitHeight)

    width: Kirigami.Units.gridUnit * 25
    height: Kirigami.Units.gridUnit * 25

    pageStack {
        initialPage: CategoryPage {
            id: categoryPage
        }
    }

    KConfig.WindowStateSaver {
        configGroupName: "MainWindow"
    }

    function report(emoji: KEmoji.emoji): void {
        if (!visible) {
            return;
        }
        CopyHelper.copyTextToClipboard(emoji.toString(Qt.RichText))
        KEmoji.Dict.emojiUsed(emoji);
        window.showPassiveNotification(i18n("%1 copied to the clipboard", emoji.toString(Qt.RichText)))
    }

    globalDrawer: Kirigami.GlobalDrawer {
        id: drawer

        collapsible: !topContent.activeFocus
        showHeaderWhenCollapsed: true
        collapseButtonVisible: false
        collapsed: true
        modal: false

        header: Kirigami.AbstractApplicationHeader {
            leftPadding: Kirigami.Units.smallSpacing
            rightPadding: Kirigami.Units.smallSpacing

            QQC2.ToolButton {
                anchors.left: parent.left
                anchors.right: parent.right
                text: drawer.collapsed ? "" : qsTr("Close Sidebar")
                icon.name: {
                    if (drawer.collapsed) {
                        return Qt.application.layoutDirection === Qt.RightToLeft ? "sidebar-expand-right" : "sidebar-expand-left";
                    } else {
                        return Qt.application.layoutDirection === Qt.RightToLeft ? "sidebar-collapse-right" : "sidebar-collapse-left";
                    }
                }

                onClicked: drawer.collapsed = !drawer.collapsed

                QQC2.ToolTip.visible: drawer.collapsed && (Kirigami.Settings.tabletMode ? pressed : hovered)
                QQC2.ToolTip.text: qsTr("Open Sidebar")
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
            }
        }

        Instantiator {
            id: instantiator
            model: KEmoji.Dict.categories
            delegate: Kirigami.Action {
                id: categoryAction
                required property int modelData

                readonly property KEmoji.CategoryHelper helper: KEmoji.CategoryHelper {
                    category: categoryAction.modelData
                }

                checked: categoryPage.model.currentCategory === modelData
                text: i18ndc("org.kde.plasma.emojier", "Emoji Category", helper.name)
                icon.name: helper.iconName

                onTriggered: categoryPage.model.currentCategory = modelData
            }

            onObjectAdded: (index, object) => {
                drawer.actions.push(object);
            }
            onObjectRemoved: (index, object) => {
                const idx = drawer.actions.indexOf(object);
                drawer.actions.splice(idx, 1);
            }
        }
    }

    Component.onCompleted: {
        categoryPage.model.setCurrentCategory(KEmoji.Categories.Recent);
        if (categoryPage.model.rowCount() > 0) {
            return;
        }
        categoryPage.model.setCurrentCategory(KEmoji.Categories.Favorite);
        if (categoryPage.model.rowCount() > 0) {
            return;
        }
        categoryPage.model.setCurrentCategory(KEmoji.Categories.All);
    }
}
