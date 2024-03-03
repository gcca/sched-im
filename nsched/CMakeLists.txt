find_package(Curses REQUIRED)
add_executable(nsched nsched.cc)
target_link_libraries(nsched SchedIm ${CURSES_LIBRARIES})
