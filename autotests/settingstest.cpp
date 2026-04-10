// SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#include <QObject>
#include <QTest>

#include "settings_p.h"

using namespace Qt::Literals::StringLiterals;
using namespace KEmoji;

class SettingsTest : public QObject
{
    Q_OBJECT

private:
    QString customEmojiName = u"Konqi"_s;
    QUrl customImageFile = QUrl::fromLocalFile(u"%1/360px-Mascot_konqi.png"_s.arg(DATA_DIR));
    QUrl localTextFile = QUrl::fromLocalFile(u"%1/text_file.txt"_s.arg(DATA_DIR));

private Q_SLOTS:
    void emojiUsedTest();
    void registerCustomTest();
};

void SettingsTest::emojiUsedTest()
{
    auto &settings = Settings::instance();
    settings.clearHistory();

    const auto emoji1 = u"😀"_s;
    const auto emoji2 = u"😶‍🌫️"_s;
    QCOMPARE(settings.isRecent(emoji1), false);
    QCOMPARE(settings.recentIndex(emoji1), -1);
    QCOMPARE(settings.timesUsed(emoji1), 0);
    QCOMPARE(settings.isRecent(emoji2), false);
    QCOMPARE(settings.recentIndex(emoji2), -1);
    QCOMPARE(settings.timesUsed(emoji2), 0);

    settings.emojiUsed(emoji1);
    QCOMPARE(settings.isRecent(emoji1), true);
    QCOMPARE(settings.recentIndex(emoji1), 0);
    QCOMPARE(settings.timesUsed(emoji1), 1);
    QCOMPARE(settings.isRecent(emoji2), false);
    QCOMPARE(settings.recentIndex(emoji2), -1);
    QCOMPARE(settings.timesUsed(emoji2), 0);

    settings.emojiUsed(emoji1);
    QCOMPARE(settings.isRecent(emoji1), true);
    QCOMPARE(settings.recentIndex(emoji1), 0);
    QCOMPARE(settings.timesUsed(emoji1), 2);
    QCOMPARE(settings.isRecent(emoji2), false);
    QCOMPARE(settings.recentIndex(emoji2), -1);
    QCOMPARE(settings.timesUsed(emoji2), 0);

    settings.emojiUsed(emoji2);
    QCOMPARE(settings.isRecent(emoji1), true);
    QCOMPARE(settings.recentIndex(emoji1), 1);
    QCOMPARE(settings.timesUsed(emoji1), 2);
    QCOMPARE(settings.isRecent(emoji2), true);
    QCOMPARE(settings.recentIndex(emoji2), 0);
    QCOMPARE(settings.timesUsed(emoji2), 1);

    settings.emojiUsed(emoji1);
    QCOMPARE(settings.isRecent(emoji1), true);
    QCOMPARE(settings.recentIndex(emoji1), 0);
    QCOMPARE(settings.timesUsed(emoji1), 3);
    QCOMPARE(settings.isRecent(emoji2), true);
    QCOMPARE(settings.recentIndex(emoji2), 1);
    QCOMPARE(settings.timesUsed(emoji2), 1);

    settings.clearHistory();
}

void SettingsTest::registerCustomTest()
{
    QCOMPARE(Settings::instance().isCustomEmoji(customEmojiName), false);

    auto registered = Settings::instance().registerCustomEmoji(customImageFile, customEmojiName);
    QCOMPARE(registered, true);
    QCOMPARE(Settings::instance().isCustomEmoji(customEmojiName), true);
    QCOMPARE(Settings::instance().customEmojiSource(customEmojiName), customImageFile);

    auto unregistered = Settings::instance().unregisterCustomEmoji(customEmojiName);
    QCOMPARE(unregistered, true);
    QCOMPARE(Settings::instance().isCustomEmoji(customEmojiName), false);
    QCOMPARE(Settings::instance().customEmojiSource(customEmojiName), QUrl());

    registered = Settings::instance().registerCustomEmoji(localTextFile, customEmojiName);
    QCOMPARE(registered, false);
    QCOMPARE(Settings::instance().isCustomEmoji(customEmojiName), false);
    QCOMPARE(Settings::instance().customEmojiSource(customEmojiName), QUrl());

    registered = Settings::instance().registerCustomEmoji(QUrl(u"https://kde.org"_s), customEmojiName);
    QCOMPARE(registered, false);
    QCOMPARE(Settings::instance().isCustomEmoji(customEmojiName), false);
    QCOMPARE(Settings::instance().customEmojiSource(customEmojiName), QUrl());
}

QTEST_GUILESS_MAIN(SettingsTest)
#include "settingstest.moc"
