cmake_minimum_required(VERSION 3.21)
project(LLP1 C)

set(CMAKE_C_STANDARD 11)

include_directories(include/structure)
include_directories(include/parser)
include_directories(include/iotools)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CURRENT_SOURCE_DIR .)
set(SOURCES src/configuration.c
        src/adv_tools/file_manager.c
        src/adv_types/tree_header_tools.c
        src/adv_types/tuple_tools.c
        src/basic_tools/basic_convert.c
        src/basic_tools/basic_file_manager.c
        src/basic_types/list.c
        src/basic_types/tree_header.c
        src/basic_types/tuple.c
        src/commands/add_command.c
        src/commands/find_command.c
        src/commands/remove_command.c
        src/commands/update_command.c
        src/crud_tools/crud.c
        src/generator/empty_generator.c
        src/ui/console_tools.c
        src/ui/interactive.c)
set(HEADERS include/configuration.h
        include/adv_tools/file_manager.h
        include/adv_types/tree_header_tools.h
        include/adv_types/tuple_tools.h
        include/basic_tools/basic_convert.h
        include/basic_tools/basic_file_manager.h
        include/basic_types/list.h
        include/basic_types/tree_header.h
        include/basic_types/tuple.h
        include/commands/add_command.h
        include/commands/find_command.h
        include/commands/remove_command.h
        include/commands/update_command.h
        include/crud_tools/crud.h
        include/generator/empty_generator.h
        include/ui/console_tools.h
        include/ui/interactive.h)

add_library(Database_lib SHARED ${SOURCES} ${HEADERS})
target_include_directories(Database_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
