# SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
# SPDX-License-Identifier: BSD-2-Clause

file(GLOB EMOJI_DATA_FILES "*.dict")
file(COPY ${EMOJI_DATA_FILES} DESTINATION ${PROJECT_BINARY_DIR}/data)
install(FILES ${EMOJI_DATA_FILES} DESTINATION ${KDE_INSTALL_DATADIR}/kemoji)
