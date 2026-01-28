/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "emojifiltermodel.h"

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
    m_searchText = searchText.trimmed();
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
    int result = isRecentMatch(source_left, source_right);
    if (m_currentCategoryId == recentCategoryID && result != 0) {
        return result > 0;
    }

    result = isFavoriteMatch(source_left, source_right);
    if (m_currentCategoryId == favoriteCategoryID && result != 0) {
        return result > 0;
    }

    result = exactNameMatch(source_left, source_right);
    if (result != 0) {
        return result > 0;
    }

    result = exactAltNameMatch(source_left, source_right);
    if (result != 0) {
        return result > 0;
    }

    return false;
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
            || sourceModel()->index(source_row, 0, source_parent).data(EmojiModel::CategoryRole).view<Category>().id() == m_currentCategoryId;
    }

    const auto idx = sourceModel()->index(source_row, 0, source_parent);
    searchFilter = nameContainsSearch(idx) || !idx.data(EmojiModel::AltNamesRole).toStringList().filter(m_searchText, Qt::CaseInsensitive).isEmpty();

    return categoryFilter && searchFilter;
}

bool EmojiFilterModel::nameContainsSearch(const QModelIndex &index) const
{
    const auto name = index.data(EmojiModel::NameRole).toString();
    const auto fallbackName = index.data(EmojiModel::FallbackNameRole).toString();

    const auto searchParts = m_searchText.split(u' ');

    int nameMatches = 0;
    int fallbackNameMatches = 0;
    for (const auto &part : searchParts) {
        if (name.contains(part)) {
            ++nameMatches;
        }
        if (fallbackName.contains(part)) {
            ++fallbackNameMatches;
        }
    }

    return nameMatches == searchParts.size() || fallbackNameMatches == searchParts.size();
}

int EmojiFilterModel::exactNameMatch(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    const auto leftMatch =
        source_left.data(EmojiModel::NameRole).toString() == m_searchText || source_left.data(EmojiModel::FallbackNameRole).toString() == m_searchText;
    const auto rightMatch =
        source_right.data(EmojiModel::NameRole).toString() == m_searchText || source_right.data(EmojiModel::FallbackNameRole).toString() == m_searchText;
    return leftMatch == rightMatch ? 0 : leftMatch ? 1 : -1;
}

int EmojiFilterModel::exactAltNameMatch(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    const auto leftMatch = source_left.data(EmojiModel::AltNamesRole).toStringList().contains(m_searchText);
    const auto rightMatch = source_right.data(EmojiModel::AltNamesRole).toStringList().contains(m_searchText);
    return leftMatch == rightMatch ? 0 : leftMatch ? 1 : -1;
}

int EmojiFilterModel::isRecentMatch(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    const auto leftMatch = source_left.data(EmojiModel::RecentIndexRole).toInt();
    const auto rightMatch = source_right.data(EmojiModel::RecentIndexRole).toInt();
    return leftMatch == rightMatch ? 0 : rightMatch > leftMatch ? 1 : -1;
}

int EmojiFilterModel::isFavoriteMatch(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    const auto leftMatch = source_left.data(EmojiModel::TimesUsedRole).toInt();
    const auto rightMatch = source_right.data(EmojiModel::TimesUsedRole).toInt();
    return leftMatch == rightMatch ? 0 : leftMatch > rightMatch ? 1 : -1;
}

#include "moc_emojifiltermodel.cpp"
