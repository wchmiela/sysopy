#include "semaphores.h"

void give_semaphore(int sid, unsigned short sind) {
    struct sembuf smbuf;
    smbuf.sem_num = sind;
    smbuf.sem_op = 1;
    smbuf.sem_flg = 0;
    if (semop(sid, &smbuf, 1) == -1) {
        printf("semop error: giving semaphore error\n");
        exit(EXIT_FAILURE);
    }
}

void take_semaphore(int sid, unsigned short sind) {
    struct sembuf smbuf;
    smbuf.sem_num = sind;
    smbuf.sem_op = -1;
    smbuf.sem_flg = 0;
    if (semop(sid, &smbuf, 1) == -1) {
        printf("semop error: taking semaphore error\n");
        exit(EXIT_FAILURE);
    }
}
