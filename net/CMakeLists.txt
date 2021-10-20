cmake_minimum_required(VERSION 3.0)

project(TINYWEBNET CXX)

set(CXX_FLAGS -g -O2 -Wall -Werror)
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_CXX_COMPILER "g++")

link_directories(../base/lib)
add_subdirectory(./src)
link_directories(./lib)

add_executable(EventLoop_test01 EventLoop_test01.cc)
target_link_libraries(EventLoop_test01 tinyWeb_base tinyWeb_net pthread)

add_executable(EventLoop_test02 EventLoop_test02.cc)
target_link_libraries(EventLoop_test02 tinyWeb_base tinyWeb_net pthread)

add_executable(EventLoop_test03 EventLoop_test03.cc)
target_link_libraries(EventLoop_test03 tinyWeb_base tinyWeb_net pthread)

add_executable(EventLoop_test04 EventLoop_test04.cc)
target_link_libraries(EventLoop_test04 tinyWeb_base tinyWeb_net pthread)

add_executable(InetAddress_unittest InetAddress_unittest.cc)
target_link_libraries(InetAddress_unittest tinyWeb_base tinyWeb_net pthread boost_unit_test_framework)

add_executable(EventLoop_test06 EventLoop_test06.cc)
target_link_libraries(EventLoop_test06 tinyWeb_base tinyWeb_net pthread)