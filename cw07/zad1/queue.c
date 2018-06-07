#include "queue.h"

#define SIZE_IND 0
#define POINTER_IND 1
#define CLI_NUM_IND 2
#define CHAIR_IND 3
#define QUEUE_START_POINTER 4

void bqueue_init(pid_t *arr, int size)
{
    // size of queue
    arr[0] = size + 4;
    // queue pointer
    arr[1] = QUEUE_START_POINTER;
    // number of clients
    arr[2] = 0;
    // pid of customer on chair
    arr[3] = -1;
}

int bqueue_empty(pid_t *arr)
{
    return (arr[CLI_NUM_IND] == 0) ? 1 : 0;
}

int bqueue_full(pid_t *arr)
{
    int cli_max = arr[SIZE_IND] - 4;
    return (arr[CLI_NUM_IND] == cli_max) ? 1 : 0;
}

int bqueue_put(pid_t *arr, pid_t pid)
{
    if (bqueue_full(arr) == 1)
        return -1;
    int ind = arr[POINTER_IND];
    arr[ind] = pid;
    arr[POINTER_IND]++;
    arr[CLI_NUM_IND]++;

    return 0;
}

pid_t bqueue_get(pid_t *arr)
{
    if (bqueue_empty(arr) == 1)
        return -1;
    pid_t pid = arr[QUEUE_START_POINTER];
    int len = arr[CLI_NUM_IND] + 4;

    for (int i = QUEUE_START_POINTER; i < len - 1; i++) {
        arr[i] = arr[i + 1];
    }

    arr[POINTER_IND]--;
    arr[CLI_NUM_IND]--;

    return pid;
}

void bqueue_occupy_chair(pid_t *arr, pid_t pid)
{
    arr[CHAIR_IND] = pid;
}

pid_t bqueue_get_customer_from_chair(pid_t *arr)
{
    return arr[CHAIR_IND];
}

void bqueue_show(pid_t *arr)
{
    if (bqueue_empty(arr) == 1) {
        printf("%s\n", "Barber queue is empty.");
        return;
    }

    int len = arr[CLI_NUM_IND] + 4;
    printf("%s\n", "Barber queue:");
    for (int i = QUEUE_START_POINTER; i < len; i++) {
        printf("\t(%d) PID: %d"  "\n", i - QUEUE_START_POINTER, arr[i]);
    }
    printf("\tTotal clients number on waiting room: %d"  "\n", arr[CLI_NUM_IND]);
    printf("Client PID who is on chair: %d"  "\n", arr[CHAIR_IND]);
    printf("Current pointer (index): %d"  "\n", arr[POINTER_IND]);
    printf("Queue size: %d"  "\n\n", arr[SIZE_IND]);
}
