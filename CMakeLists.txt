cmake_minimum_required(VERSION 3.29)
project(SELab1)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib)
include_directories(googletest/include googletest)

add_executable(test main.cpp
        graph.h
        test_b.cpp)
target_link_libraries(test gtest gtest_main )
