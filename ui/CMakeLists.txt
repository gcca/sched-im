find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)
add_executable(ui-sched main.cpp)
target_link_libraries(ui-sched Qt6::Core Qt6::Gui Qt6::Widgets)
