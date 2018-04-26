cmake_minimum_required(VERSION 2.8)
project(zad2 C)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES_MASTER master.c)
add_executable(master ${SOURCE_FILES_MASTER})

set(SOURCE_FILES_SLAVE slave.c)
add_executable(slave ${SOURCE_FILES_SLAVE})