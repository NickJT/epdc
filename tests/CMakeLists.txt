cmake_minimum_required(VERSION 3.6)

################# Basic tests for geometry classes ######################
add_executable(tests ${CMAKE_HOME_DIRECTORY}/tests/tests.cpp)
target_include_directories(tests PUBLIC ${CMAKE_HOME_DIRECTORY}/headers)
target_link_libraries(tests libPico)
#########################################################################

################# Standalone test for quoteServer class #################
add_executable(qsTests ${CMAKE_HOME_DIRECTORY}/tests/qsTests.cpp)
target_link_libraries(qsTests libPico)
target_include_directories(qsTests PUBLIC ${CMAKE_HOME_DIRECTORY}/headers)
#########################################################################

################# Standalone test for wordwrap function #################
add_executable(tokenise tokenise.cpp)
target_include_directories(tokenise PUBLIC ${CMAKE_HOME_DIRECTORY}/headers)
#########################################################################
