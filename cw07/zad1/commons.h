#ifndef ZAD1_COMMON_H
#define ZAD1_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <string.h>
#include <bits/time.h>
#include <time.h>
#include <signal.h>
#include <wait.h>
#include <unistd.h>

#include "semaphores.h"
#include "queue.h"

#define SEMG_KEY 1
#define BARBER_SEM_NUM 0
#define FIFO_SEM_NUM 1
#define CUT_SEM_NUM 2

#define HOME "HOME"

__time_t get_time();

#endif //ZAD1_COMMON_H
