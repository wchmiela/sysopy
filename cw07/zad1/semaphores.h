#ifndef ZAD1_SEMAPHORES_H
#define ZAD1_SEMAPHORES_H

#include "commons.h"

void give_semaphore(int semid, unsigned short sem_num);

void take_semaphore(int semid, unsigned short sem_num);

#endif //ZAD1_SEMAPHORES_H
