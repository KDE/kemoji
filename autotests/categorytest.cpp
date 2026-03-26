// SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include <QObject>
#include <QTest>

#include "category.h"

using namespace KEmoji;

class CategoryTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void valuesTest_data();
    void valuesTest();
};

void CategoryTest::valuesTest_data()
{
    QTest::addColumn<Category::Categories>("id");
    QTest::addColumn<QString>("name");
    QTest::addColumn<QLatin1String>("iconName");

    QTest::newRow("All") << Category::All << u"All"_s << "view-list-icons"_L1;
    QTest::newRow("Recent") << Category::Recent << u"Recent"_s << "document-open-recent-symbolic"_L1;
    QTest::newRow("Recent") << Category::Favorite << u"Favorite"_s << "favorite"_L1;
    QTest::newRow("Recent") << Category::Smileys << u"Smileys and Emotion"_s << "smiley"_L1;
    QTest::newRow("Recent") << Category::People << u"People and Body"_s << "user"_L1;
    QTest::newRow("Recent") << Category::Animals << u"Animals and Nature"_s << "animal"_L1;
    QTest::newRow("Recent") << Category::Food << u"Food and Drink"_s << "food"_L1;
    QTest::newRow("Recent") << Category::Travel << u"Travel and Places"_s << "globe"_L1;
    QTest::newRow("Recent") << Category::Activities << u"Activities"_s << "games-highscores"_L1;
    QTest::newRow("Recent") << Category::Objects << u"Objects"_s << "object"_L1;
    QTest::newRow("Recent") << Category::Symbols << u"Symbols"_s << "checkmark-symbolic"_L1;
    QTest::newRow("Recent") << Category::Flags << u"Flags"_s << "flag"_L1;
    QTest::newRow("Recent") << Category::None << u""_s << ""_L1;
}

void CategoryTest::valuesTest()
{
    QFETCH(Category::Categories, id);
    QFETCH(QString, name);
    QFETCH(QLatin1String, iconName);

    const auto category = Category(id);

    QCOMPARE(category.id(), id);
    QCOMPARE(category.name(), name);
    QCOMPARE(category.iconName(), iconName);
}

QTEST_GUILESS_MAIN(CategoryTest)
#include "categorytest.moc"
