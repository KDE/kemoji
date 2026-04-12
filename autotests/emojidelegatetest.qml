// SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick
import QtTest
import QtQuick.Controls as QQC2

import org.kde.kemoji as KEmoji

TestCase {
    name: "EmojiDelegateTest"

    KEmoji.EmojiDelegate {
        id: delegate

        emoji: emoji1
        variantEmojis: testGroup
    }

    function test_emojiData(): void {
        compare(delegate.contentItem.item.text, "😀");
        compare(delegate.text, "Name 1");
        compare(delegate.contentItem.item instanceof QQC2.Label, true)
    }
}
