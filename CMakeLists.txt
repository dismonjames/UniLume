# UniLume project
# SPDX-License-Identifier: GPL-2.0-or-later

cmake_minimum_required(VERSION 3.16)

project(
  UniLume
  VERSION 0.1.0
  DESCRIPTION "Bộ gõ tiếng Việt hiện đại, nhẹ và nhanh cho Linux, phát triển trực tiếp từ lõi UniKey."
  LANGUAGES CXX
)

include(CTest)

option(UNILUME_ENABLE_ASAN "Enable AddressSanitizer for UniLume targets" OFF)
option(UNILUME_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer for UniLume targets" OFF)

add_library(unilume_sanitizer_options INTERFACE)

if(UNILUME_ENABLE_ASAN OR UNILUME_ENABLE_UBSAN)
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(FATAL_ERROR
      "UniLume sanitizers require a GCC- or Clang-compatible compiler"
    )
  endif()

  set(_unilume_sanitizers)
  if(UNILUME_ENABLE_ASAN)
    list(APPEND _unilume_sanitizers address)
  endif()
  if(UNILUME_ENABLE_UBSAN)
    list(APPEND _unilume_sanitizers undefined)
  endif()
  list(JOIN _unilume_sanitizers "," _unilume_sanitizer_list)

  target_compile_options(unilume_sanitizer_options INTERFACE
    "-fsanitize=${_unilume_sanitizer_list}"
    -fno-omit-frame-pointer
  )
  target_link_options(unilume_sanitizer_options INTERFACE
    "-fsanitize=${_unilume_sanitizer_list}"
  )
endif()

add_library(unilume_engine STATIC
  src/byteio/byteio.cpp
  src/vnconv/charset.cpp
  src/vnconv/convert.cpp
  src/vnconv/data.cpp
  src/vnconv/error.cpp
  src/vnconv/pattern.cpp
  src/ukengine/inputproc.cpp
  src/ukengine/mactab.cpp
  src/ukengine/ukengine.cpp
  src/ukengine/usrkeymap.cpp
  src/ukinterface/unikey.cpp
)

target_link_libraries(unilume_engine PRIVATE unilume_sanitizer_options)

target_include_directories(unilume_engine
  PUBLIC
    src/ukinterface
    src/ukengine
  PRIVATE
    src/byteio
    src/vnconv
)

# The inherited tables use 8-bit character constants and predate C++11.
# Keeping the original language dialect avoids changing their data or behavior.
set_target_properties(unilume_engine PROPERTIES
  CXX_STANDARD 98
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS YES
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(unilume_engine PRIVATE
    -Wall
    -Wextra
    -Wno-narrowing
    # One value is retained for a disabled legacy code path in ukengine.cpp.
    -Wno-unused-but-set-variable
    -Wno-write-strings
  )
endif()

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
