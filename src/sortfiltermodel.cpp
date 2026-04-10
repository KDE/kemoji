/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "sortfiltermodel.h"

#include "category.h"
#include "dict.h"
#include "model.h"
#include "tones.h"

using namespace Qt::Literals::StringLiterals;
using namespace KEmoji;

SortFilterModel::SortFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_currentCategory(Categories::Category::All)
{
    sort(0);
    connect(&Dict::instance(), &Dict::emojiHistoryChanged, this, [this]() {
        if (m_currentCategory == Categories::Favorite || m_currentCategory == Categories::Recent) {
            invalidate();
            return;
        }
#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
        beginFilterChange();
        endFilterChange(QSortFilterProxyModel::Direction::Rows);
#else
        invalidateRowsFilter();
#endif
    });
    connect(this, &SortFilterModel::sourceModelChanged, this, &SortFilterModel::invalidate);
    connect(this, &SortFilterModel::sourceModelChanged, this, [this]() {
        if (sourceModel()) {
            connect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &SortFilterModel::invalidate);
        }
    });
}

QString SortFilterModel::searchText() const
{
    return m_searchText;
}

void SortFilterModel::setSearchText(const QString &searchText)
{
    if (searchText == m_searchText) {
        return;
    }
    m_searchText = searchText.trimmed();
    invalidate();
    Q_EMIT searchTextChanged();
}

Categories::Category SortFilterModel::currentCategory() const
{
    return m_currentCategory;
}

void SortFilterModel::setCurrentCategory(Categories::Category category)
{
    if (category == m_currentCategory) {
        return;
    }
    m_currentCategory = category;
    invalidate();
    Q_EMIT categoryChanged();
}

Tones::Tone SortFilterModel::toneFilter() const
{
    return m_toneFilter;
}

QString SortFilterModel::toneFilterUnicode() const
{
    return Tones::exampleEmojiUnicodeForTone(m_toneFilter);
}

void SortFilterModel::setToneFilter(Tones::Tone toneFilter)
{
    if (toneFilter == m_toneFilter) {
        return;
    }
    m_toneFilter = toneFilter;
#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
    beginFilterChange();
    endFilterChange(QSortFilterProxyModel::Direction::Rows);
#else
    invalidateRowsFilter();
#endif
    Q_EMIT toneFilterChanged();
}

bool SortFilterModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    int result = isRecentMatch(source_left, source_right);
    if (m_currentCategory == Categories::Recent && result != 0) {
        return result > 0;
    }

    result = isFavoriteMatch(source_left, source_right);
    if (m_currentCategory == Categories::Favorite && result != 0) {
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

    return sourceIndexLessThan(source_left, source_right);
}

bool SortFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    bool categoryFilter = false;
    bool searchFilter = false;
    bool toneFilter = false;

    const auto idx = sourceModel()->index(source_row, 0, source_parent);

    // First if the category is Recent accept based on whether the emoji is a recent emoji, then favorite.
    if (m_currentCategory == Categories::Recent) {
        categoryFilter = idx.data(Model::RecentIndexRole).toInt() >= 0;
    } else if (m_currentCategory == Categories::Favorite) {
        categoryFilter = idx.data(Model::TimesUsedRole).toInt() > 0;
    } else {
        categoryFilter = m_currentCategory == Categories::All || idx.data(Model::EmojiRole).view<Emoji>().category() == m_currentCategory;
    }

    searchFilter = nameContainsSearch(idx);

    const auto emoji = idx.data(Model::EmojiRole).view<Emoji>();
    const auto tones = Tones::tonesForEmoji(emoji);
    const auto family = Dict::instance().variantGroupForEmoji(emoji);
    toneFilter = tones.length() == 1 && ((family.isEmpty() && tones.contains(Tones::Neutral)) || tones.contains(m_toneFilter));

    return categoryFilter && searchFilter && (toneFilter || m_currentCategory == Categories::Recent || m_currentCategory == Categories::Favorite);
}

bool SortFilterModel::nameContainsSearch(const QModelIndex &index) const
{
    const auto emoji = index.data(Model::EmojiRole).view<Emoji>();

    int nameMatches = 0;
    int fallbackNameMatches = 0;
    QHash<QString, int> altNameMatches;
    int partsCount = 0;
    for (const auto part : QStringTokenizer(m_searchText, ' '_L1)) {
        ++partsCount;
        if (emoji.name().contains(part, Qt::CaseInsensitive)) {
            ++nameMatches;
        }
        if (emoji.fallbackName().contains(part, Qt::CaseInsensitive)) {
            ++fallbackNameMatches;
        }
        std::ranges::for_each(emoji.altNames(), [&altNameMatches, part](const QString &altName) {
            if (!altNameMatches.contains(altName)) {
                altNameMatches[altName] = 0;
            }
            if (altName.contains(part, Qt::CaseInsensitive)) {
                ++altNameMatches[altName];
            }
        });
    }

    return nameMatches == partsCount || fallbackNameMatches == partsCount || std::ranges::any_of(altNameMatches, [partsCount](int matches) {
               return matches == partsCount;
           });
}

int SortFilterModel::exactNameMatch(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    const auto leftEmoji = source_left.data(Model::EmojiRole).view<Emoji>();
    const auto leftMatch = leftEmoji.name().compare(m_searchText, Qt::CaseInsensitive) || leftEmoji.fallbackName().compare(m_searchText, Qt::CaseInsensitive);
    const auto rightEmoji = source_right.data(Model::EmojiRole).view<Emoji>();
    const auto rightMatch =
        rightEmoji.name().compare(m_searchText, Qt::CaseInsensitive) || rightEmoji.fallbackName().compare(m_searchText, Qt::CaseInsensitive);
    return leftMatch == rightMatch ? 0 : leftMatch ? 1 : -1;
}

int SortFilterModel::exactAltNameMatch(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    const auto leftMatch = source_left.data(Model::EmojiRole).view<Emoji>().altNames().contains(m_searchText, Qt::CaseInsensitive);
    const auto rightMatch = source_right.data(Model::EmojiRole).view<Emoji>().altNames().contains(m_searchText, Qt::CaseInsensitive);
    return leftMatch == rightMatch ? 0 : leftMatch ? 1 : -1;
}

int SortFilterModel::isRecentMatch(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    const auto leftMatch = source_left.data(Model::RecentIndexRole).toInt();
    const auto rightMatch = source_right.data(Model::RecentIndexRole).toInt();
    return leftMatch == rightMatch ? 0 : rightMatch > leftMatch ? 1 : -1;
}

int SortFilterModel::isFavoriteMatch(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    const auto leftMatch = source_left.data(Model::TimesUsedRole).toInt();
    const auto rightMatch = source_right.data(Model::TimesUsedRole).toInt();
    return leftMatch == rightMatch ? 0 : leftMatch > rightMatch ? 1 : -1;
}

bool SortFilterModel::sourceIndexLessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    const auto source = dynamic_cast<Model *>(sourceModel());
    if (!source) {
        return false;
    }
    const auto &group = source->emojis();
    const auto leftEmoji = source_left.data(Model::EmojiRole).view<Emoji>();
    const auto rightEmoji = source_right.data(Model::EmojiRole).view<Emoji>();
    return group.indexForEmoji(leftEmoji) < group.indexForEmoji(rightEmoji);
}

#include "moc_sortfiltermodel.cpp"
