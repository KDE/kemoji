cmake_minimum_required(VERSION 3.16)

set(KF_VERSION "6.18.0") # handled by release scripts
set(KF_DEP_VERSION "6.17.0") # handled by release scripts

project(KEmoji VERSION ${KF_VERSION})

# ECM setup
include(FeatureSummary)
find_package(ECM 6.17.0 NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://commits.kde.org/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)

include(KDEGitCommitHooks)
include(GenerateExportHeader)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMQtDeclareLoggingCategory)
include(ECMPoQmTools)
include(ECMGenerateExportHeader)
include(ECMDeprecationSettings)
include(ECMQmlModule)
include(ECMFindQmlModule)
include(ECMGenerateQDoc)
include(CMakeDependentOption)
include(CMakePackageConfigHelpers)

set(EXCLUDE_DEPRECATED_BEFORE_AND_AT 0 CACHE STRING "Control the range of deprecated API excluded from the build [default=0].")

set(kemoji_version_header "${CMAKE_CURRENT_BINARY_DIR}/src/kemoji_version.h")
ecm_setup_version(PROJECT
    VARIABLE_PREFIX KEMOJI
    VERSION_HEADER "${kemoji_version_header}"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KEmojiConfigVersion.cmake"
    SOVERSION 6
)

#
# Dependencies
#
set(REQUIRED_QT_VERSION 6.7.0)
set(CMAKE_AUTORCC TRUE)

find_package(Qt6 ${REQUIRED_QT_VERSION} CONFIG REQUIRED COMPONENTS Core Quick Gui QuickControls2 Widgets)

find_package(KF6 ${KF_DEP_VERSION} REQUIRED COMPONENTS Config I18n CoreAddons Crash DBusAddons Kirigami QQC2DesktopStyle WindowSystem IconThemes)

ecm_find_qmlmodule(org.kde.kirigami REQUIRED)

#
# Actually build the stuff
#
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KEmoji")

add_subdirectory(data)
add_subdirectory(src)
add_subdirectory(emojier)

#
# CMake package config file generation
#

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KEmojiConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KEmojiConfig.cmake"
    INSTALL_DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/KEmojiConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/KEmojiConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel)

install(EXPORT KEmojiTargets
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        FILE KEmojiTargets.cmake
        NAMESPACE KF6::)

install(FILES ${kemoji_version_header}
        DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/KEmoji
        COMPONENT Devel)

include(ECMFeatureSummary)
ecm_feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
