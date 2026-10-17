/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QString>
#include <QStringList>
#include <QUrl>
#include <QtQmlIntegration/qqmlintegration.h>

#include "category.h"
#include "kemoji_export.h"

/*!
 * \namespace KEmoji
 * \inmodule KEmoji
 */
namespace KEmoji
{
class EmojiPrivate;

/*!
 * \class KEmoji::Emoji
 * \inmodule KEmoji
 *
 * \brief A class to represent a single emoji.
 *
 * This supports both unicode emojis and custom emojis (as rich \c <img> tags).
 *
 * A unicode emoji must be exactly 1 emoji (which may be any valid combination of
 * codepoints that specify a single emoji in the unicode spec). Trying to set more
 * than 1 emoji or adding other character will result in the input being rejected
 * and the Emoji being invalid.
 *
 * A custom emoji is specified by a name and URL to a local file (remote files are rejected).
 * The \c Emoji::toString() method will then produce a string with an \c <img> tag with
 * the hRef set to the local URL if the textFormat is set to \c Qt::TextFormat::RichText.
 *
 * \note While this class can have its values modified it is designed to act as a
 * value type from QML so only supports reading in QML.
 */
class KEMOJI_EXPORT Emoji
{
    Q_GADGET
    QML_VALUE_TYPE(emoji)

    /*!
     * \property KEmoji::Emoji::unicode
     * \brief The unicode codepoints of the emoji in a \c QString.
     *
     * The string will be empty for an invalid or custom emoji.
     *
     * \sa source
     */
    Q_PROPERTY(QString unicode READ unicode)

    /*!
     * \property KEmoji::Emoji::source
     * \brief The URL source of the emoji in a \c QUrl.
     *
     * The \c QUrl will be empty for an invalid or unicode emoji.
     *
     * \sa unicode
     */
    Q_PROPERTY(QUrl source READ source)

    /*!
     * \property KEmoji::Emoji::name
     * \brief The name of the \c KEmoji::Emoji.
     *
     * Empty if \c KEmoji::Emoji::isValid() is \c false.
     */
    Q_PROPERTY(QString name READ name)

    /*!
     * \property KEmoji::Emoji::fallbackName
     * \brief The fallback name of the \c KEmoji::Emoji.
     *
     * Empty if \c KEmoji::Emoji::isValid() is \c false.
     */
    Q_PROPERTY(QString fallbackName READ fallbackName)

    /*!
     * \property KEmoji::Emoji::altNames
     * \brief The alternate names of the \c KEmoji::Emoji.
     *
     * Empty if \c KEmoji::Emoji::isValid() is \c false.
     *
     * Typically used when searching for emojis.
     */
    Q_PROPERTY(QStringList altNames READ altNames)

    /*!
     * \property KEmoji::Emoji::category
     * \brief The \c KEmoji::Category of the \c KEmoji::Emoji.
     *
     * Empty if \c KEmoji::Emoji::isValid() is \c false.
     */
    Q_PROPERTY(KEmoji::Categories::Category category READ category)

public:
    Emoji();
    Emoji(const QString &unicodeOrCustomName);
    Emoji(const Emoji &other);
    ~Emoji();

    /*!
     * \brief The ID to reference this \c KEmoji::Emoji by.
     *
     * This will be the unicode string for a unicode emoji and the name string for
     * a custom emoji.
     *
     * This is mostly useful as a key for storing the \c KEmoji::Emoji in a map type
     * container.
     */
    QString id() const;
    QString unicode() const;

    QUrl source() const;
    QString unqualifiedUnicode() const;

    /*!
     * \brief Whether the \c KEmoji::Emoji is valid.
     *
     * A \c KEmoji::Emoji is valid if it contains either exactly 1 unicode emoji
     * or a \c QUrl to a local file.
     */
    bool isValid() const;

    /*!
     * \brief Whether the \c KEmoji::Emoji represents a valid unicode emoji.
     */
    bool isUnicode() const;

    /*!
     * \brief Whether the \c KEmoji::Emoji represents a valid custom emoji.
     */
    bool isCustom() const;

    QString name() const;
    QString fallbackName() const;
    QStringList altNames() const;
    Categories::Category category() const;

    /*!
     * \brief Return a string representation that can be added to a text component.
     *
     * \param textFormat The \c Qt::TextFormat of the output. Custom emojis are represented
     * by \c <img> html tags so can only be rendered in text components that support
     * \c Qt::RichText.
     *
     * \sa Qt::TextFormat
     */
    Q_INVOKABLE QString toString(Qt::TextFormat textFormat = Qt::PlainText) const;

    Emoji &operator=(const Emoji &);
    bool operator==(const Emoji &right) const;
    bool operator==(const QString &right) const;

private:
    QExplicitlySharedDataPointer<EmojiPrivate> d;
};

struct FavoriteEmoji {
    Emoji emoji;
    int timesUsed;

    bool operator==(const FavoriteEmoji &right) const;
    bool operator==(const Emoji &right) const;
};
}

KEMOJI_EXPORT QDebug operator<<(QDebug debug, const KEmoji::Emoji &emoji);
