// SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#include "category.h"
#include <QFont>
#include <QFontDatabase>
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
    QUrl localTextFile = QUrl::fromLocalFile(u"%1/text_file.txt"_s.arg(DATA_DIR));
    QUrl remoteFile = QUrl(u"https://kde.org"_s);

private Q_SLOTS:
    void initTestCase();

    void loadTest();
    void categoriesTest();
    void emojiUsedTest();
    void customTest();
    void glyphAvailableTest();

    void cleanupTestCase();
};

void DictTest::initTestCase()
{
    const auto &dict = Dict::instance();

    QSignalSpy spy(&dict, &Dict::loadingChanged);
    while (dict.loading()) {
        QVERIFY(spy.wait(5000));
    }
}

void DictTest::loadTest()
{
    const auto &dict = Dict::instance();

    QCOMPARE(dict.loading(), false);
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

void DictTest::emojiUsedTest()
{
    auto &dict = Dict::instance();
    dict.clearHistory();

    const auto emoji1 = u"😀"_s;
    const auto emoji2 = u"😶‍🌫️"_s;
    QCOMPARE(dict.isRecentEmoji(emoji1), false);
    QCOMPARE(dict.recentEmojiIndex(emoji1), -1);
    QCOMPARE(dict.timesEmojiUsed(emoji1), 0);
    QCOMPARE(dict.categoryGroup(Categories::Recent).contains(emoji1), false);
    QCOMPARE(dict.categoryGroup(Categories::Favorite).contains(emoji1), false);
    QCOMPARE(dict.isRecentEmoji(emoji2), false);
    QCOMPARE(dict.recentEmojiIndex(emoji2), -1);
    QCOMPARE(dict.timesEmojiUsed(emoji2), 0);
    QCOMPARE(dict.categoryGroup(Categories::Recent).contains(emoji2), false);
    QCOMPARE(dict.categoryGroup(Categories::Favorite).contains(emoji2), false);

    dict.emojiUsed(emoji1);
    QCOMPARE(dict.isRecentEmoji(emoji1), true);
    QCOMPARE(dict.recentEmojiIndex(emoji1), 0);
    QCOMPARE(dict.timesEmojiUsed(emoji1), 1);
    QCOMPARE(dict.categoryGroup(Categories::Recent).contains(emoji1), true);
    QCOMPARE(dict.categoryGroup(Categories::Favorite).contains(emoji1), true);
    QCOMPARE(dict.isRecentEmoji(emoji2), false);
    QCOMPARE(dict.recentEmojiIndex(emoji2), -1);
    QCOMPARE(dict.timesEmojiUsed(emoji2), 0);
    QCOMPARE(dict.categoryGroup(Categories::Recent).contains(emoji2), false);
    QCOMPARE(dict.categoryGroup(Categories::Favorite).contains(emoji2), false);

    dict.emojiUsed(emoji1);
    QCOMPARE(dict.isRecentEmoji(emoji1), true);
    QCOMPARE(dict.recentEmojiIndex(emoji1), 0);
    QCOMPARE(dict.timesEmojiUsed(emoji1), 2);
    QCOMPARE(dict.categoryGroup(Categories::Recent).contains(emoji1), true);
    QCOMPARE(dict.categoryGroup(Categories::Favorite).contains(emoji1), true);
    QCOMPARE(dict.isRecentEmoji(emoji2), false);
    QCOMPARE(dict.recentEmojiIndex(emoji2), -1);
    QCOMPARE(dict.timesEmojiUsed(emoji2), 0);
    QCOMPARE(dict.categoryGroup(Categories::Recent).contains(emoji2), false);
    QCOMPARE(dict.categoryGroup(Categories::Favorite).contains(emoji2), false);

    dict.emojiUsed(emoji2);
    QCOMPARE(dict.isRecentEmoji(emoji1), true);
    QCOMPARE(dict.recentEmojiIndex(emoji1), 1);
    QCOMPARE(dict.timesEmojiUsed(emoji1), 2);
    QCOMPARE(dict.categoryGroup(Categories::Recent).contains(emoji1), true);
    QCOMPARE(dict.categoryGroup(Categories::Favorite).contains(emoji1), true);
    QCOMPARE(dict.isRecentEmoji(emoji2), true);
    QCOMPARE(dict.recentEmojiIndex(emoji2), 0);
    QCOMPARE(dict.timesEmojiUsed(emoji2), 1);
    QCOMPARE(dict.categoryGroup(Categories::Recent).contains(emoji2), true);
    QCOMPARE(dict.categoryGroup(Categories::Favorite).contains(emoji2), true);

    dict.emojiUsed(emoji1);
    QCOMPARE(dict.isRecentEmoji(emoji1), true);
    QCOMPARE(dict.recentEmojiIndex(emoji1), 0);
    QCOMPARE(dict.timesEmojiUsed(emoji1), 3);
    QCOMPARE(dict.categoryGroup(Categories::Recent).contains(emoji1), true);
    QCOMPARE(dict.categoryGroup(Categories::Favorite).contains(emoji1), true);
    QCOMPARE(dict.isRecentEmoji(emoji2), true);
    QCOMPARE(dict.recentEmojiIndex(emoji2), 1);
    QCOMPARE(dict.timesEmojiUsed(emoji2), 1);
    QCOMPARE(dict.categoryGroup(Categories::Recent).contains(emoji2), true);
    QCOMPARE(dict.categoryGroup(Categories::Favorite).contains(emoji2), true);

    dict.clearHistory();
    QCOMPARE(dict.isRecentEmoji(emoji1), false);
    QCOMPARE(dict.recentEmojiIndex(emoji1), -1);
    QCOMPARE(dict.timesEmojiUsed(emoji1), 0);
    QCOMPARE(dict.categoryGroup(Categories::Recent).contains(emoji1), false);
    QCOMPARE(dict.categoryGroup(Categories::Favorite).contains(emoji1), false);
    QCOMPARE(dict.isRecentEmoji(emoji2), false);
    QCOMPARE(dict.recentEmojiIndex(emoji2), -1);
    QCOMPARE(dict.timesEmojiUsed(emoji2), 0);
    QCOMPARE(dict.categoryGroup(Categories::Recent).contains(emoji2), false);
    QCOMPARE(dict.categoryGroup(Categories::Favorite).contains(emoji2), false);
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
    QCOMPARE(dict.categoryGroup(Categories::Custom).at(0).source(), customImageFile);
    QCOMPARE(dict.categoryGroup(Categories::All).indexForEmoji(customEmojiName) != -1, true);
    QCOMPARE(dict.categories().contains(Categories::Custom), true);

    // If we register again it should return true saying the custom already exists
    // but the group size should still only be 1.
    registered = dict.registerCustomEmoji(customImageFile, customEmojiName);
    QCOMPARE(registered, true);
    QCOMPARE(dict.categoryGroup(Categories::Custom).isEmpty(), false);
    QCOMPARE(dict.categoryGroup(Categories::Custom).size(), 1);
    QCOMPARE(dict.categoryGroup(Categories::Custom).at(0), customEmojiName);
    QCOMPARE(dict.categoryGroup(Categories::Custom).at(0).source(), customImageFile);
    QCOMPARE(dict.categoryGroup(Categories::All).indexForEmoji(customEmojiName) != -1, true);
    QCOMPARE(dict.categories().contains(Categories::Custom), true);

    auto unregistered = dict.unregisterCustomEmoji(customEmojiName);
    QCOMPARE(unregistered, true);
    QCOMPARE(dict.categoryGroup(Categories::Custom).isEmpty(), true);
    QCOMPARE(dict.categoryGroup(Categories::Custom).size(), 0);
    QCOMPARE(dict.categoryGroup(Categories::All).indexForEmoji(customEmojiName) == -1, true);
    QCOMPARE(dict.categories().contains(Categories::Custom), false);

    registered = dict.registerCustomEmoji(localTextFile, customEmojiName);
    QCOMPARE(registered, false);
    QCOMPARE(dict.categoryGroup(Categories::Custom).isEmpty(), true);
    QCOMPARE(dict.categoryGroup(Categories::Custom).size(), 0);
    QCOMPARE(dict.categoryGroup(Categories::All).indexForEmoji(customEmojiName) == -1, true);
    QCOMPARE(dict.categories().contains(Categories::Custom), false);

    registered = dict.registerCustomEmoji(remoteFile, customEmojiName);
    QCOMPARE(registered, true);
    QCOMPARE(dict.categoryGroup(Categories::Custom).isEmpty(), false);
    QCOMPARE(dict.categoryGroup(Categories::Custom).size(), 1);
    QCOMPARE(dict.categoryGroup(Categories::Custom).at(0), customEmojiName);
    QCOMPARE(dict.categoryGroup(Categories::Custom).at(0).source(), remoteFile);
    QCOMPARE(dict.categoryGroup(Categories::All).indexForEmoji(customEmojiName) != -1, true);
    QCOMPARE(dict.categories().contains(Categories::Custom), true);

    unregistered = dict.unregisterCustomEmoji(customEmojiName);
    QCOMPARE(unregistered, true);
    QCOMPARE(dict.categoryGroup(Categories::Custom).isEmpty(), true);
    QCOMPARE(dict.categoryGroup(Categories::Custom).size(), 0);
    QCOMPARE(dict.categoryGroup(Categories::All).indexForEmoji(customEmojiName) == -1, true);
    QCOMPARE(dict.categories().contains(Categories::Custom), false);
}

void DictTest::glyphAvailableTest()
{
    auto emoji = Emoji(u"😀"_s);
    bool hasEmojiFont = std::ranges::any_of(QFontDatabase::families(), [](const QString &family) {
        return family.contains(u"emoji", Qt::CaseInsensitive);
    });

    if (hasEmojiFont) {
#ifdef Q_OS_WINDOWS
        QFont emojiFont(u"Segoe UI Emoji"_s);
#else
        QFont emojiFont(u"emoji"_s);
#endif
        QCOMPARE(Dict::instance().emojiSupportedByFont(emoji, emojiFont), true);
    }

    auto noFont = QFont();
    QCOMPARE(Dict::instance().emojiSupportedByFont(emoji, noFont), false);
}

void DictTest::cleanupTestCase()
{
    // If a test fails make sure all settings data is removed.
    Dict::instance().unregisterCustomEmoji(customEmojiName);
    Dict::instance().clearHistory();
}

QTEST_MAIN(DictTest)
#include "dicttest.moc"
