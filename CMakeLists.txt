cmake_minimum_required(VERSION 3.27)

project(sched-im VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_library(SchedIm src/schedim/base.cpp)
target_compile_options(SchedIm PRIVATE -Wno-register)

find_package(GTest REQUIRED)
enable_testing()

add_executable(sched-im_test src/sched-im_test.cc)
target_link_libraries(sched-im_test GTest::gtest_main SchedIm)

include(GoogleTest)
gtest_discover_tests(sched-im_test)
