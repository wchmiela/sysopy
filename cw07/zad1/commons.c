#include "commons.h"

__time_t get_time()
{
    struct timespec time;
    if (clock_gettime(CLOCK_MONOTONIC, &time) == -1) {
        perror("Blad pobrania czasu");
        exit(EXIT_FAILURE);
    }

    return time.tv_nsec / 1000;
}