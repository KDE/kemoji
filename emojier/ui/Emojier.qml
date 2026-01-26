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

    function report(emoji: KEmoji.Emoji): void {
        if (!visible) {
            return;
        }
        CopyHelper.copyTextToClipboard(emoji.unicode)
        KEmoji.EmojiDict.emojiUsed(emoji);
        window.showPassiveNotification(i18n("%1 copied to the clipboard", emoji.unicode))
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

            model: categoryPage.model.categoryModel
            delegate: Kirigami.Action {
                required property string name
                required property string id
                required property string iconName

                checked: categoryPage.model.currentCategoryId === id
                text: i18ndc("org.kde.plasma.emojier", "Emoji Category", name)
                icon.name: iconName

                onTriggered: categoryPage.model.currentCategoryId = id
            }

            onObjectAdded: (index, object) => {
                drawer.actions.push(object);
            }
        }
    }

    Component.onCompleted: {
        categoryPage.model.currentCategoryId = "recent";
        if (categoryPage.model.rowCount() > 0) {
            return;
        }
        categoryPage.model.currentCategoryId = "favorite";
        if (categoryPage.model.rowCount() > 0) {
            return;
        }
        categoryPage.model.currentCategoryId = "all";
    }
}
