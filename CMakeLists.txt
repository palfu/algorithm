cmake_minimum_required(VERSION 2.8.3)
project(algorithm)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -std=c++11 -O -Wall -g -ggdb -fPIC -fopenmp")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -std=c++11 -O3 -Wall  -g -fPIC -fopenmp -pthread")

execute_process( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/${ARCHITECTURE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/${ARCHITECTURE})

find_package(Eigen3 REQUIRED)
find_package(OpenCV 3 REQUIRED)

include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIRS}
)

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${OpenCV_LIB_DIR})

add_subdirectory(optimize)