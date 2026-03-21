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
    tonestest.cpp
    LINK_LIBRARIES KEmoji Qt::Test KF6::I18n
    TEST_NAME tonestest
)
