/*
 *    SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QFont>
#include <QRawFont>
#include <QString>
#include <QTextBoundaryFinder>
#include <QTextLayout>

using namespace Qt::Literals::StringLiterals;

namespace KEmoji::KUnicode
{
inline QString qualify(QString unicode)
{
    QTextBoundaryFinder finder(QTextBoundaryFinder::Grapheme, unicode);
    int from = 0;
    while (finder.toNextBoundary() != -1) {
        auto to = finder.position();
        const auto currentChar = unicode.mid(from, to - from);
        QString nextChar;
        const auto next = finder.toNextBoundary();
        if (next != -1) {
            nextChar = unicode.mid(to, next - to);
            finder.toPreviousBoundary();
        }
        if (currentChar != u"\u200D"_s && currentChar != u"\uFE0F"_s && nextChar != u"\uFE0F"_s) {
            unicode.replace(from, to - from, u"%1\uFE0F"_s.arg(currentChar));
            to += u"\uFE0F"_s.length();
        }
        from = to;
    }
    return unicode;
}

inline bool unicodeSupportedByFont(const QString &unicode, const QFont &font)
{
    // Check if the unicode produces a single glyph when rendered. This shows that
    // the font supports any variation sequences.
    QTextLayout textLayout(unicode);
    textLayout.setFont(font);
    textLayout.beginLayout();
    QTextLine line = textLayout.createLine();
    textLayout.endLayout();
    if (!line.isValid()) {
        return false;
    }
    if (line.glyphRuns().at(0).glyphIndexes().size() > 1) {
        return false;
    }

    // Check that all character are supported. This is to handle a font not supporting
    // new code point gracefully.
    const auto rawFont = QRawFont::fromFont(font);
    QTextBoundaryFinder finder(QTextBoundaryFinder::Grapheme, unicode);
    int from = 0;
    while (finder.toNextBoundary() != -1) {
        auto to = finder.position();

        auto character = unicode.mid(from, to - from).toUcs4()[0];
        if (!rawFont.supportsCharacter(character)) {
            return false;
        }
        from = to;
    }
    return true;
}
}
