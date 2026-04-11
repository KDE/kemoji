// SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#include "sortfiltermodel.h"
#include "category.h"
#include "group.h"
#include "model.h"
#include "tones.h"
#include <QAbstractItemModelTester>
#include <QSignalSpy>
#include <QTest>

#include <KEmoji/Category>
#include <KEmoji/Dict>
#include <KEmoji/Model>
#include <KEmoji/SortFilterModel>

using namespace KEmoji;

class SortFilterModelTest : public QObject
{
    Q_OBJECT

private:
    Model *model;
    SortFilterModel *sortFilterModel;
    QAbstractItemModelTester *tester;
    int unfilteredCount;

private Q_SLOTS:
    void initTestCase();

    void categoryTest();
    void toneFilterTest();
    void searchFilterTest();

    void cleanup();
};

void SortFilterModelTest::initTestCase()
{
    const auto &dict = Dict::instance();

    QSignalSpy spy(&dict, &Dict::loadedChanged);
    QVERIFY(spy.wait(5000));

    model = new Model(this);
    sortFilterModel = new SortFilterModel(this);
    sortFilterModel->setSourceModel(model);
    tester = new QAbstractItemModelTester(sortFilterModel, this);
    tester->setUseFetchMore(true);

    QCOMPARE(model->rowCount(), 0);
    QCOMPARE(sortFilterModel->rowCount(), 0);

    model->setEmojis(Dict::instance().emojis());

    unfilteredCount = sortFilterModel->rowCount();
    QCOMPARE(model->rowCount() > 0, true);
    QCOMPARE(unfilteredCount > 0, true);

    QCOMPARE(sortFilterModel->searchText(), QString());
    QCOMPARE(sortFilterModel->currentCategory(), Categories::All);
    QCOMPARE(sortFilterModel->toneFilter(), Tones::Neutral);
}

void SortFilterModelTest::categoryTest()
{
    sortFilterModel->setCurrentCategory(Categories::Smileys);
    QCOMPARE(unfilteredCount > sortFilterModel->rowCount(), true);
    auto firstEmoji = sortFilterModel->index(0, 0).data(Model::EmojiRole).value<Emoji>();
    QCOMPARE(firstEmoji.category(), Categories::Smileys);
    auto lastEmlojiEmoji = sortFilterModel->index(sortFilterModel->rowCount() - 1, 0).data(Model::EmojiRole).value<Emoji>();
    QCOMPARE(firstEmoji.category(), Categories::Smileys);

    sortFilterModel->setCurrentCategory(Categories::Flags);
    QCOMPARE(unfilteredCount > sortFilterModel->rowCount(), true);
    firstEmoji = sortFilterModel->index(0, 0).data(Model::EmojiRole).value<Emoji>();
    QCOMPARE(firstEmoji.category(), Categories::Flags);
    lastEmlojiEmoji = sortFilterModel->index(sortFilterModel->rowCount() - 1, 0).data(Model::EmojiRole).value<Emoji>();
    QCOMPARE(firstEmoji.category(), Categories::Flags);

    // Even when searching all items must be in the category.
    sortFilterModel->setSearchText(u"B"_s);
    firstEmoji = sortFilterModel->index(0, 0).data(Model::EmojiRole).value<Emoji>();
    QCOMPARE(firstEmoji.category(), Categories::Flags);
    lastEmlojiEmoji = sortFilterModel->index(sortFilterModel->rowCount() - 1, 0).data(Model::EmojiRole).value<Emoji>();
    QCOMPARE(firstEmoji.category(), Categories::Flags);
}

void SortFilterModelTest::toneFilterTest()
{
    // Because the People & Body has the toned emojis this will reduce searching.
    sortFilterModel->setCurrentCategory(Categories::People);

    for (int i = 0; i < Tones::LENGTH; ++i) {
        sortFilterModel->setToneFilter(static_cast<Tones::Tone>(i));
        int row = 0;
        auto tonedEmoji = sortFilterModel->index(row, row).data(Model::EmojiRole).value<Emoji>();
        auto subGroup = Dict::instance().variantGroupForEmoji(tonedEmoji);
        // Find an emoji that has tones.
        while (subGroup.size() > 0) {
            ++row;
            tonedEmoji = sortFilterModel->index(row, row).data(Model::EmojiRole).value<Emoji>();
            subGroup = Dict::instance().variantGroupForEmoji(tonedEmoji);
        }
        QCOMPARE(Tones::tonesForEmoji(tonedEmoji).contains(sortFilterModel->toneFilter()), true);
    }
}

void SortFilterModelTest::searchFilterTest()
{
    sortFilterModel->setSearchText(u"raised hand"_s);
    // Perfect match should be first.
    auto firstEmoji = sortFilterModel->index(0, 0).data(Model::EmojiRole).value<Emoji>();
    QCOMPARE(firstEmoji, u"✋"_s);
}

void SortFilterModelTest::cleanup()
{
    sortFilterModel->setSearchText({});
    sortFilterModel->setCurrentCategory(Categories::All);
    sortFilterModel->setToneFilter(Tones::Neutral);
}

QTEST_GUILESS_MAIN(SortFilterModelTest)
#include "sortfiltermodeltest.moc"
