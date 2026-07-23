# UniLume project
# SPDX-License-Identifier: GPL-2.0-or-later

include(GNUInstallDirs)

add_library(unilume_fcitx5 MODULE
  addon.cpp
  fcitx_key_mapper.cpp
  fcitx_replacement_backend.cpp
  input_context_state.cpp
)

target_link_libraries(unilume_fcitx5 PRIVATE
  PkgConfig::FCITX5_CORE
  unilume_core
)

target_include_directories(unilume_fcitx5 PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(unilume_fcitx5 PROPERTIES
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
  PREFIX ""
  OUTPUT_NAME "unilume"
)

configure_file(
  unilume-addon.conf.in
  ${CMAKE_CURRENT_BINARY_DIR}/unilume-addon.conf
  @ONLY
)

install(TARGETS unilume_fcitx5
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/fcitx5
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/unilume-addon.conf
  DESTINATION ${CMAKE_INSTALL_DATADIR}/fcitx5/addon
  RENAME unilume.conf
)
install(FILES unilume-inputmethod.conf
  DESTINATION ${CMAKE_INSTALL_DATADIR}/fcitx5/inputmethod
  RENAME unilume.conf
)
