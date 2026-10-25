/*
 *  SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QFont>
#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>

#include "category.h"
#include "dict.h"

/*!
 * \qmltype Dict
 * \inqmlmodule org.kde.kemoji
 * \nativetype KEmoji::Dict
 */
class QuickDict : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Dict)
    QML_SINGLETON

    Q_PROPERTY(KEmoji::Group emojis READ emojis NOTIFY allEmojisChanged)

    Q_PROPERTY(QList<KEmoji::Categories::Category> categories READ categories NOTIFY categoriesChanged)

    Q_PROPERTY(QRangeModel *categoryModel READ categoryModel CONSTANT)

public:
    QuickDict(QObject *parent = nullptr)
        : QObject(parent)
    {
        connect(&KEmoji::Dict::instance(), &KEmoji::Dict::categoriesChanged, this, &QuickDict::categoriesChanged);
        connect(&KEmoji::Dict::instance(), &KEmoji::Dict::emojisChanged, this, [this](const QList<KEmoji::Categories::Category> &categories) {
            QVariantList varCategories;
            std::ranges::for_each(categories, [&varCategories](KEmoji::Categories::Category category) {
                varCategories.append(int(category));
            });
            Q_EMIT emojisChanged(varCategories);
        });
        connect(&KEmoji::Dict::instance(), &KEmoji::Dict::emojisChanged, this, [this](QList<KEmoji::Categories::Category> categories) {
            if (categories.contains(KEmoji::Categories::All)) {
                Q_EMIT allEmojisChanged();
            }
        });
    }

    const KEmoji::Group emojis() const
    {
        return KEmoji::Dict::instance().emojis();
    }

    Q_INVOKABLE [[nodiscard]] bool emojiSupportedByFont(const KEmoji::Emoji &emoji, const QFont &font) const
    {
        return KEmoji::Dict::instance().emojiSupportedByFont(emoji, font);
    }

    const QList<KEmoji::Categories::Category> &categories() const
    {
        return KEmoji::Dict::instance().categories();
    }

    QRangeModel *categoryModel() const
    {
        return KEmoji::Dict::instance().categoryModel();
    }

    Q_INVOKABLE const KEmoji::Group categoryGroup(KEmoji::Categories::Category category) const
    {
        return KEmoji::Dict::instance().categoryGroup(category);
    }

    Q_INVOKABLE void emojiUsed(const KEmoji::Emoji &emoji)
    {
        KEmoji::Dict::instance().emojiUsed(emoji);
    }

    Q_INVOKABLE void unregisterCustomEmoji(const KEmoji::Emoji &emoji)
    {
        KEmoji::Dict::instance().unregisterCustomEmoji(emoji.name());
    }

    Q_INVOKABLE void clearHistory()
    {
        KEmoji::Dict::instance().clearHistory();
    }

Q_SIGNALS:
    void emojisChanged(QVariantList categorys);
    void allEmojisChanged();
    void categoriesChanged();
};
