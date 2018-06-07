cmake_minimum_required(VERSION 3.10)
project(zad1 C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c11 -pthread")

set(CMAKE_C_STANDARD 11)

add_executable(zad1 main.c main.h)

target_link_libraries(zad1 m)