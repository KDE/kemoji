# SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
# SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#!/bin/sh

# Extract strings from all source files.
# EXTRACT_TR_STRINGS extracts strings with lupdate and convert them to .pot with
# lconvert.
$EXTRACT_TR_STRINGS `find . -name \*.cpp -o -name \*.h -o -name \*.ui -o -name \*.qml` -o $podir/kemoji_qt.pot
