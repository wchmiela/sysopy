cmake_minimum_required(VERSION 3.9)
project(zestaw1 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

set(SOURCE_FILES my_lib.c)
add_library(shared SHARED ${SOURCE_FILES})
add_library(static STATIC ${SOURCE_FILES})