cmake_minimum_required(VERSION 2.8)

project(Matrix-Raspberry)

set(CMAKE_VERBOSE_MAKEFILE OFF)

SET(CMAKE_SYSTEM_NAME Linux)

set(MATRIX_VERSION_MAJOR "\"0\"")
set(MATRIX_VERSION_MINOR "\"2\"")

# Get the current working branch
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
set(GIT_BRANCH "\"${GIT_BRANCH}\"")

# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
set(GIT_COMMIT_HASH "\"${GIT_COMMIT_HASH}\"")

set(CMAKE_C_FLAGS_DEBUG "-ggdb -O0 -Wall -Wpedantic -Wextra -pedantic")

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")

include_directories("${PROJECT_BINARY_DIR}/wiringPi/wiringPi")

configure_file(
  "${PROJECT_SOURCE_DIR}/matrix-config.h.in"
  "${PROJECT_BINARY_DIR}/matrix-config.h"
)

set(MATRIX_SRCS src/74hc595.c src/74hc595.h
                src/SPI.c src/SPI.h
                src/graphics.c src/graphics.h
                src/main.c src/matrix.c
                src/matrix.h src/tools.c
                src/tools.h
                src/main.c)

add_executable(Matrix-Raspberry ${MATRIX_SRCS})
target_link_libraries(Matrix-Raspberry wiringPi)
