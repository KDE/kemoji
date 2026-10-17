/*
    SPDX-FileCopyrightText: 2022 Weng Xuetian <wegnxt@gmail.com>
    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dict.h"

#include "category.h"
#include "emoji.h"
#include "emojidata_p.h"
#include "group.h"
#include "tones.h"
#include "unicodeUtils_p.h"

using namespace Qt::Literals::StringLiterals;
using namespace KEmoji;

Dict::Dict(QObject *parent)
    : QObject(parent)
    , m_categoryModelAdapter(CategoryAdapter(&m_categories))
{
    m_categoryGroups[Categories::All] = Group();

    m_categoryModelAdapter.insertRow(m_categoryModelAdapter.rowCount(), Categories::Recent);
    m_categoryModelAdapter.insertRow(m_categoryModelAdapter.rowCount(), Categories::Favorite);
    m_categoryModelAdapter.insertRow(m_categoryModelAdapter.rowCount(), Categories::All);

    connect(&EmojiData::instance(), &EmojiData::loadingChanged, this, &Dict::loadingChanged);
    connect(&EmojiData::instance(), &EmojiData::emojisAdded, this, &Dict::addEmojisToCategories);
    connect(&EmojiData::instance(), &EmojiData::emojiHistoryChanged, this, &Dict::emojiHistoryChanged);
}

Dict &Dict::instance()
{
    static Dict _instance;
    return _instance;
}

bool Dict::loading() const
{
    return EmojiData::instance().loading();
}

void Dict::addEmojisToCategories(QStringList emojis)
{
    QList<Categories::Category> updatedCategories;

    std::ranges::for_each(emojis, [this, &updatedCategories](const QString &unicodeOrCustomName) {
        const auto emoji = Emoji(unicodeOrCustomName);

        if (!m_categoryGroups[Categories::All].contains(emoji)) {
            m_categoryGroups[Categories::All].add(emoji);
            if (!updatedCategories.contains(Categories::All)) {
                updatedCategories += Categories::All;
            }
        }

        if (!m_categories.contains(emoji.category())) {
            m_categoryModelAdapter.insertRow(m_categoryModelAdapter.rowCount(), emoji.category());
            Q_EMIT categoriesChanged();
        }
        if (!m_categoryGroups.contains(emoji.category())) {
            m_categoryGroups[emoji.category()] = Group();
        }
        m_categoryGroups[emoji.category()].add(emoji);
        if (!updatedCategories.contains(emoji.category())) {
            updatedCategories += emoji.category();
        }

        const auto tonelessEmoji = Tones::removeTonesFromEmoji(emoji);
        if (!m_categoryGroups[Categories::All].contains(tonelessEmoji)) {
            return;
        }
        if (tonelessEmoji == emoji) {
            return;
        }
        if (m_variantGroups.contains(tonelessEmoji.id())) {
            auto &group = m_variantGroups[tonelessEmoji.id()];
            group.add(emoji);
        } else {
            Group group;
            group.add(tonelessEmoji);
            group.add(emoji);
            m_variantGroups[tonelessEmoji.id()] = group;
        }
    });

    Q_EMIT emojisChanged(updatedCategories);
}

const Group Dict::emojis() const
{
    return m_categoryGroups.at(Categories::All);
}

const Group Dict::variantGroupForEmoji(const Emoji &emoji) const
{
    if (!m_variantGroups.contains(emoji.id())) {
        return emptyGroup;
    }
    return m_variantGroups.at(emoji.id());
}

bool Dict::emojiSupportedByFont(const Emoji &emoji, const QFont &font) const
{
    return KUnicode::unicodeSupportedByFont(emoji.unicode(), font);
}

const QList<Categories::Category> &Dict::categories() const
{
    return m_categories;
}

QRangeModel *Dict::categoryModel() const
{
    return m_categoryModelAdapter.model();
}

const Group Dict::categoryGroup(Categories::Category category) const
{
    if (!m_categoryGroups.contains(category)) {
        return emptyGroup;
    }
    return m_categoryGroups.at(category);
}

bool Dict::isRecentEmoji(const QString &emoji) const
{
    return EmojiData::instance().isRecent(emoji);
}

int Dict::recentEmojiIndex(const KEmoji::Emoji &emoji) const
{
    return EmojiData::instance().recentIndex(emoji.id());
}

int Dict::timesEmojiUsed(const KEmoji::Emoji &emoji) const
{
    return EmojiData::instance().timesUsed(emoji.id());
}

void Dict::emojiUsed(const Emoji &emoji)
{
    if (!m_categoryGroups[Categories::All].contains(emoji)) {
        return;
    }

    EmojiData::instance().emojiUsed(emoji.id());
}

bool Dict::registerCustomEmoji(const QUrl &source, const QString &name)
{
    return EmojiData::instance().registerCustomEmoji(source, name);
}

bool Dict::unregisterCustomEmoji(const QString &name)
{
    const auto unregistered = EmojiData::instance().unregisterCustomEmoji(name);
    if (!unregistered) {
        return false;
    }
    m_categoryGroups[Categories::All].remove(name);
    m_categoryGroups[Categories::Custom].remove(name);
    if (m_categoryGroups[Categories::Custom].isEmpty() && m_categories.contains(Categories::Custom)) {
        m_categoryModelAdapter.removeRow(m_categories.indexOf(Categories::Custom));
        Q_EMIT categoriesChanged();
    }
    Q_EMIT emojisChanged({Categories::All, Categories::Custom});
    return true;
}

void Dict::clearHistory()
{
    EmojiData::instance().clearHistory();
}

#include "moc_dict.cpp"
