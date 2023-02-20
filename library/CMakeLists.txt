cmake_minimum_required(VERSION 3.6)

add_library(
  libPico STATIC
  font.cpp
  glyph.cpp
  fontServer.cpp
  frameBuffer.cpp
  geometry.cpp
  layoutServer.cpp
  quoteServer.cpp
  topCat.cpp
  displayDriver.cpp)

set_target_properties(libPico PROPERTIES OUTPUT_NAME "pico")

target_include_directories(libPico PUBLIC ${CMAKE_HOME_DIRECTORY}/headers
                                          ${CMAKE_HOME_DIRECTORY}/fonts)