cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES_SERVER server.c server.h consts.h)
add_executable(server ${SOURCE_FILES_SERVER})

set(SOURCE_FILES_CLIENT client.c client.h consts.h )
add_executable(client ${SOURCE_FILES_CLIENT})

target_link_libraries(server -lrt)
target_link_libraries(client -lrt)