cmake_minimum_required(VERSION 3.15.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project("GameBoyMeBob")
set(GBMB_TARGET "GameBoyMeBob")
set(TEST_TARGET "GbmbTest")

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb")

set(BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

find_package(spdlog REQUIRED)

set(GBMB_SRCS
  # Core files
  ${BASE_DIR}/src/cartridge.cc
  ${BASE_DIR}/src/cpu.cc
  ${BASE_DIR}/src/interrupts.cc
  ${BASE_DIR}/src/mmu.cc
  ${BASE_DIR}/src/registers.cc
  ${BASE_DIR}/src/serial.cc
  ${BASE_DIR}/src/timer.cc

  ## Memory Bank Controllers
  ${BASE_DIR}/src/mbc/mbc0.cc
  ${BASE_DIR}/src/mbc/mbc1.cc

  # Debug files

  # Util files

  # Emulator files

  # Frontend files

  # Main ile
)

add_executable(${GBMB_TARGET}
  ${GBMB_SRCS}
  ${BASE_DIR}/main.cc
)

target_include_directories(${GBMB_TARGET} PRIVATE
  ${BASE_DIR}/src
)

target_link_libraries(${GBMB_TARGET}
  spdlog::spdlog_header_only
)

enable_testing()
find_package(GTest)
include(GoogleTest)

set(TEST_SRCS
  ${BASE_DIR}/tests/mem_test.cc
  ${BASE_DIR}/tests/cpu_test.cc
)

add_executable(${TEST_TARGET}
  ${GBMB_SRCS}
  ${TEST_SRCS}
)

target_include_directories(${TEST_TARGET} PRIVATE
  ${BASE_DIR}/src
)

target_link_libraries(${TEST_TARGET}
  spdlog::spdlog_header_only
  GTest::gtest_main
)

gtest_discover_tests(${TEST_TARGET})
