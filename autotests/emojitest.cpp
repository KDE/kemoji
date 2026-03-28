// SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include <QObject>
#include <QTest>

#include "emoji.h"

#include "category.h"
#include "settings.h"

using namespace KEmoji;

class EmojiTest : public QObject
{
    Q_OBJECT

private:
    QString customEmojiName = u"Konqi"_s;
    QUrl customImageFile = QUrl::fromLocalFile(u"%1/360px-Mascot_konqi.png"_s.arg(DATA_DIR));
    QUrl localTextFile = QUrl::fromLocalFile(u"%1/text_file.txt"_s.arg(DATA_DIR));

private Q_SLOTS:
    void empty();
    void unicodeTest();
    void customTest();
    void equalityTest();

    void cleanupTestCase();
};

void EmojiTest::empty()
{
    const auto emoji = Emoji();

    QCOMPARE(emoji.isValid(), false);
    QCOMPARE(emoji.isUnicode(), false);
    QCOMPARE(emoji.isCustom(), false);
    QCOMPARE(emoji.id(), QString());
    QCOMPARE(emoji.unicode(), u"�"_s);
    QCOMPARE(emoji.source(), QUrl());
    QCOMPARE(emoji.unqualifiedUnicode(), QString());
    QCOMPARE(emoji.name(), QString());
    QCOMPARE(emoji.fallbackName(), QString());
    QCOMPARE(emoji.altNames(), QStringList());
    QCOMPARE(emoji.category(), Category());
    QCOMPARE(emoji.toString(), u"�"_s);
    QCOMPARE(emoji.toString(Qt::RichText), u"�"_s);
}

void EmojiTest::unicodeTest()
{
    auto emoji = Emoji(u"😀"_s);

    QCOMPARE(emoji.isValid(), true);
    QCOMPARE(emoji.isUnicode(), true);
    QCOMPARE(emoji.isCustom(), false);
    QCOMPARE(emoji.id(), u"😀"_s);
    QCOMPARE(emoji.unicode(), u"😀"_s);
    QCOMPARE(emoji.source(), QUrl());
    QCOMPARE(emoji.unqualifiedUnicode(), QString());
    QCOMPARE(emoji.name(), QString());
    QCOMPARE(emoji.fallbackName(), QString());
    QCOMPARE(emoji.altNames(), QStringList());
    QCOMPARE(emoji.category(), Category());
    QCOMPARE(emoji.toString(), u"😀"_s);
    QCOMPARE(emoji.toString(Qt::RichText), u"😀"_s);

    // An Emoji is exactly 1 emoji.
    emoji = Emoji(u"😀😀"_s);
    QCOMPARE(emoji.isValid(), false);
    QCOMPARE(emoji.isUnicode(), false);
    QCOMPARE(emoji.isCustom(), false);
    QCOMPARE(emoji.id(), QString());
    QCOMPARE(emoji.unicode(), u"�"_s);
    QCOMPARE(emoji.source(), QUrl());
    QCOMPARE(emoji.unqualifiedUnicode(), QString());
    QCOMPARE(emoji.name(), QString());
    QCOMPARE(emoji.fallbackName(), QString());
    QCOMPARE(emoji.altNames(), QStringList());
    QCOMPARE(emoji.category(), Category());
    QCOMPARE(emoji.toString(), u"�"_s);
    QCOMPARE(emoji.toString(Qt::RichText), u"�"_s);
}

void EmojiTest::customTest()
{
    // Should not be able to set an unregistered custom emoji
    auto emoji = Emoji(customEmojiName);

    QCOMPARE(emoji.isValid(), false);
    QCOMPARE(emoji.isUnicode(), false);
    QCOMPARE(emoji.isCustom(), false);
    QCOMPARE(emoji.id(), QString());
    QCOMPARE(emoji.unicode(), u"�"_s);
    QCOMPARE(emoji.source(), QUrl());
    QCOMPARE(emoji.unqualifiedUnicode(), QString());
    QCOMPARE(emoji.name(), QString());
    QCOMPARE(emoji.fallbackName(), QString());
    QCOMPARE(emoji.altNames(), QStringList());
    QCOMPARE(emoji.category(), Category());
    QCOMPARE(emoji.toString(), u"�"_s);
    QCOMPARE(emoji.toString(Qt::RichText), u"�"_s);

    const auto registered = Settings::instance().registerCustomEmoji(customImageFile, customEmojiName);
    QCOMPARE(registered, true);
    emoji.setName(customEmojiName);

    QCOMPARE(emoji.isValid(), true);
    QCOMPARE(emoji.isUnicode(), false);
    QCOMPARE(emoji.isCustom(), true);
    QCOMPARE(emoji.id(), customEmojiName);
    QCOMPARE(emoji.unicode(), u"�"_s);
    QCOMPARE(emoji.source(), customImageFile);
    QCOMPARE(emoji.unqualifiedUnicode(), QString());
    QCOMPARE(emoji.name(), customEmojiName);
    QCOMPARE(emoji.fallbackName(), QString());
    QCOMPARE(emoji.altNames(), QStringList());
    QCOMPARE(emoji.category(), Category());
    QCOMPARE(emoji.toString(), u"�"_s);
    QCOMPARE(emoji.toString(Qt::RichText), u"<img title=\"Konqi\" src=\"%1\" />"_s.arg(customImageFile.toString()));

    const auto unregistered = Settings::instance().unregisterCustomEmoji(customEmojiName);
    QCOMPARE(unregistered, true);

    // The unregistered emoji should now be invalid.
    emoji = Emoji(customEmojiName);
    QCOMPARE(emoji.isValid(), false);
}

void EmojiTest::equalityTest()
{
    auto emoji1 = Emoji(u"😀"_s);
    auto emoji2 = Emoji(u"😀"_s);
    QCOMPARE(emoji1 == emoji2, true);

    emoji2 = Emoji(u"🤠"_s);
    QCOMPARE(emoji1 == emoji2, false);

    const auto registered = Settings::instance().registerCustomEmoji(customImageFile, customEmojiName);
    QCOMPARE(registered, true);
    emoji1 = Emoji(customEmojiName);
    emoji2 = Emoji(customEmojiName);
    QCOMPARE(emoji1 == emoji2, true);
}

void EmojiTest::cleanupTestCase()
{
    // If a test fails the custom emoji may end up still registered.
    Settings::instance().unregisterCustomEmoji(customEmojiName);
}

QTEST_GUILESS_MAIN(EmojiTest)
#include "emojitest.moc"
