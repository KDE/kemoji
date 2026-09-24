# SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
# SPDX-License-Identifier: BSD-2-Clause

add_definitions(-DTRANSLATION_DOMAIN=\"kemoji\")

add_library(KEmoji)

qt_extract_metatypes(KEmoji)

set_target_properties(KEmoji PROPERTIES
    VERSION     ${KEMOJI_VERSION}
    SOVERSION   ${KEMOJI_SOVERSION}
    EXPORT_NAME KEmoji
)

target_sources(KEmoji PRIVATE
    emoji.cpp
    category.cpp
    dict.cpp
    settings.cpp
    group.cpp
    category.h
    tones.h
    tones.cpp
    model.cpp
    sortfiltermodel.cpp
)

if(KEMOJI_USE_QML)
    add_subdirectory(quick)
endif()

ecm_qt_declare_logging_category(KEmoji
    HEADER kemoji_logging.h
    IDENTIFIER KEMOJI
    CATEGORY_NAME kemoji
    DESCRIPTION "KEmoji"
    EXPORT KEMOJI
)

generate_export_header(KEmoji
    EXPORT_FILE_NAME kemoji_export.h
    BASE_NAME KEmoji
)

ecm_generate_qdoc(KEmoji kemoji.qdocconf)

target_include_directories(KEmoji INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/KEmoji>")

# For the public interface require at least C++20 (e.g. std::ranges used in inline methods)
# The CMake target features use a minimum concept, consumers can still use higher standardds.
target_compile_features(KEmoji INTERFACE cxx_std_20)

target_link_libraries(KEmoji
    PUBLIC
        Qt::Gui
        KF6::I18n
    PRIVATE
        Qt::Concurrent
)

ecm_generate_headers(KEmoji_FORWARDING_HEADERS
  HEADER_NAMES
    Emoji
    Dict
    Group
    Category
    Tones
    Model
    SortFilterModel

  REQUIRED_HEADERS KEmoji_HEADERS
  PREFIX KEmoji
)

install(TARGETS KEmoji EXPORT KEmojiTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/kemoji_export.h
  ${KEmoji_HEADERS}
  DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KEmoji/kemoji COMPONENT Devel
)

install(FILES
  ${KEmoji_FORWARDING_HEADERS}
  DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KEmoji/KEmoji COMPONENT Devel
)

ecm_qt_install_logging_categories(
    EXPORT KEMOJI
    FILE kemoji.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)

if(${ECM_VERSION} VERSION_GREATER_EQUAL 6.30)
    include(ECMInstalledLibraryCheck)

    ecm_add_installed_library_check(KEmoji
        PACKAGE_NAME "KEmoji"
        NO_PACKAGE_TARGET_NAMESPACE
    )

    ecm_installed_library_check_include_strings(KEmoji
        HEADERS ${KEmoji_FORWARDING_HEADERS}
        PREFIX KEmoji
    )
endif()
