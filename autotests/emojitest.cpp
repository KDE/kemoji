// SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include <QObject>
#include <QTest>

#include "emoji.h"

#include "category.h"

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
    void registerCustomTest();
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

    const auto registered = Emoji::registerCustomEmoji(customImageFile, customEmojiName);
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
    QCOMPARE(emoji.toString(Qt::RichText), u"<img title=\"Konqi\" src=\"%1\">"_s.arg(customImageFile.toString()));

    const auto unregistered = Emoji::unregisterCustomEmoji(customEmojiName);
    QCOMPARE(unregistered, true);

    // The unregistered emoji should now be invalid.
    emoji = Emoji(customEmojiName);
    QCOMPARE(emoji.isValid(), false);
}

void EmojiTest::registerCustomTest()
{
    auto registered = Emoji::registerCustomEmoji(customImageFile, customEmojiName);
    QCOMPARE(registered, true);

    auto unregistered = Emoji::unregisterCustomEmoji(customEmojiName);
    QCOMPARE(unregistered, true);

    registered = Emoji::registerCustomEmoji(localTextFile, customEmojiName);
    QCOMPARE(registered, false);

    registered = Emoji::registerCustomEmoji(QUrl(u"https://kde.org"_s), customEmojiName);
    QCOMPARE(registered, false);
}

QTEST_GUILESS_MAIN(EmojiTest)
#include "emojitest.moc"
