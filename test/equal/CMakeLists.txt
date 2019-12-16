cmake_minimum_required(VERSION 2.8)
project(test_equal)

set(CMAKE_CXX_FLAGS "-std=c++11")

add_executable(test_equal test_equal.cpp)
link_libraries(test_equal)