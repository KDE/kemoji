/*
    SPDX-FileCopyrightText: 2022 Weng Xuetian <wegnxt@gmail.com>
    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <qqmlintegration.h>

#include <KLazyLocalizedString>

#include "kemoji_export.h"

using namespace Qt::Literals::StringLiterals;

namespace KEmoji
{

struct KEMOJI_EXPORT Category {
    Q_GADGET
    QML_ELEMENT

    Q_PROPERTY(QString id MEMBER id CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString iconName MEMBER iconName CONSTANT)

public:
    explicit Category(const QString &id = {}, const KLazyLocalizedString &name = {}, const QString &iconName = {})
        : id(id)
        , iconName(iconName)
        , m_name(name)
    {
    }

    QString id;
    QString name() const
    {
        return m_name.toString();
    }
    QString iconName;

    bool operator==(const Category &right) const
    {
        return id == right.id;
    }
    bool operator==(const QString &right) const
    {
        return id == right;
    }

private:
    KLazyLocalizedString m_name;
};

static const Category emptyCategory = Category();

static const std::unordered_map<QString, Category> categoryDict = {
    {"all"_L1,
     Category{
         "all"_L1,
         kli18nc("Emoji Category", "All"),
         "view-list-icons"_L1,
     }},
    {"recent"_L1,
     Category{
         "recent"_L1,
         kli18nc("Emoji Category", "Recent"),
         "document-open-recent-symbolic"_L1,
     }},
    {"smileysAndEmotion"_L1,
     Category{
         "smileysAndEmotion"_L1,
         kli18nc("Emoji Category", "Smileys and Emotion"),
         "smiley"_L1,
     }},
    {"peopleAndBody"_L1,
     Category{
         "peopleAndBody"_L1,
         kli18nc("Emoji Category", "People and Body"),
         "user"_L1,
     }},
    {"animalsAndNature"_L1,
     Category{
         "animalsAndNature"_L1,
         kli18nc("Emoji Category", "Animals and Nature"),
         "animal"_L1,
     }},
    {"foodAndDrink"_L1,
     Category{
         "foodAndDrink"_L1,
         kli18nc("Emoji Category", "Food and Drink"),
         "food"_L1,
     }},
    {"travelAndPlaces"_L1,
     Category{
         "travelAndPlaces"_L1,
         kli18nc("Emoji Category", "Travel and Places"),
         "globe"_L1,
     }},
    {"activities"_L1,
     Category{
         "activities"_L1,
         kli18nc("Emoji Category", "Activities"),
         "games-highscores"_L1,
     }},
    {"objects"_L1,
     Category{
         "objects"_L1,
         kli18nc("Emoji Category", "Objects"),
         "object"_L1,
     }},
    {"symbols"_L1,
     Category{
         "symbols"_L1,
         kli18nc("Emoji Category", "Symbols"),
         "checkmark-symbolic"_L1,
     }},
    {"flags"_L1,
     Category{
         "flags"_L1,
         kli18nc("Emoji Category", "Flags"),
         "flag"_L1,
     }},
};

}
