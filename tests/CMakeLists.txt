# UniLume project
# SPDX-License-Identifier: GPL-2.0-or-later

add_executable(unilume_engine_tests
  engine/engine_test.cpp
)

target_link_libraries(unilume_engine_tests PRIVATE unilume_engine)

set_target_properties(unilume_engine_tests PROPERTIES
  CXX_STANDARD 98
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS YES
)

add_test(NAME engine COMMAND unilume_engine_tests)
