# SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16)

project(KEmoji VERSION 0.1)

set(QT_MIN_VERSION 6.7.0)
set(KF_MIN_VERSION 6.17.0)

set(QT_QML_GENERATE_QMLLS_INI ON)

# ECM setup
include(FeatureSummary)
find_package(ECM ${KF_MIN_VERSION} NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://commits.kde.org/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

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
include(ECMFindQmlModule)
include(ECMGenerateQDoc)
include(CMakeDependentOption)
include(CMakePackageConfigHelpers)
include(KDEClangFormat)

set(kemoji_version_header "${CMAKE_CURRENT_BINARY_DIR}/src/kemoji_version.h")
ecm_setup_version(PROJECT
    VARIABLE_PREFIX KEMOJI
    VERSION_HEADER "${kemoji_version_header}"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KEmojiConfigVersion.cmake"
    SOVERSION 1
)

#
# Dependencies
#
option(KEMOJI_USE_QML "Build components using Qt6Qml" ON)
option(KEMOJI_BUILD_EMOJIER "Build components using Qt6Qml" ON)

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core Concurrent)

find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS I18n)

if(KEMOJI_USE_QML OR KEMOJI_BUILD_EMOJIER)
    include(ECMQmlModule)
    find_package(Qt6 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Quick)
    find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS Kirigami)
endif()

ecm_find_qmlmodule(org.kde.kirigami REQUIRED)

#
# Actually build the stuff
#
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KEmoji")

add_subdirectory(data)
add_subdirectory(src)

if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()

if(KEMOJI_BUILD_EMOJIER)
    find_package(Qt6 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Gui QuickControls2 Widgets)
    find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS Config CoreAddons WindowSystem)

    if(NOT ANDROID AND NOT WIN32)
        find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS DBusAddons Crash)
    endif()

    add_subdirectory(emojier)
endif()

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

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES src/*.cpp src/*.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
