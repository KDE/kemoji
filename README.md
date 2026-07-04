<!--
    SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
    SPDX-License-Identifier: CC0-1.0
-->

# KEmoji

Library to provide applications with standardized emoji support.

## Structure

The core of KEmoji is a dictionary that contains all emojis defined in the [Unicode
standard](https://www.unicode.org/Public/emoji/) (currently version 16).

The dictionary uses an Emoji object which also contains all the other relevant data
e.g. name, alternative names, category, etc. Which is populated from the included
dictionaries.

The library also provides other support to help integrate emojis into applications:
- Groups
- Models
- QML Components


