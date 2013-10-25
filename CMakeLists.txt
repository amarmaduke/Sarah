

cmake_minimum_required(VERSION 2.8)

project(presburger CXX)

# Set compilation flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

# FIXME: Put this in a custom CMake module.
find_path(GMP_INCLUDE_DIR NAMES gmp.h)
find_library(GMP_LIBRARIES NAMES gmp libgmp)

# Enable testing.
enable_testing()

include_directories(src ${GMP_INCLUDE_DIR})

add_subdirectory(src)
