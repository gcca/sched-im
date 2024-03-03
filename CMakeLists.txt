cmake_minimum_required(VERSION 3.27)

project(sched-im VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(GTest REQUIRED)
enable_testing()
include(GoogleTest)

add_subdirectory(schedim)
add_subdirectory(ui)
add_subdirectory(nsched)
