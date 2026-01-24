/*
 *    SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "tonemodel.h"

#include "tones.h"

using namespace KEmoji;

ToneModel::ToneModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QVariant ToneModel::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index,
                    QAbstractItemModel::CheckIndexOption::IndexIsValid | QAbstractItemModel::CheckIndexOption::ParentIsInvalid
                        | QAbstractItemModel::CheckIndexOption::DoNotUseParent)
        || index.column() != 0)
        return {};

    const auto tone = static_cast<Tones::Tone>(index.row());
    switch (role) {
    case ExampleUnicodeRole:
        return Tones::exampleEmojiUnicodeForTone(tone);
    case NameRole:
        return Tones::nameStringForTone(tone);
    case ToneRole:
        return tone;
    case CodePointRole:
        return Tones::codePointForTone(tone);
    case UnicodeNameStringRole:
        return Tones::exampleEmojiUnicodeNameStringForTone(tone);
    }
    return {};
}

int ToneModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : KEmoji::Tones::LENGTH;
}

QHash<int, QByteArray> ToneModel::roleNames() const
{
    return {
        {ExampleUnicodeRole, "exampleUnicode"},
        {NameRole, "name"},
        {ToneRole, "tone"},
        {CodePointRole, "codePoint"},
        {UnicodeNameStringRole, "unicodeNameString"},
    };
}

#include "moc_tonemodel.cpp"
