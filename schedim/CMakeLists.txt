add_library(SchedIm schedim.cpp)
target_compile_options(SchedIm PRIVATE -Wno-register)

add_executable(schedim-test schedim-test.cc)
target_link_libraries(schedim-test GTest::gtest_main SchedIm)

gtest_discover_tests(schedim-test)
