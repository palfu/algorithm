cmake_minimum_required(VERSION 2.8)
project(test_filter)

set(CMAKE_CXX_FLAGS "-std=c++11")

find_package(OpenCV 3 REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

message("opencv libs " ${OpenCV_LIBS})



add_executable(test_filter test_filter.cpp)
target_link_libraries(test_filter ${OpenCV_LIBS})