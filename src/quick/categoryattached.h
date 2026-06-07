/*
 *  SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>

#include "category.h"

/*!
 * \class CategoryAttached
 *
 * \brief An attached property to allow KEmoji::Categories::Category data to be accessed
 * in QML.
 *
 * By declaring a Category object in QML and assigning it a category value the
 * relevant category name string and icon name strings can be obtained.
 *
 * \code
 * import org.kde.kemoji as KEmoji
 *
 * ...
 * Repeater {
 *   model: 4
 *   delegate: QQC2.Action {
 *     required property int modelData
 *
 *     KEmoji.Category.category: modelData
 *     text: KEmoji.Category.name
 *     icon.name: KEmoji.Category.iconName
 *     onTriggered: {
 *       ...
 *     }
 *   }
 * }
 * ...
 * \endcode
 *
 * While primarily designed for setting the properties of an action it can be used more
 * generally.
 *
 * \code
 * ...
 * Kirigami.ScrollablePage {
 *   KEmoji.Category.category: KEmoji.Category.Animals
 *   title: KEmoji.Category.name
 * ...
 * \endcode
 */
class CategoryAttached : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Category)
    QML_ATTACHED(CategoryAttached)

    /*!
     * \brief The \c KEmoji::Categories::Category the attached property is for.
     *
     * \sa KEmoji::Categories::Category
     */
    Q_PROPERTY(KEmoji::Categories::Category category READ category WRITE setCategory NOTIFY categoryChanged FINAL)

    /*!
     * \brief The name of the \c KEmoji::Categories::Category as a \c QString.
     */
    Q_PROPERTY(QString name READ name NOTIFY categoryChanged FINAL)

    /*!
     * \brief The icon name of the \c KEmoji::Categories::Category as a \c QString.
     */
    Q_PROPERTY(QString iconName READ iconName NOTIFY categoryChanged FINAL)

public:
    explicit CategoryAttached(QObject *parent = nullptr);

    KEmoji::Categories::Category category() const;
    void setCategory(KEmoji::Categories::Category category);
    QString name() const;
    QString iconName() const;

    static CategoryAttached *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    void categoryChanged();

private:
    KEmoji::Categories::Category m_category = KEmoji::Categories::None;
};
