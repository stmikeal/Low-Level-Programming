cmake_minimum_required(VERSION 2.8)
project(LLP3 C)

set(CMAKE_C_STANDARD 11)

include_directories(parser_module/include)
include_directories(parser_module/include/parser)
include_directories(parser_module/include/iotools)
include_directories(parser_module/include/structure)
include_directories(database_module/include)
include_directories(proto_module)
include_directories(proto_module/nanopb)

add_executable(LLP3 server.c
        proto_module/message.pb.c
        proto_module/nanopb/pb_encode.c
        proto_module/nanopb/pb_decode.c
        proto_module/nanopb/pb_common.c)
add_executable(LLP3_1 client.c
        proto_module/message.pb.c
        proto_module/nanopb/pb_encode.c
        proto_module/nanopb/pb_decode.c
        proto_module/nanopb/pb_common.c)
add_subdirectory(parser_module)
target_link_libraries(LLP3 Xpath_lib)
target_link_libraries(LLP3_1 Xpath_lib)
add_subdirectory(database_module)
target_link_libraries(LLP3 Database_lib)
target_link_libraries(LLP3_1 Database_lib)
