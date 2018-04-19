cmake_minimum_required(VERSION 3.9)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES2 child.c)
add_executable(child ${SOURCE_FILES2})

set(SOURCE_FILES main.c)
add_executable(zad3 ${SOURCE_FILES})