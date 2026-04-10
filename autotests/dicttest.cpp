// SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#include "dict.h"
#include "category.h"
#include <QObject>
#include <QSignalSpy>
#include <QTest>

#include <KEmoji/Category>
#include <KEmoji/Dict>

using namespace KEmoji;

class DictTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    void loadTest();
    void categoriesTest();
};

void DictTest::initTestCase()
{
    const auto &dict = Dict::instance();

    QSignalSpy spy(&dict, &Dict::loadedChanged);
    QVERIFY(spy.wait(5000));
}

void DictTest::loadTest()
{
    const auto &dict = Dict::instance();

    QCOMPARE(dict.loaded(), true);
    QCOMPARE(dict.categoryGroup(Categories::All), dict.emojis());
}

// These categories should all be populated.
void DictTest::categoriesTest()
{
    const auto &dict = Dict::instance();
    QCOMPARE(dict.categoryGroup(Categories::Smileys).isEmpty(), false);
    QCOMPARE(dict.categoryGroup(Categories::People).isEmpty(), false);
    QCOMPARE(dict.categoryGroup(Categories::Animals).isEmpty(), false);
    QCOMPARE(dict.categoryGroup(Categories::Food).isEmpty(), false);
    QCOMPARE(dict.categoryGroup(Categories::Travel).isEmpty(), false);
    QCOMPARE(dict.categoryGroup(Categories::Activities).isEmpty(), false);
    QCOMPARE(dict.categoryGroup(Categories::Objects).isEmpty(), false);
    QCOMPARE(dict.categoryGroup(Categories::Symbols).isEmpty(), false);
    QCOMPARE(dict.categoryGroup(Categories::Flags).isEmpty(), false);
}

QTEST_GUILESS_MAIN(DictTest)
#include "dicttest.moc"
