cmake_minimum_required(VERSION 3.15)
project(archiver)
message("Building ${PROJECT_NAME} with CMake version: ${CMAKE_VERSION}")

# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED True)
# set(CMAKE_CXX_FLAGS "-Wall -Werror -stdlib=libc++")

find_package(Boost REQUIRED)
find_package(GTest REQUIRED)

add_executable(${PROJECT_NAME} src/main.cc)
target_link_libraries(${PROJECT_NAME} boost::boost gtest::gtest)
