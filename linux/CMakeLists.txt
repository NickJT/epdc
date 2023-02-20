cmake_minimum_required(VERSION 3.6)

# ##############################################################################
# ############## Executable for linux/X11 application ##########################
# ##############################################################################
add_executable(xclock desktop.cpp x11Driver.cpp)
target_include_directories(xclock PUBLIC ${CMAKE_HOME_DIRECTORY}/headers)
target_link_libraries(xclock X11 libPico)

# ##############################################################################
# ############### Executable for BDF include file generator app ################
# ##############################################################################
add_executable(fontGen fontGenerator.cpp)
target_include_directories(fontGen PUBLIC ${CMAKE_HOME_DIRECTORY}/headers
                                          ${CMAKE_HOME_DIRECTORY}/fonts)
target_link_libraries(fontGen libPico)

# ##############################################################################
# ############## Executable for text include file generator app ################
# ##############################################################################
add_executable(textGen textGenerator.cpp)
target_include_directories(textGen PUBLIC ${CMAKE_HOME_DIRECTORY}/headers
                                          ${CMAKE_HOME_DIRECTORY}/fonts)
