cmake_minimum_required(VERSION 3.13)
project(TRON)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-lncurses -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio")
add_executable(TRON main.cpp)
target_link_libraries(TRON curses)