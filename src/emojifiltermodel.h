/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QPointer>
#include <QSortFilterProxyModel>
#include <QtQmlIntegration/qqmlintegration.h>

#include "kemoji_export.h"

#include "category.h"
#include "tones.h"

class KEMOJI_EXPORT EmojiFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString searchText READ searchText WRITE setSearchText NOTIFY searchTextChanged)

    Q_PROPERTY(KEmoji::Category currentCategory READ currentCategory WRITE setCurrentCategory NOTIFY categoryChanged)

    Q_PROPERTY(KEmoji::Tones::Tone defaultTone READ defaultTone WRITE setDefaultTone NOTIFY defaultToneChanged)

    Q_PROPERTY(QString defaultToneUnicode READ defaultToneUnicode NOTIFY defaultToneChanged)

public:
    explicit EmojiFilterModel(QObject *parent = nullptr);

    QString searchText() const;
    void setSearchText(const QString &searchText);

    KEmoji::Category currentCategory() const;
    Q_INVOKABLE void setCurrentCategory(const QString &category);
    void setCurrentCategory(const KEmoji::Category &category);

    KEmoji::Tones::Tone defaultTone() const;
    QString defaultToneUnicode() const;
    void setDefaultTone(KEmoji::Tones::Tone defaultTone);

    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

Q_SIGNALS:
    void searchTextChanged();
    void categoryChanged();
    void defaultToneChanged();

private:
    QString m_searchText = {};

    KEmoji::Category m_currentCategory;
    KEmoji::Tones::Tone m_defaultTone = KEmoji::Tones::Neutral;

    bool nameContainsSearch(const QModelIndex &index) const;
    int exactNameMatch(const QModelIndex &source_left, const QModelIndex &source_right) const;
    int exactAltNameMatch(const QModelIndex &source_left, const QModelIndex &source_right) const;
    int isRecentMatch(const QModelIndex &source_left, const QModelIndex &source_right) const;
    int isFavoriteMatch(const QModelIndex &source_left, const QModelIndex &source_right) const;
    bool sourceIndexLessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;
};
