# SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
# SPDX-License-Identifier: BSD-2-Clause

add_library(KEmoji)

qt_extract_metatypes(KEmoji)

set_target_properties(KEmoji PROPERTIES
    VERSION     ${KEMOJI_VERSION}
    SOVERSION   ${KEMOJI_SOVERSION}
    EXPORT_NAME Emoji
)

target_sources(KEmoji PRIVATE
    emoji.cpp
    dict.cpp
    settings.cpp
    group.cpp
    category.cpp
    tones.h
    emojimodel.cpp
    emojifiltermodel.cpp
    emojicategorymodel.cpp
    tonemodel.cpp
)

if(KEMOJI_USE_QML)
    add_subdirectory(quick)
endif()

ecm_qt_declare_logging_category(KEmoji
    HEADER kemoji_logging.h
    IDENTIFIER KEMOJI
    CATEGORY_NAME kf.emoji
    DESCRIPTION "KEmoji"
    EXPORT KEMOJI
)

generate_export_header(KEmoji
    EXPORT_FILE_NAME kemoji_export.h
    BASE_NAME KEmoji
)

target_include_directories(KEmoji INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF}/KEmoji>")

target_link_libraries(KEmoji PRIVATE
    Qt::Core
    Qt::Concurrent
    KF6::I18n
)

ecm_generate_headers(KEmoji_HEADERS
  HEADER_NAMES
    Dict
    EmojiModel
    EmojiFilterModel
    EmojiCategoryModel

  REQUIRED_HEADERS KEmoji_HEADERS
)

install(TARGETS KEmoji EXPORT KEmojiTargets ${KF_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/kemoji_export.h
  ${KEmoji_HEADERS}
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/KEmoji COMPONENT Devel
)

ecm_qt_install_logging_categories(
    EXPORT KEMOJI
    FILE kemoji.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)
