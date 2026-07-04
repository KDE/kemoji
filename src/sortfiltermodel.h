/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QFont>
#include <QPointer>
#include <QSortFilterProxyModel>
#include <QtQmlIntegration/qqmlintegration.h>

#include "kemoji_export.h"

#include "category.h"
#include "tones.h"

namespace KEmoji
{
/*!
 * \class SortFilterModel
 *
 * \brief A proxy model to sort and filter a \c KEmoji::Model.
 *
 * The source \c KEmoji::Model can be sorted and filtered by category and by search.
 *
 * Setting \c ::currentCategory to the desired \c KEmoji::Categories::Category value
 * will filter out all \c KEmoji::Emoji not in that \c KEmoji::Categories::Category.
 *
 * The \c KEmoji::Emoji can be filtered by \c KEmoji::Tones::Tone. This only applies
 * to \c KEmoji::Emoji that have tone variants. If a \c KEmoji::Emoji does not have
 * \c KEmoji::Tones::Tone variants it is shown.
 *
 * Setting \c ::searchText will filter out any items that do not match the search
 * and will sort by how closely the \c KEmoji::Emoji matches the search text.
 *
 * Currently search results are sorted in the current priority order (with 1 being the
 * highest priority):
 * \list 1
 *      \li Exact Name Match
 *      \li Exact Alternate Name Match
 *      \li Order as per Unicode Spec
 * \endlist
 *
 * Category and search text based filtering can be applied simultaneously.
 *
 * \sa KEmoji::Model, KEmoji::Emoji, KEmoji::Categories::Category
 */
class KEMOJI_EXPORT SortFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ELEMENT

    /*!
     * \brief The search text to filter and sort by.
     */
    Q_PROPERTY(QString searchText READ searchText WRITE setSearchText NOTIFY searchTextChanged)

    /*!
     * \brief The \c KEmoji::Categories::Category to filter by.
     *
     * \sa KEmoji::Categories::Category
     */
    Q_PROPERTY(KEmoji::Categories::Category currentCategory READ currentCategory WRITE setCurrentCategory NOTIFY categoryChanged)

    /*!
     * \brief The \c KEmoji::Tones::Tone to filter by.
     *
     * This only applies to \c KEmoji::Emoji that have tone variants. If a
     * \c KEmoji::Emoji does not have \c KEmoji::Tones::Tone variants it is shown.
     *
     * \sa KEmoji::Tones::Tone
     */
    Q_PROPERTY(KEmoji::Tones::Tone toneFilter READ toneFilter WRITE setToneFilter NOTIFY toneFilterChanged)

    /*!
     * \brief Whether unicode sequences not supported by the current font should be shown.
     *
     * As new unicode code points are added not all fonts may support them and may be
     * missing glyphs, this allows for them not to be shown.
     */
    Q_PROPERTY(bool showUnsupportedEmojis READ showUnsupportedEmojis WRITE setShowUnsupportedEmojis NOTIFY showUnsupportedEmojisChanged)

    /*!
     * \brief The current font being used to visualize \c KEmoji::Emoji.
     *
     * This is used to check if the font has a glyph for an \c KEmoji::Emoji.
     */
    Q_PROPERTY(QFont currentFont READ currentFont WRITE setCurrentFont NOTIFY currentFontChanged)

public:
    explicit SortFilterModel(QObject *parent = nullptr);

    QString searchText() const;
    void setSearchText(const QString &searchText);

    Categories::Category currentCategory() const;
    void setCurrentCategory(Categories::Category category);

    Tones::Tone toneFilter() const;
    void setToneFilter(Tones::Tone toneFilter);

    bool showUnsupportedEmojis() const;
    void setShowUnsupportedEmojis(bool showUnsupportedEmojis);

    QFont currentFont() const;
    void setCurrentFont(const QFont &currentFont);

Q_SIGNALS:
    void searchTextChanged();
    void categoryChanged();
    void toneFilterChanged();
    void showUnsupportedEmojisChanged();
    void currentFontChanged();

private:
    QString m_searchText;
    std::vector<QStringView> m_searchTextParts;

    Categories::Category m_currentCategory;
    Tones::Tone m_toneFilter = Tones::Neutral;
    bool m_showUnsupportedEmojis = true;
    QFont m_currentFont = {};

    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

    [[nodiscard]] bool nameContainsSearch(const Emoji &emoji) const;
    [[nodiscard]] int exactNameMatch(const Emoji &leftEmoji, const Emoji &rightEmoji) const;
    [[nodiscard]] int exactAltNameMatch(const Emoji &leftEmoji, const Emoji &rightEmoji) const;
    int isRecentMatch(const QModelIndex &source_left, const QModelIndex &source_right) const;
    int isFavoriteMatch(const QModelIndex &source_left, const QModelIndex &source_right) const;
    [[nodiscard]] bool sourceIndexLessThan(const Emoji &leftEmoji, const Emoji &rightEmoji) const;
};
}
