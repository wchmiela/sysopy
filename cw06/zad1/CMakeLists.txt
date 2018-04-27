cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES_SERVER server.c)
add_executable(server ${SOURCE_FILES_SERVER})

set(SOURCE_FILES_CLIENT client.c)
add_executable(client ${SOURCE_FILES_CLIENT})