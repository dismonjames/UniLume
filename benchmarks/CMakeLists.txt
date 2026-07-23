# UniLume project
# SPDX-License-Identifier: GPL-2.0-or-later

find_package(Git QUIET)
set(UNILUME_BENCHMARK_COMMIT "unknown")
if(Git_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    OUTPUT_VARIABLE UNILUME_BENCHMARK_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )
endif()

add_executable(unilume_core_benchmark
  core/benchmark_options.cpp
  core/burst_benchmark.cpp
  core/checksum.cpp
  core/correctness.cpp
  core/corpus_loader.cpp
  core/engine_fixture.cpp
  core/latency_benchmark.cpp
  core/main.cpp
  core/report_writer.cpp
  core/statistics.cpp
  core/system_metadata.cpp
  core/throughput_benchmark.cpp
)

target_link_libraries(unilume_core_benchmark PRIVATE
  unilume_engine
  unilume_sanitizer_options
)

target_include_directories(unilume_core_benchmark PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/core
)

target_compile_definitions(unilume_core_benchmark PRIVATE
  UNILUME_BENCHMARK_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
  UNILUME_BENCHMARK_COMMIT="${UNILUME_BENCHMARK_COMMIT}"
  UNILUME_BENCHMARK_COMPILER="${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}"
  UNILUME_BENCHMARK_CORPUS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/corpus"
)

set_target_properties(unilume_core_benchmark PROPERTIES
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(unilume_core_benchmark PRIVATE -Wall -Wextra)
endif()

if(BUILD_TESTING)
  add_test(
    NAME benchmark-smoke
    COMMAND unilume_core_benchmark --smoke
  )
  set_tests_properties(benchmark-smoke PROPERTIES TIMEOUT 60)
endif()
