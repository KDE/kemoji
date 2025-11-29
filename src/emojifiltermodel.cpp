/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "emojifiltermodel.h"

#include "emojicategory.h"
#include "emojidict.h"
#include "emojimodel.h"
#include "kemoji_logging.h"

using namespace Qt::Literals::StringLiterals;
using namespace KEmoji;

EmojiFilterModel::EmojiFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_currentCategoryId(allCategoryID)
{
    sort(0);
    connect(&EmojiDict::instance(), &EmojiDict::favoriteEmojisChanged, this, &EmojiFilterModel::invalidate);
}

QString EmojiFilterModel::searchText() const
{
    return m_searchText;
}

void EmojiFilterModel::setSearchText(const QString &searchText)
{
    if (searchText == m_searchText) {
        return;
    }
    m_searchText = searchText;
    invalidate();
    Q_EMIT searchTextChanged();
}

QString EmojiFilterModel::currentCategoryId() const
{
    return m_currentCategoryId;
}

void EmojiFilterModel::setCurrentCategoryId(QString currentCategoryId)
{
    if (!EmojiDict::instance().categories().contains(currentCategoryId)) {
        qCWarning(KEMOJI) << currentCategoryId << "is not an emoji category in the current dictionary";
        currentCategoryId = allCategoryID;
    }

    if (currentCategoryId == m_currentCategoryId) {
        return;
    }

    m_currentCategoryId = currentCategoryId;
    invalidate();
    Q_EMIT categoryChanged();
}

Category EmojiFilterModel::currentCategory() const
{
    return EmojiDict::instance().categories()[EmojiDict::instance().categories().indexOf(m_currentCategoryId)];
}

EmojiCategoryModel *EmojiFilterModel::categoryModel()
{
    if (!m_categoryModel) {
        m_categoryModel = new EmojiCategoryModel(this);
    }

    return m_categoryModel;
}

bool EmojiFilterModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    if (!(m_currentCategoryId == recentCategoryID || m_currentCategoryId == favoriteCategoryID)) {
        return false;
    }

    const auto rightIndex =
        m_currentCategoryId == recentCategoryID ? source_right.data(EmojiModel::RecentIndexRole).toInt() : source_right.data(EmojiModel::TimesUsedRole).toInt();
    const auto leftIndex =
        m_currentCategoryId == recentCategoryID ? source_left.data(EmojiModel::RecentIndexRole).toInt() : source_left.data(EmojiModel::TimesUsedRole).toInt();
    if (rightIndex == -1) {
        return false;
    }
    if (leftIndex == -1) {
        return true;
    }

    return m_currentCategoryId == recentCategoryID ? rightIndex > leftIndex : leftIndex > rightIndex;
}

bool EmojiFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    bool categoryFilter = false;
    bool searchFilter = false;

    // First if the category is Recent accept based on whether the emoji is a recent emoji.
    if (m_currentCategoryId == recentCategoryID) {
        categoryFilter = sourceModel()->index(source_row, 0, source_parent).data(EmojiModel::RecentIndexRole).toInt() >= 0;
    } else if (m_currentCategoryId == favoriteCategoryID) {
        categoryFilter = sourceModel()->index(source_row, 0, source_parent).data(EmojiModel::TimesUsedRole).toInt() > 0;
    } else {
        categoryFilter = m_currentCategoryId == allCategoryID
            || sourceModel()->index(source_row, 0, source_parent).data(EmojiModel::CategoryRole).view<Category>().id == m_currentCategoryId;
    }

    const auto idx = sourceModel()->index(source_row, 0, source_parent);
    searchFilter = idx.data(Qt::ToolTipRole).toString().contains(m_searchText, Qt::CaseInsensitive)
        || idx.data(EmojiModel::FallbackDescriptionRole).toString().contains(m_searchText, Qt::CaseInsensitive)
        || !idx.data(EmojiModel::AnnotationsRole).toStringList().filter(m_searchText, Qt::CaseInsensitive).isEmpty();

    return categoryFilter && searchFilter;
}

#include "moc_emojifiltermodel.cpp"
