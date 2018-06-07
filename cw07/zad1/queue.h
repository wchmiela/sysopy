#ifndef ZAD1_QUEUE_H
#define ZAD1_QUEUE_H

#include "commons.h"

void bqueue_init(pid_t *arr, int size);

int bqueue_empty(pid_t *arr);

int bqueue_full(pid_t *arr);

int bqueue_put(pid_t *arr, pid_t pid);

pid_t bqueue_get(pid_t *arr);

void bqueue_occupy_chair(pid_t *arr, pid_t pid);

pid_t bqueue_get_customer_from_chair(pid_t *arr);

void bqueue_show(pid_t *arr);

#endif //ZAD1_QUEUE_H
