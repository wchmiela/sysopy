#ifndef ZAD1_COMMON_H
#define ZAD1_COMMON_H

#include <sys/stat.h>
#include <semaphore.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <wait.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>

#include "queue.h"

#define BARBER_SEM_NUM 0
#define FIFO_SEM_NUM 1
#define CUT_SEM_NUM 2

#define MEMORY_NAME "memory"
#define BARBER_NAME "barber"
#define FIFO_NAME "fifo"
#define CUT_NAME "cut"

__time_t get_time();

#endif //ZAD1_COMMON_H
