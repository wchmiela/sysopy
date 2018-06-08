cmake_minimum_required(VERSION 3.0)
project(zad2)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -Wall -Werror -w -pthread")

set(BARBER_SOURCE_FILES commons.c barber.c queue.c)
set(CLIENT_SOURCE_FILES commons.c client.c queue.c)

add_executable(barber ${BARBER_SOURCE_FILES})
add_executable(client ${CLIENT_SOURCE_FILES})

target_link_libraries(barber -lrt)
target_link_libraries(client -lrt)