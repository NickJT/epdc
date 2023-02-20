cmake_minimum_required(VERSION 3.6)

if(${CMAKE_CXX_COMPILER} STREQUAL "/usr/bin/arm-none-eabi-g++")
  message("\n\r******** Building pico directory **********\n\r")
  set(PICO_BOARD "pico_w")
  set(PICO_CYW43_SUPPORTED 1)
  message("\nIncluding pico_sdk_import.cmake")
  include(pico_sdk_import.cmake)
endif()

project(epdClock C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

add_compile_options(
  -Wall
  # -Wextra
  -Wunreachable-code
  -Wno-format
  -Wno-unused-function
  -Wno-maybe-uninitialized
  -Wno-unknown-pragmas)

if(${CMAKE_CXX_COMPILER} STREQUAL "/usr/bin/g++")
  message("\n\r******** Entering library directory **********\n\r")
  add_subdirectory(library)
  message("\n\r******** Entering linux directory **********\n\r")
  add_subdirectory(linux)
  message("\n\r******** Entering tests directory **********\n\r")
  add_subdirectory(tests)
elseif(${CMAKE_CXX_COMPILER} STREQUAL "/usr/bin/arm-none-eabi-g++")
  message("\n\r******** Entering pico directory **********\n\r")
  add_subdirectory(pico)
endif()
