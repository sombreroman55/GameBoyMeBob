cmake_minimum_required(VERSION 3.15.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project("GameBoyMeBob")
set(GBMB_TARGET "GameBoyMeBob")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb")

set(BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(GBMB_SRCS
  # Core files

  # Debug files

  # Util files

  # Emulator files

  # Frontend files

  # Main ile
)

add_executable(${GBMB_TARGET}
  ${GBMB_SRCS}
)

target_include_directories(${GBMB_TARGET} PRIVATE
  ${BASE_DIR}/src
)
