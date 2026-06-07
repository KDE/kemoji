# SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
# SPDX-License-Identifier: BSD-2-Clause

ecm_add_qml_module(KEmojiPlugin URI org.kde.kemoji GENERATE_PLUGIN_SOURCE
    QML_FILES
        EmojiDelegate.qml
        EmojiGrid.qml
        SubEmojiPopup.qml
    SOURCES
        quickdict.h
        categoryattached.cpp
        toneattached.cpp
)

target_link_libraries(KEmojiPlugin PRIVATE
    Qt::Quick
    KF6::I18n
    KEmoji
)

qt_generate_foreign_qml_types(KEmoji KEmojiPlugin)

ecm_finalize_qml_module(KEmojiPlugin DESTINATION ${KDE_INSTALL_QMLDIR})
