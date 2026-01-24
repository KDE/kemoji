/*
 *    SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QAbstractListModel>
#include <QtQmlIntegration/qqmlintegration.h>

#include "kemoji_export.h"
#include "tones.h"

class KEMOJI_EXPORT EmojiModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(KEmoji::Tones::Tone defaultTone READ defaultTone WRITE setDefaultTone NOTIFY defaultToneChanged)

    Q_PROPERTY(QString defaultToneUnicode READ defaultToneUnicode NOTIFY defaultToneChanged)

public:
    enum RoleNames {
        UnicodeRole = Qt::DisplayRole,
        NameRole = Qt::ToolTipRole,
        CategoryRole = Qt::UserRole + 1,
        AltNamesRole,
        FallbackNameRole,
        RecentIndexRole,
        TimesUsedRole,
        SubEmojisRole,
    };

    explicit EmojiModel(QObject *parent = nullptr);

    KEmoji::Tones::Tone defaultTone() const;
    QString defaultToneUnicode() const;
    void setDefaultTone(KEmoji::Tones::Tone defaultTone);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void defaultToneChanged();

private:
    KEmoji::Tones::Tone m_defaultTone = KEmoji::Tones::Neutral;
};
