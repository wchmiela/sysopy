cmake_minimum_required(VERSION 3.0)
project(zad2)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")

set(SOURCE_FILES main.c)
add_executable(zad2 ${SOURCE_FILES})