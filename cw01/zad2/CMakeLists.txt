cmake_minimum_required(VERSION 3.9)
project(zad2 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

add_subdirectory(libs)

set(SOURCE_FILES app.c)
add_executable(zad2 ${SOURCE_FILES})

target_link_libraries(zad2 static)