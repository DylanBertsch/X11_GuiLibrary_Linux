cmake_minimum_required(VERSION 3.15)
project(x11Test)

set(CMAKE_CXX_STANDARD 14)

add_executable(x11Test main.cpp)
find_package(X11 REQUIRED)
target_link_libraries(x11Test ${X11_LIBRARIES})