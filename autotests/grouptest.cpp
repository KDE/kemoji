// SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include <QAbstractItemModelTester>
#include <QObject>
#include <QTest>

#include "group.h"

using namespace KEmoji;

class GroupTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void populate();
};

void GroupTest::populate()
{
    std::list<Emoji> emojis{u"😀"_s, u"🤚🏿"_s, u"🧠"_s, u"🫱🏼‍🫲🏿"_s, u"⚽"_s};
    Group group;

    QCOMPARE(group.size(), 0);
    QCOMPARE(group.isEmpty(), true);

    auto it = emojis.begin();
    while (it != emojis.end()) {
        group.add(it);
        ++it;
    }

    QCOMPARE(group.size(), 5);
    QCOMPARE(group.isEmpty(), false);
    QCOMPARE(group.at(0), u"😀"_s);
    QCOMPARE(group.at(1), u"🤚🏿"_s);
    QCOMPARE(group.at(2), u"🧠"_s);
    QCOMPARE(group.at(3), u"🫱🏼‍🫲🏿"_s);
    QCOMPARE(group.at(4), u"⚽"_s);
    QCOMPARE(group.indexForEmoji(u"😀"_s), 0);
    QCOMPARE(group.indexForEmoji(u"🤚🏿"_s), 1);
    QCOMPARE(group.indexForEmoji(u"🧠"_s), 2);
    QCOMPARE(group.indexForEmoji(u"🫱🏼‍🫲🏿"_s), 3);
    QCOMPARE(group.indexForEmoji(u"⚽"_s), 4);
    QCOMPARE(group.contains(u"😀"_s), true);
    QCOMPARE(group.contains(u"🤚🏿"_s), true);
    QCOMPARE(group.contains(u"🧠"_s), true);
    QCOMPARE(group.contains(u"🫱🏼‍🫲🏿"_s), true);
    QCOMPARE(group.contains(u"⚽"_s), true);
    QCOMPARE(group.contains(u"👶"_s), false);
    QCOMPARE(group.contains(u"🇧🇿"_s), false);

    const auto filteredGroup = group.filtered([](const Emoji &emoji) {
        return emoji != u"🧠"_s;
    });

    QCOMPARE(filteredGroup.size(), 4);
    QCOMPARE(filteredGroup.isEmpty(), false);
    QCOMPARE(filteredGroup.at(0), u"😀"_s);
    QCOMPARE(filteredGroup.at(1), u"🤚🏿"_s);
    QCOMPARE(filteredGroup.at(2), u"🫱🏼‍🫲🏿"_s);
    QCOMPARE(filteredGroup.at(3), u"⚽"_s);
    QCOMPARE(filteredGroup.indexForEmoji(u"😀"_s), 0);
    QCOMPARE(filteredGroup.indexForEmoji(u"🤚🏿"_s), 1);
    QCOMPARE(filteredGroup.indexForEmoji(u"🧠"_s), -1);
    QCOMPARE(filteredGroup.indexForEmoji(u"🫱🏼‍🫲🏿"_s), 2);
    QCOMPARE(filteredGroup.indexForEmoji(u"⚽"_s), 3);
    QCOMPARE(filteredGroup.contains(u"😀"_s), true);
    QCOMPARE(filteredGroup.contains(u"🤚🏿"_s), true);
    QCOMPARE(filteredGroup.contains(u"🧠"_s), false);
    QCOMPARE(filteredGroup.contains(u"🫱🏼‍🫲🏿"_s), true);
    QCOMPARE(filteredGroup.contains(u"⚽"_s), true);
    QCOMPARE(filteredGroup.contains(u"👶"_s), false);
    QCOMPARE(filteredGroup.contains(u"🇧🇿"_s), false);
}

QTEST_GUILESS_MAIN(GroupTest)
#include "grouptest.moc"
