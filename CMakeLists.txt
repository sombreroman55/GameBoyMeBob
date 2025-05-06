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

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(SDL3 REQUIRED)

if (CMAKE_SYSTEM_NAME STREQUAL Linux)
  find_package(X11 REQUIRED)

  if (NOT X11_Xi_FOUND)
    message(FATAL_ERROR "X11 Xi library is required")
  endif ()
endif ()

set(IMGUI_SRCS
  # Main imgui files
  ${BASE_DIR}/third-party/imgui/imgui.cpp
  ${BASE_DIR}/third-party/imgui/imgui_demo.cpp
  ${BASE_DIR}/third-party/imgui/imgui_draw.cpp
  ${BASE_DIR}/third-party/imgui/imgui_tables.cpp
  ${BASE_DIR}/third-party/imgui/imgui_widgets.cpp

  # Imgui backend files
  ${BASE_DIR}/third-party/imgui/backends/imgui_impl_sdl3.cpp
  ${BASE_DIR}/third-party/imgui/backends/imgui_impl_opengl3.cpp
)

set(GBMB_SRCS
  # Core files
  ${BASE_DIR}/src/cartridge.cc
  ${BASE_DIR}/src/cpu.cc
  ${BASE_DIR}/src/gb.cc
  ${BASE_DIR}/src/interrupts.cc
  ${BASE_DIR}/src/joypad.cc
  ${BASE_DIR}/src/mmu.cc
  ${BASE_DIR}/src/ppu.cc
  ${BASE_DIR}/src/registers.cc
  ${BASE_DIR}/src/serial.cc
  ${BASE_DIR}/src/timer.cc

  ## Memory Bank Controllers
  ${BASE_DIR}/src/mbc/mbc0.cc
  ${BASE_DIR}/src/mbc/mbc1.cc

  # Debug files

  # Util files

)

add_executable(${GBMB_TARGET}
  ${IMGUI_SRCS}
  ${GBMB_SRCS}

  # Main ile
  ${BASE_DIR}/main.cc
)

target_include_directories(${GBMB_TARGET} PRIVATE
  ${BASE_DIR}/src
  ${BASE_DIR}/third-party/imgui
  ${BASE_DIR}/third-party/imgui/backends
  ${BASE_DIR}/third-party/imgui_club/imgui_memory_editor
  ${OPENGL_INCLUDE_DIRS}
  ${SDL3_INCLUDE_DIRS}
)

target_link_libraries(${GBMB_TARGET}
  ${SDL3_LIBRARIES}
  ${OPENGL_LIBRARIES}
  spdlog::spdlog_header_only
  -lzip
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
