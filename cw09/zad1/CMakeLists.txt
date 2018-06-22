cmake_minimum_required(VERSION 3.10)
project(zad1 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c11 -pthread")

add_executable(zad1 main.c )