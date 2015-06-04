cmake_minimum_required(VERSION 2.8.8)

set(PROJECT_NAME_STR kaveri)
PROJECT(${PROJECT_NAME_STR} C CXX)
#TODO CHANGE THIS OPTION
# set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_BUILD_TYPE Debug)

find_package(Threads REQUIRED)

# SDSL directory
set(SDSL_SOURCE_DIR "${PROJECT_SOURCE_DIR}/ext/sdsl-lite")

# flags
set(CMAKE_CXX_FLAGS                "-Wall -std=c++11 -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -std=c++11 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -std=c++11 -O3")

# include the sdsl-lite and googletest library
add_subdirectory("${PROJECT_SOURCE_DIR}/ext/sdsl-lite")
add_subdirectory("${PROJECT_SOURCE_DIR}/ext/googletest")

# add SDSL includes path
include_directories("${CMAKE_BINARY_DIR}/ext/sdsl-lite/include")
include_directories("${CMAKE_BINARY_DIR}/ext/sdsl-lite/external/libdivsufsort-2.0.1/include/")

# Compile rkaveri - stage 1 filter
ADD_EXECUTABLE(arakawa.x src/main.cpp src/ReadsDB.cpp src/AppConfig.cpp
                         src/compute_klcp.cpp)

#Link SDSL library
target_link_libraries(arakawa.x sdsl divsufsort divsufsort64)

# Testing
# enable_testing()
# set(GTEST_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/ext/googletest/include")
# set(GTEST_LIBS_DIR "${CMAKE_BINARY_DIR}/ext/googletest")
# set(PROJECT_TEST_NAME ${PROJECT_NAME_STR}_test)
# file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/test/*.cpp)

# add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES} src/ReadsDB.cpp src/KmerId.cpp)
# add_dependencies(${PROJECT_TEST_NAME} gtest gtest_main)
# include_directories(${GTEST_INCLUDE_DIRS})
# include_directories("${PROJECT_SOURCE_DIR}/src")
# target_link_libraries(${PROJECT_TEST_NAME} ${CMAKE_THREAD_LIBS_INIT}
#                       gtest gtest_main sdsl divsufsort divsufsort64)
# add_test(arakawa_test ${PROJECT_TEST_NAME})

# test files
# configure_file(${PROJECT_SOURCE_DIR}/test/t1.fq . COPYONLY)
# configure_file(${PROJECT_SOURCE_DIR}/test/t2.fq . COPYONLY)
