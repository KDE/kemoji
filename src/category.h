/*
    SPDX-FileCopyrightText: 2022 Weng Xuetian <wegnxt@gmail.com>
    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QtQmlIntegration/qqmlintegration.h>

#include <KLazyLocalizedString>

#include "kemoji_export.h"

using namespace Qt::Literals::StringLiterals;

namespace KEmoji
{

constexpr inline auto allCategoryID = "all"_L1;
constexpr inline auto recentCategoryID = "recent"_L1;
constexpr inline auto favoriteCategoryID = "favorite"_L1;

class KEMOJI_EXPORT Category
{
    Q_GADGET
    QML_ELEMENT

    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString iconName READ iconName CONSTANT)

public:
    explicit Category(const QString &id = {}, const KLazyLocalizedString &name = {}, const QString &iconName = {});

    QString id() const;
    QString name() const;
    QString iconName() const;

    bool isEmpty() const;

    bool operator==(const Category &right) const;
    bool operator==(const QString &right) const;

private:
    QString m_id;
    KLazyLocalizedString m_name;
    QString m_iconName;
};

static const Category emptyCategory = Category();

static const QHash<QString, Category> categoryDict = {
    {allCategoryID, Category(allCategoryID, kli18nc("Emoji Category", "All"), "view-list-icons"_L1)},
    {recentCategoryID, Category(recentCategoryID, kli18nc("Emoji Category", "Recent"), "document-open-recent-symbolic"_L1)},
    {favoriteCategoryID, Category(favoriteCategoryID, kli18nc("Emoji Category", "Favorite"), "favorite"_L1)},
    {"smileysAndEmotion"_L1, Category("smileysAndEmotion"_L1, kli18nc("Emoji Category", "Smileys and Emotion"), "smiley"_L1)},
    {"peopleAndBody"_L1, Category("peopleAndBody"_L1, kli18nc("Emoji Category", "People and Body"), "user"_L1)},
    {"animalsAndNature"_L1, Category("animalsAndNature"_L1, kli18nc("Emoji Category", "Animals and Nature"), "animal"_L1)},
    {"foodAndDrink"_L1, Category("foodAndDrink"_L1, kli18nc("Emoji Category", "Food and Drink"), "food"_L1)},
    {"travelAndPlaces"_L1, Category("travelAndPlaces"_L1, kli18nc("Emoji Category", "Travel and Places"), "globe"_L1)},
    {"activities"_L1, Category("activities"_L1, kli18nc("Emoji Category", "Activities"), "games-highscores"_L1)},
    {"objects"_L1, Category("objects"_L1, kli18nc("Emoji Category", "Objects"), "object"_L1)},
    {"symbols"_L1, Category("symbols"_L1, kli18nc("Emoji Category", "Symbols"), "checkmark-symbolic"_L1)},
    {"flags"_L1, Category("flags"_L1, kli18nc("Emoji Category", "Flags"), "flag"_L1)},
};
}
