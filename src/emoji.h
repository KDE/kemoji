/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QString>
#include <QStringList>
#include <QUrl>
#include <QtQmlIntegration/qqmlintegration.h>

#include "category.h"
#include "kemoji_export.h"

namespace KEmoji
{
/**
 * \class Emoji
 *
 * \brief A class to represent a single emoji.
 *
 * This supports both unicode emojis and custom emojis (as rich <img> tags).
 */
class KEMOJI_EXPORT Emoji
{
    Q_GADGET
    QML_VALUE_TYPE(emoji)

    Q_PROPERTY(QString unicode READ unicode)
    Q_PROPERTY(QString name READ name)

public:
    Emoji() = default;
    Emoji(const QString &unicode);
    Emoji(const QString &unicode,
          const QString &unqualifiedUnicode,
          const QString &name,
          const QStringList &altNames,
          const QString &category,
          const QString &fallbackName = {});
    Emoji(const QUrl &image);
    Emoji(const QUrl &image,
          const QString &unqualifiedUnicode,
          const QString &name,
          const QStringList &altNames,
          const QString &category,
          const QString &fallbackName = {});

    QString unicode() const;
    void setUnicode(const QString &unicode);

    QString unqualifiedUnicode() const;

    bool isValid() const;
    bool isUnicode() const;
    bool isCustom() const;

    QString name() const;
    QString fallbackName() const;
    QStringList altNames() const;

    Category category() const;

    /*!
     * \brief Return a string representation that can be added to a text component.
     *
     * \param textFormat The Qt::TextFormat of the output. Custom emojis are represented
     * by <img> html tags so can only be rendered in text components that support
     * Qt::RichText.
     *
     * \sa Qt::TextFormat
     */
    Q_INVOKABLE QString toString(Qt::TextFormat textFormat = Qt::PlainText) const;

    bool operator==(const Emoji &right) const;
    bool operator==(const QString &right) const;

private:
    QString m_unicode;
    QUrl m_source;

    QString m_unqualifiedUnicode;
    QString m_name;
    QString m_fallbackName;
    QStringList m_altNames;
    QString m_category;
};

using EmojiRef = std::reference_wrapper<Emoji>;

/**
 * \class EmojiGroup
 *
 * \brief A class to represent a group of emojis.
 *
 * Emojis can be grouped for any reason but a common example would be supporting the
 * relationship between an emoji and its tones.
 */
class KEMOJI_EXPORT EmojiGroup
{
public:
    /*!
     * \brief Return a filtered list of the emojis.
     *
     * If no valid filter is set a list containing all emojis is returned.
     */
    QList<Emoji> filtered(std::function<bool(const Emoji &)> filter = {}) const;

    bool isEmpty() const;

    QList<EmojiRef> &operator+=(EmojiRef emojiRef);

private:
    QList<EmojiRef> m_emojiRefs;
};

static EmojiGroup emptyGroup;

struct FavoriteEmoji {
    Emoji emoji;
    int timesUsed;

    bool operator==(const FavoriteEmoji &right) const;
    bool operator==(const Emoji &right) const;
};
}

KEMOJI_EXPORT QDataStream &operator<<(QDataStream &stream, const KEmoji::Emoji &emoji);
KEMOJI_EXPORT QDataStream &operator>>(QDataStream &stream, KEmoji::Emoji &emoji);

KEMOJI_EXPORT QDebug operator<<(QDebug debug, const KEmoji::Emoji &emoji);
