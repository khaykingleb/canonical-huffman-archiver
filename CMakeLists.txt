cmake_minimum_required(VERSION 3.15)
project(Huffman Archiver)

message("Building with CMake version: ${CMAKE_VERSION}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Werror -stdlib=libc++")

find_package(GTest REQUIRED)

enable_testing()
include(GoogleTest)
include(CTest)
