/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "emojifiltermodel.h"

#include "category.h"
#include "dict.h"
#include "emojimodel.h"
#include "tones.h"

using namespace Qt::Literals::StringLiterals;
using namespace KEmoji;

EmojiFilterModel::EmojiFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_currentCategory(Categories::Category::All)
{
    sort(0);
    connect(&Dict::instance(), &Dict::favoriteEmojisChanged, this, &EmojiFilterModel::invalidate);
    connect(this, &EmojiFilterModel::sourceModelChanged, this, &EmojiFilterModel::invalidate);
    connect(this, &EmojiFilterModel::sourceModelChanged, this, [this]() {
        if (sourceModel()) {
            connect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &EmojiFilterModel::invalidate);
        }
    });
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

Categories::Category EmojiFilterModel::currentCategory() const
{
    return m_currentCategory;
}

void EmojiFilterModel::setCurrentCategory(Categories::Category category)
{
    if (category == m_currentCategory) {
        return;
    }
    m_currentCategory = category;
    invalidate();
    Q_EMIT categoryChanged();
}

Tones::Tone EmojiFilterModel::defaultTone() const
{
    return m_defaultTone;
}

QString EmojiFilterModel::defaultToneUnicode() const
{
    return Tones::exampleEmojiUnicodeForTone(m_defaultTone);
}

void EmojiFilterModel::setDefaultTone(Tones::Tone defaultTone)
{
    if (defaultTone == m_defaultTone) {
        return;
    }
    m_defaultTone = defaultTone;
    beginFilterChange();
    endFilterChange(QSortFilterProxyModel::Direction::Rows);
    Q_EMIT defaultToneChanged();
}

bool EmojiFilterModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
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

bool EmojiFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    bool categoryFilter = false;
    bool searchFilter = false;
    bool toneFilter = false;

    const auto idx = sourceModel()->index(source_row, 0, source_parent);

    // First if the category is Recent accept based on whether the emoji is a recent emoji, then favorite.
    if (m_currentCategory == Categories::Recent) {
        categoryFilter = idx.data(EmojiModel::RecentIndexRole).toInt() >= 0;
    } else if (m_currentCategory == Categories::Favorite) {
        categoryFilter = idx.data(EmojiModel::TimesUsedRole).toInt() > 0;
    } else {
        categoryFilter = m_currentCategory == Categories::All || idx.data(EmojiModel::EmojiRole).view<Emoji>().category() == m_currentCategory;
    }

    searchFilter = nameContainsSearch(idx);

    const auto emoji = idx.data(EmojiModel::EmojiRole).view<Emoji>();
    const auto tones = Tones::tonesForEmoji(emoji);
    const auto family = Dict::instance().variantGroupForEmoji(emoji);
    toneFilter = tones.length() == 1 && ((family.isEmpty() && tones.contains(Tones::Neutral)) || tones.contains(m_defaultTone));

    return categoryFilter && searchFilter && (toneFilter || m_currentCategory == Categories::Recent || m_currentCategory == Categories::Favorite);
}

bool EmojiFilterModel::nameContainsSearch(const QModelIndex &index) const
{
    const auto emoji = index.data(EmojiModel::EmojiRole).view<Emoji>();

    const auto searchParts = m_searchText.split(u' ');

    int nameMatches = 0;
    int fallbackNameMatches = 0;
    QHash<QString, int> altNameMatches;
    for (const auto &part : searchParts) {
        if (emoji.name().contains(part)) {
            ++nameMatches;
        }
        if (emoji.fallbackName().contains(part)) {
            ++fallbackNameMatches;
        }
        std::ranges::for_each(emoji.altNames(), [&altNameMatches, part](const QString &altName) {
            if (!altNameMatches.contains(altName)) {
                altNameMatches[altName] = 0;
            }
            if (altName.contains(part)) {
                ++altNameMatches[altName];
            }
        });
    }

    return nameMatches == searchParts.size() || fallbackNameMatches == searchParts.size() || std::ranges::any_of(altNameMatches, [searchParts](int matches) {
               return matches == searchParts.size();
           });
}

int EmojiFilterModel::exactNameMatch(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    const auto leftEmoji = source_left.data(EmojiModel::EmojiRole).view<Emoji>();
    const auto leftMatch = leftEmoji.name() == m_searchText || leftEmoji.fallbackName() == m_searchText;
    const auto rightEmoji = source_right.data(EmojiModel::EmojiRole).view<Emoji>();
    const auto rightMatch = rightEmoji.name() == m_searchText || rightEmoji.fallbackName() == m_searchText;
    return leftMatch == rightMatch ? 0 : leftMatch ? 1 : -1;
}

int EmojiFilterModel::exactAltNameMatch(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    const auto leftMatch = source_left.data(EmojiModel::EmojiRole).view<Emoji>().altNames().contains(m_searchText);
    const auto rightMatch = source_right.data(EmojiModel::EmojiRole).view<Emoji>().altNames().contains(m_searchText);
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

bool EmojiFilterModel::sourceIndexLessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    const auto source = dynamic_cast<EmojiModel *>(sourceModel());
    if (!source) {
        return false;
    }
    const auto &group = source->emojis();
    const auto leftEmoji = source_left.data(EmojiModel::EmojiRole).view<Emoji>();
    const auto rightEmoji = source_right.data(EmojiModel::EmojiRole).view<Emoji>();
    return group.indexForEmoji(leftEmoji) < group.indexForEmoji(rightEmoji);
}

#include "moc_emojifiltermodel.cpp"
