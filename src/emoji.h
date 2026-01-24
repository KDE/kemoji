/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QString>
#include <QStringList>

#include "emojicategory.h"
#include "kemoji_export.h"
#include "tones.h"

namespace KEmoji
{
class KEMOJI_EXPORT Emoji
{
    Q_GADGET
    QML_ELEMENT

    Q_PROPERTY(QString unicode READ unicode CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)

public:
    explicit Emoji() = default;
    explicit Emoji(const QString &unicode,
                   const QString &unqualifiedUnicode,
                   const QString &name,
                   const QStringList &altNames,
                   const QString &category,
                   const QString &fallbackName = {});

    QString unicode(Tones::Tone defaultTone = Tones::Neutral) const;
    QString unqualifiedUnicode() const;
    QString baseUnicode() const;

    QString name() const;
    QString fallbackName() const;
    QStringList altNames() const;

    QList<Emoji> subEmojis(Tones::Tone toneFilter = Tones::Neutral) const;
    void addSubEmoji(const Emoji &emoji);
    bool isSubEmoji() const;

    Category category() const;

    bool operator==(const Emoji &right) const;
    bool operator==(const QString &right) const;

private:
    QString m_unicode;
    QString m_unqualifiedUnicode;
    QString m_name;
    QString m_fallbackName;
    QStringList m_altNames;
    QList<Emoji> m_subEmojis;
    QString m_category;
};
}
