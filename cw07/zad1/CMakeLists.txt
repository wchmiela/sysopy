cmake_minimum_required(VERSION 3.0)
project(zad1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -Wall -Werror -w")

set(BARBER_SOURCE_FILES commons.c barber.c queue.c semaphores.c barber.h client.h)
set(CLIENT_SOURCE_FILES commons.c client.c queue.c semaphores.c)
add_executable(barber ${BARBER_SOURCE_FILES})
add_executable(client ${CLIENT_SOURCE_FILES})