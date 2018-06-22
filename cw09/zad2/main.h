//
// Created by wojtek on 14.06.18.
//

#ifndef ZAD1_MAIN_H
#define ZAD1_MAIN_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <stdbool.h>
#include <semaphore.h>

int length_search(int line_length);

void *producer(void *pVoid);

void *consumer(void *pVoid);

void start_threads();

void join_threads();

void sig_handler(int signo);


#endif //ZAD1_MAIN_H
