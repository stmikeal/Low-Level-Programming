cmake_minimum_required(VERSION 3.21)
project(LLP2 C)

set(CMAKE_C_STANDARD 11)

include_directories(include)
include_directories(include/structure)
include_directories(include/parser)
include_directories(include/iotools)

add_executable(LLP2
        include/structure/finite_state.h
        include/structure/view.h
        src/structure/view.c
        src/parser/parser.c
        src/iotools/printer.c
        src/main.c)
