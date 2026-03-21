// SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include <QAbstractItemModelTester>
#include <QObject>
#include <QTest>

#include "emojimodel.h"

using namespace KEmoji;

class EmojiModelTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void populate();
};

void EmojiModelTest::populate()
{
    auto model = new EmojiModel(this);
    auto tester = new QAbstractItemModelTester(model, this);
    tester->setUseFetchMore(true);

    QCOMPARE(model->rowCount(), 0);

    model->setEmojis({u"😀"_s, u"🤚🏿"_s, u"🧠"_s, u"🧏🏼"_s, u"⚽"_s});
    QCOMPARE(model->rowCount(), 5);
    QCOMPARE(model->data(model->index(0), EmojiModel::EmojiRole).view<Emoji>(), u"😀"_s);
    QCOMPARE(model->data(model->index(1), EmojiModel::EmojiRole).view<Emoji>(), u"🤚🏿"_s);
    QCOMPARE(model->data(model->index(2), EmojiModel::EmojiRole).view<Emoji>(), u"🧠"_s);
    QCOMPARE(model->data(model->index(3), EmojiModel::EmojiRole).view<Emoji>(), u"🧏🏼"_s);
    QCOMPARE(model->data(model->index(4), EmojiModel::EmojiRole).view<Emoji>(), u"⚽"_s);
    QCOMPARE(model->data(model->index(0), EmojiModel::RecentIndexRole).toInt(), -1);
    QCOMPARE(model->data(model->index(1), EmojiModel::RecentIndexRole).toInt(), -1);
    QCOMPARE(model->data(model->index(2), EmojiModel::RecentIndexRole).toInt(), -1);
    QCOMPARE(model->data(model->index(3), EmojiModel::RecentIndexRole).toInt(), -1);
    QCOMPARE(model->data(model->index(4), EmojiModel::RecentIndexRole).toInt(), -1);
    QCOMPARE(model->data(model->index(0), EmojiModel::TimesUsedRole).toInt(), 0);
    QCOMPARE(model->data(model->index(1), EmojiModel::TimesUsedRole).toInt(), 0);
    QCOMPARE(model->data(model->index(2), EmojiModel::TimesUsedRole).toInt(), 0);
    QCOMPARE(model->data(model->index(3), EmojiModel::TimesUsedRole).toInt(), 0);
    QCOMPARE(model->data(model->index(4), EmojiModel::TimesUsedRole).toInt(), 0);
}

QTEST_GUILESS_MAIN(EmojiModelTest)
#include "emojimodeltest.moc"
