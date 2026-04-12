// SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick
import QtTest
import QtQuick.Controls as QQC2

import org.kde.kemoji as KEmoji

TestCase {
    id: root
    name: "EmojiGridTest"
    visible: true
    when: windowShown

    width: 500
    height: 500

    Component {
        id: testGrid
        KEmoji.EmojiGrid {
            width: 100
            height: 100
            model: KEmoji.SortFilterModel {
                id: categoryFilterModel
                sourceModel: KEmoji.Model {
                    emojis: testGroup
                }
            }
        }
    }

    SignalSpy {
        id: spyClicked
    }

    function test_limitedDataSet(): void {
        const grid = createTemporaryObject(testGrid, root);
        spyClicked.target = grid;
        spyClicked.signalName = "clicked"
        verify(grid);
        compare(grid.count, 4);
        compare(grid.currentIndex, -1);
        compare(spyClicked.count, 0);

        grid.forceActiveFocus();
        keyClick(Qt.Key_Right);
        compare(grid.currentIndex, 0);
        keyClick(Qt.Key_Return);
        compare(spyClicked.count, 1);
        compare(spyClicked.signalArguments[0][0].toString(), "😀")
        compare(spyClicked.signalArguments[0][0].name, "Name 1")
        compare(spyClicked.signalArguments[0][0].category, KEmoji.Categories.Smileys)

        keyClick(Qt.Key_Right);
        compare(grid.currentIndex, 1);
        keyClick(Qt.Key_Return);
        compare(spyClicked.count, 2);
        compare(spyClicked.signalArguments[1][0].toString(), "😃")
        compare(spyClicked.signalArguments[1][0].name, "Name 2")
        compare(spyClicked.signalArguments[1][0].category, KEmoji.Categories.Smileys)

        keyClick(Qt.Key_Right);
        compare(grid.currentIndex, 2);
        keyClick(Qt.Key_Return);
        compare(spyClicked.count, 3);
        compare(spyClicked.signalArguments[2][0].toString(), "😄")
        compare(spyClicked.signalArguments[2][0].name, "Name 3")
        compare(spyClicked.signalArguments[2][0].category, KEmoji.Categories.Smileys)

        keyClick(Qt.Key_Right);
        compare(grid.currentIndex, 3);
        keyClick(Qt.Key_Return);
        compare(spyClicked.count, 4);
        compare(spyClicked.signalArguments[3][0].toString(), "😁")
        compare(spyClicked.signalArguments[3][0].name, "Name 4")
        compare(spyClicked.signalArguments[3][0].category, KEmoji.Categories.Smileys)

        keyClick(Qt.Key_Left);
        compare(grid.currentIndex, 2);
        keyClick(Qt.Key_Return);
        compare(spyClicked.count, 5);
        compare(spyClicked.signalArguments[4][0].toString(), "😄")
        compare(spyClicked.signalArguments[4][0].name, "Name 3")
        compare(spyClicked.signalArguments[4][0].category, KEmoji.Categories.Smileys)

        keyClick(Qt.Key_Up);
        compare(grid.currentIndex, 0);
        keyClick(Qt.Key_Return);
        compare(spyClicked.count, 6);
        compare(spyClicked.signalArguments[5][0].toString(), "😀")
        compare(spyClicked.signalArguments[5][0].name, "Name 1")
        compare(spyClicked.signalArguments[5][0].category, KEmoji.Categories.Smileys)
    }
}
