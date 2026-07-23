# UniLume project
# SPDX-License-Identifier: GPL-2.0-or-later

add_executable(unilume_engine_tests
  engine/editing_tests.cpp
  engine/engine_test_main.cpp
  engine/input_tests.cpp
  engine/telex_tests.cpp
  engine/test_support/engine_fixture.cpp
  engine/test_support/test_context.cpp
  engine/unicode_invalid_input_tests.cpp
  engine/unicode_passthrough_tests.cpp
  engine/url_and_code_input_tests.cpp
  engine/vni_tests.cpp
  engine/viqr_tests.cpp
)

target_link_libraries(unilume_engine_tests PRIVATE
  unilume_engine
  unilume_sanitizer_options
)

target_include_directories(unilume_engine_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/engine
)

set_target_properties(unilume_engine_tests PROPERTIES
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
)

add_test(NAME engine COMMAND unilume_engine_tests)
