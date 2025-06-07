cmake_minimum_required(VERSION 3.13)

add_library(
  libPico STATIC
  ${CMAKE_HOME_DIRECTORY}/library/font.cpp
  ${CMAKE_HOME_DIRECTORY}/library/glyph.cpp
  ${CMAKE_HOME_DIRECTORY}/library/fontServer.cpp
  ${CMAKE_HOME_DIRECTORY}/library/frameBuffer.cpp
  ${CMAKE_HOME_DIRECTORY}/library/geometry.cpp
  ${CMAKE_HOME_DIRECTORY}/library/layoutServer.cpp
  ${CMAKE_HOME_DIRECTORY}/library/quoteServer.cpp
  ${CMAKE_HOME_DIRECTORY}/library/topCat.cpp
  ${CMAKE_HOME_DIRECTORY}/library/displayDriver.cpp)

set_target_properties(libPico PROPERTIES OUTPUT_NAME "pico")

target_include_directories(libPico PUBLIC ${CMAKE_HOME_DIRECTORY}/headers
                                          ${CMAKE_HOME_DIRECTORY}/fonts)
                                          

# ##############################################################################
# ############## Executable for main app ################
# ##############################################################################
add_executable(epdc epdc.cpp ntpClient.cpp uc8151.cpp)
pico_set_program_name(epdc "epdc")
pico_set_program_version(epdc "0.1")

target_include_directories(
  epdc
  PUBLIC ${CMAKE_HOME_DIRECTORY}/headers
)

target_compile_definitions(epdc PRIVATE WIFI_SSID="Treehouse"
                                        WIFI_PASSWORD="jWafsbwrh@12" PICO)

pico_enable_stdio_usb(epdc 0)
pico_enable_stdio_uart(epdc 1)

target_link_libraries(
  epdc
  libPico
  pico_stdlib
  hardware_rtc
  hardware_spi
  hardware_i2c
  pico_cyw43_arch_lwip_poll)

pico_add_extra_outputs(epdc)
