cmake_minimum_required(VERSION 2.8)

project(matrixd)

set(CMAKE_VERBOSE_MAKEFILE OFF)

SET(CMAKE_SYSTEM_NAME Linux)

set(MATRIXD_VERSION_MAJOR "\"0\"")
set(MATRIXD_VERSION_MINOR "\"3\"")

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

set(CMAKE_C_FLAGS "-ggdb -O3 -Wall -Wextra")

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")

find_package (Threads)

configure_file(
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
)

set(MATRIXD_SRCS src/74hc595.c src/74hc595.h
                src/SPI.c src/SPI.h
                src/graphics.c src/graphics.h
                src/main.c src/matrix.c
                src/matrix.h src/tools.c
                src/tools.h
                src/main.c)

add_executable(matrixd ${MATRIXD_SRCS})
target_link_libraries(matrixd wiringPi ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS matrixd DESTINATION sbin/)

add_subdirectory(webserver)
