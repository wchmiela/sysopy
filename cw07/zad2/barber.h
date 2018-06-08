#ifndef ZAD1_BARBER_H
#define ZAD1_BARBER_H

#include "commons.h"

int main(int argc, char **argv);

void clean();

void make_haircut(int clientPid);

void work();

void sigint_handler(int sig, siginfo_t *siginfo, void *context);

void configure();

#endif //ZAD1_BARBER_H
