#include "semaphores.h"

void give_semaphore(int semid, unsigned short sem_num)
{
    struct sembuf smbuf;
    smbuf.sem_num = sem_num;
    smbuf.sem_op = 1;
    smbuf.sem_flg = 0;
    if (semop(semid, &smbuf, 1) == -1) {
        perror("semop error: giving semaphore error");
        exit(EXIT_FAILURE);
    }
}

void take_semaphore(int semid, unsigned short sem_num)
{
    struct sembuf smbuf;
    smbuf.sem_num = sem_num;
    smbuf.sem_op = -1;
    smbuf.sem_flg = 0;
    if (semop(semid, &smbuf, 1) == -1) {
        perror("semop error: taking semaphore error");
        exit(EXIT_FAILURE);
    }
}
