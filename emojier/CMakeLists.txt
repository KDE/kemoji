# SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
# SPDX-License-Identifier: BSD-2-Clause

add_executable(emojier emojier.cpp)

ecm_add_qml_module(emojier GENERATE_PLUGIN_SOURCE
    URI org.kde.emojier
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/org/kde/emojier
    QML_FILES
        ui/Emojier.qml
        ui/CategoryPage.qml
    SOURCES
        copyhelper.cpp
)

target_link_libraries(emojier PRIVATE
    Qt::Quick
    Qt::Qml
    Qt::Gui
    Qt::QuickControls2
    Qt::Widgets
    KF6::ConfigGui
    KF6::I18n
    KF6::CoreAddons
    KF6::Kirigami
    KF6::WindowSystem
    KEmoji
)

if(NOT ANDROID AND NOT WIN32)
    target_link_libraries(emojier PRIVATE KF6::DBusAddons KF6::Crash)
    target_compile_definitions(emojier PUBLIC -DHAVE_KDBUSADDONS)
    target_compile_definitions(emojier PUBLIC -DHAVE_KCRASH)
endif()

target_include_directories(emojier PRIVATE ${CMAKE_BINARY_DIR})

install(TARGETS emojier ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS org.kde.emojier.desktop DESTINATION ${KDE_INSTALL_DATADIR}/kglobalaccel)
install(PROGRAMS org.kde.emojier.desktop DESTINATION ${KDE_INSTALL_APPDIR})
