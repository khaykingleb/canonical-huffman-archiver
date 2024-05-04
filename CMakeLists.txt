cmake_minimum_required(VERSION 3.15)
project(archiver)
message("Building ${PROJECT_NAME} with CMake version: ${CMAKE_VERSION}")

add_subdirectory(src)
add_subdirectory(tests)
