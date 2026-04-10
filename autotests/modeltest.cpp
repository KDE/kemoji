// SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#include <QAbstractItemModelTester>
#include <QObject>
#include <QTest>

#include "dict.h"
#include "group.h"
#include "model.h"

using namespace KEmoji;

class ModelTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void populate();
};

void ModelTest::populate()
{
    auto model = new Model(this);
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
    QCOMPARE(model->data(model->index(0), Model::EmojiRole).value<Emoji>(), u"😀"_s);
    QCOMPARE(model->data(model->index(1), Model::EmojiRole).value<Emoji>(), u"🤚🏿"_s);
    QCOMPARE(model->data(model->index(2), Model::EmojiRole).value<Emoji>(), u"🧠"_s);
    QCOMPARE(model->data(model->index(3), Model::EmojiRole).value<Emoji>(), u"🫱🏼‍🫲🏿"_s);
    QCOMPARE(model->data(model->index(4), Model::EmojiRole).value<Emoji>(), u"⚽"_s);
    QCOMPARE(model->data(model->index(0), Model::RecentIndexRole).toInt(), -1);
    QCOMPARE(model->data(model->index(1), Model::RecentIndexRole).toInt(), -1);
    QCOMPARE(model->data(model->index(2), Model::RecentIndexRole).toInt(), -1);
    QCOMPARE(model->data(model->index(3), Model::RecentIndexRole).toInt(), -1);
    QCOMPARE(model->data(model->index(4), Model::RecentIndexRole).toInt(), -1);
    QCOMPARE(model->data(model->index(0), Model::TimesUsedRole).toInt(), 0);
    QCOMPARE(model->data(model->index(1), Model::TimesUsedRole).toInt(), 0);
    QCOMPARE(model->data(model->index(2), Model::TimesUsedRole).toInt(), 0);
    QCOMPARE(model->data(model->index(3), Model::TimesUsedRole).toInt(), 0);
    QCOMPARE(model->data(model->index(4), Model::TimesUsedRole).toInt(), 0);
    QCOMPARE(model->data(model->index(0), Model::VariantEmojisRole).value<Group>(), emptyGroup);
    QCOMPARE(model->data(model->index(1), Model::VariantEmojisRole).value<Group>(), handSubGroup);
    QCOMPARE(model->data(model->index(2), Model::VariantEmojisRole).value<Group>(), emptyGroup);
    QCOMPARE(model->data(model->index(3), Model::VariantEmojisRole).value<Group>(), shakeSubGroup);
    QCOMPARE(model->data(model->index(4), Model::VariantEmojisRole).value<Group>(), emptyGroup);
}

QTEST_GUILESS_MAIN(ModelTest)
#include "modeltest.moc"
