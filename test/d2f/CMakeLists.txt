cmake_minimum_required(VERSION 2.8)
project(d2f)

set(CMAKE_CXX_FLAGS "-std=c++11")

find_package(OpenCV 3 REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(d2f d2f.cpp)
target_link_libraries(d2f ${OpenCV_LIBS})