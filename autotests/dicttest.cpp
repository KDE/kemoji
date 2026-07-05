// SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#include <QObject>
#include <QSignalSpy>
#include <QTest>

#include <KEmoji/Category>
#include <KEmoji/Dict>

using namespace KEmoji;

class DictTest : public QObject
{
    Q_OBJECT

private:
    QString customEmojiName = u"Konqi"_s;
    QUrl customImageFile = QUrl::fromLocalFile(u"%1/360px-Mascot_konqi.png"_s.arg(DATA_DIR));

private Q_SLOTS:
    void initTestCase();

    void loadTest();
    void categoriesTest();
    void customTest();

    void cleanupTestCase();
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

void DictTest::customTest()
{
    auto &dict = Dict::instance();
    QCOMPARE(dict.categoryGroup(Categories::Custom).isEmpty(), true);
    QCOMPARE(dict.categoryGroup(Categories::Custom).size(), 0);
    QCOMPARE(dict.categoryGroup(Categories::All).indexForEmoji(customEmojiName) == -1, true);
    QCOMPARE(dict.categories().contains(Categories::Custom), false);

    auto registered = dict.registerCustomEmoji(customImageFile, customEmojiName);
    QCOMPARE(registered, true);

    QCOMPARE(dict.categoryGroup(Categories::Custom).isEmpty(), false);
    QCOMPARE(dict.categoryGroup(Categories::Custom).size(), 1);
    QCOMPARE(dict.categoryGroup(Categories::Custom).at(0), customEmojiName);
    QCOMPARE(dict.categoryGroup(Categories::All).indexForEmoji(customEmojiName) != -1, true);
    QCOMPARE(dict.categories().contains(Categories::Custom), true);

    // If we register again it should return true saying the custom already exists
    // but the group size should still only be 1.
    registered = dict.registerCustomEmoji(customImageFile, customEmojiName);
    QCOMPARE(registered, true);

    QCOMPARE(dict.categoryGroup(Categories::Custom).isEmpty(), false);
    QCOMPARE(dict.categoryGroup(Categories::Custom).size(), 1);
    QCOMPARE(dict.categoryGroup(Categories::Custom).at(0), customEmojiName);
    QCOMPARE(dict.categoryGroup(Categories::All).indexForEmoji(customEmojiName) != -1, true);
    QCOMPARE(dict.categories().contains(Categories::Custom), true);

    const auto unregistered = dict.unregisterCustomEmoji(customEmojiName);
    QCOMPARE(unregistered, true);

    QCOMPARE(dict.categoryGroup(Categories::Custom).isEmpty(), true);
    QCOMPARE(dict.categoryGroup(Categories::Custom).size(), 0);
    QCOMPARE(dict.categoryGroup(Categories::All).indexForEmoji(customEmojiName) == -1, true);
    QCOMPARE(dict.categories().contains(Categories::Custom), false);
}

void DictTest::cleanupTestCase()
{
    // If a test fails the custom emoji may end up still registered.
    Dict::instance().unregisterCustomEmoji(customEmojiName);
}

QTEST_GUILESS_MAIN(DictTest)
#include "dicttest.moc"
