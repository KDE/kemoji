add_library(KEmoji)

qt_extract_metatypes(KEmoji)

set_target_properties(KEmoji PROPERTIES
    VERSION     ${KEMOJI_VERSION}
    SOVERSION   ${KEMOJI_SOVERSION}
    EXPORT_NAME Emoji
)

target_sources(KEmoji PRIVATE
    emoji.cpp
    emojidict.cpp
    emojicategory.h
    emojimodel.cpp
    emojifiltermodel.cpp
    emojicategorymodel.cpp
)

add_subdirectory(quick)

ecm_qt_declare_logging_category(KEmoji
    HEADER kemoji_logging.h
    IDENTIFIER KEMOJI
    CATEGORY_NAME kf.emoji
    DESCRIPTION "KEmoji"
    EXPORT KEMOJI
)

ecm_generate_export_header(KEmoji
    EXPORT_FILE_NAME kemoji_export.h
    BASE_NAME KEmoji
    GROUP_BASE_NAME KF
    VERSION ${KF_VERSION}
    USE_VERSION_HEADER
    DEPRECATED_BASE_VERSION 0
    DEPRECATION_VERSIONS
    EXCLUDE_DEPRECATED_BEFORE_AND_AT ${EXCLUDE_DEPRECATED_BEFORE_AND_AT}
)

target_include_directories(KEmoji INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF}/KEmoji>")

target_link_libraries(KEmoji PRIVATE
    Qt::Qml
    Qt::Gui
    KF6::ConfigGui
    KF6::I18n
)

ecm_generate_headers(KEmoji_HEADERS
  HEADER_NAMES
    EmojiDict
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
