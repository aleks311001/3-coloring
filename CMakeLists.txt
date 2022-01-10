cmake_minimum_required(VERSION 3.21)
project(3coloring)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

add_executable(3coloring main.cpp SSS.hpp Coloring.hpp)
