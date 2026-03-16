// SPDX-FileCopyrightText: 2023 James Graham <james.h.graham@protonmail.com>
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
    QUrl customImageFile = QUrl::fromLocalFile(u"%1/360px-Mascot_konqi.png"_s.arg(DATA_DIR));
    QUrl localTextFile = QUrl::fromLocalFile(u"%1/text_file.txt"_s.arg(DATA_DIR));

private Q_SLOTS:
    void empty();
    void setUnicode();
    void setSource();
};

void EmojiTest::empty()
{
    const auto emoji = Emoji();

    QCOMPARE(emoji.isValid(), false);
    QCOMPARE(emoji.isUnicode(), false);
    QCOMPARE(emoji.isCustom(), false);
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

void EmojiTest::setUnicode()
{
    auto emoji = Emoji(u"😀"_s);

    QCOMPARE(emoji.isValid(), true);
    QCOMPARE(emoji.isUnicode(), true);
    QCOMPARE(emoji.isCustom(), false);
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

void EmojiTest::setSource()
{
    auto emoji = Emoji(customImageFile);

    QCOMPARE(emoji.isValid(), true);
    QCOMPARE(emoji.isUnicode(), false);
    QCOMPARE(emoji.isCustom(), true);
    QCOMPARE(emoji.unicode(), u"�"_s);
    QCOMPARE(emoji.source(), customImageFile);
    QCOMPARE(emoji.unqualifiedUnicode(), QString());
    QCOMPARE(emoji.name(), QString());
    QCOMPARE(emoji.fallbackName(), QString());
    QCOMPARE(emoji.altNames(), QStringList());
    QCOMPARE(emoji.category(), Category());
    QCOMPARE(emoji.toString(), u"�"_s);
    QCOMPARE(emoji.toString(Qt::RichText), u"<img title=\"\" src=\"%1\">"_s.arg(customImageFile.toString()));

    // Should only accept images.
    emoji = Emoji(localTextFile);

    QCOMPARE(emoji.isValid(), false);
    QCOMPARE(emoji.isUnicode(), false);
    QCOMPARE(emoji.isCustom(), false);
    QCOMPARE(emoji.unicode(), u"�"_s);
    QCOMPARE(emoji.source(), QUrl());
    QCOMPARE(emoji.unqualifiedUnicode(), QString());
    QCOMPARE(emoji.name(), QString());
    QCOMPARE(emoji.fallbackName(), QString());
    QCOMPARE(emoji.altNames(), QStringList());
    QCOMPARE(emoji.category(), Category());
    QCOMPARE(emoji.toString(), u"�"_s);
    QCOMPARE(emoji.toString(Qt::RichText), u"�"_s);

    emoji = Emoji(QUrl(u"https://kde.org"_s));

    QCOMPARE(emoji.isValid(), false);
    QCOMPARE(emoji.isUnicode(), false);
    QCOMPARE(emoji.isCustom(), false);
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

QTEST_GUILESS_MAIN(EmojiTest)
#include "emojitest.moc"
