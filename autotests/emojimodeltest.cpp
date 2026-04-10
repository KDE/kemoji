// SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#include <QAbstractItemModelTester>
#include <QObject>
#include <QTest>

#include "dict.h"
#include "emojimodel.h"
#include "group.h"

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

    std::list<Emoji> emojis{u"😀"_s, u"🤚🏿"_s, u"🧠"_s, u"🫱🏼‍🫲🏿"_s, u"⚽"_s};
    Group group;
    auto it = emojis.begin();
    while (it != emojis.end()) {
        group.add(it);
        ++it;
    }

    Group handSubGroup = Dict::instance().variantGroupForEmoji(u"🤚"_s).filtered([](const Emoji &familyEmoji) {
        return familyEmoji != u"🤚🏿"_s;
    });
    Group shakeSubGroup = Dict::instance().variantGroupForEmoji(u"🤝"_s).filtered([](const Emoji &familyEmoji) {
        return familyEmoji != u"🫱🏼‍🫲🏿"_s;
    });

    model->setEmojis(group);
    QCOMPARE(model->rowCount(), 5);
    QCOMPARE(model->data(model->index(0), EmojiModel::EmojiRole).value<Emoji>(), u"😀"_s);
    QCOMPARE(model->data(model->index(1), EmojiModel::EmojiRole).value<Emoji>(), u"🤚🏿"_s);
    QCOMPARE(model->data(model->index(2), EmojiModel::EmojiRole).value<Emoji>(), u"🧠"_s);
    QCOMPARE(model->data(model->index(3), EmojiModel::EmojiRole).value<Emoji>(), u"🫱🏼‍🫲🏿"_s);
    QCOMPARE(model->data(model->index(4), EmojiModel::EmojiRole).value<Emoji>(), u"⚽"_s);
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
    QCOMPARE(model->data(model->index(0), EmojiModel::VariantEmojisRole).value<Group>(), emptyGroup);
    QCOMPARE(model->data(model->index(1), EmojiModel::VariantEmojisRole).value<Group>(), handSubGroup);
    QCOMPARE(model->data(model->index(2), EmojiModel::VariantEmojisRole).value<Group>(), emptyGroup);
    QCOMPARE(model->data(model->index(3), EmojiModel::VariantEmojisRole).value<Group>(), shakeSubGroup);
    QCOMPARE(model->data(model->index(4), EmojiModel::VariantEmojisRole).value<Group>(), emptyGroup);
}

QTEST_GUILESS_MAIN(EmojiModelTest)
#include "emojimodeltest.moc"
