# SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
# SPDX-License-Identifier: BSD-2-Clause

enable_testing()

include(ECMAddTests)

find_package(Qt6 ${QT_MIN_VERSION} NO_MODULE COMPONENTS Test)

add_definitions(-DDATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data" )

ecm_add_test(
    emojitest.cpp
    LINK_LIBRARIES KEmoji Qt::Test KF6::I18n
    TEST_NAME emojitest
)

ecm_add_test(
    dicttest.cpp
    LINK_LIBRARIES KEmoji Qt::Test KF6::I18n
    TEST_NAME dicttest
)

ecm_add_test(
    grouptest.cpp
    LINK_LIBRARIES KEmoji Qt::Test KF6::I18n
    TEST_NAME grouptest
)

ecm_add_test(
    modeltest.cpp
    LINK_LIBRARIES KEmoji Qt::Test KF6::I18n
    TEST_NAME modeltest
)

ecm_add_test(
    sortfiltermodeltest.cpp
    LINK_LIBRARIES KEmoji Qt::Test KF6::I18n
    TEST_NAME sortfiltermodeltest
)

ecm_add_test(
    tonestest.cpp
    LINK_LIBRARIES KEmoji Qt::Test KF6::I18n
    TEST_NAME tonestest
)

ecm_add_test(
    settingstest.cpp
    LINK_LIBRARIES KEmoji Qt::Test KF6::I18n
    TEST_NAME settingstest
)

macro(add_qml_tests)
    if (WIN32)
        set(_extra_args -platform offscreen)
    endif()

    foreach(test ${ARGV})
        add_test(NAME ${test}
                COMMAND qmltest
                ${_extra_args}
                -input ${test}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    endforeach()
endmacro()

add_executable(qmltest qmltest.cpp)
qt_add_qml_module(qmltest URI NeoChatTestUtils)

target_link_libraries(qmltest PRIVATE
    Qt6::Qml
    Qt6::QuickTest
    KF6::I18n
    KEmoji
)

add_qml_tests(
    emojidelegatetest.qml
    emojigridtest.qml
)
