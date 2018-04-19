#ifndef ZAD2_CHILD_H
#define ZAD2_CHILD_H

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

void handler_SIG_USR2(int signo, siginfo_t *siginfo, void *context);

void prepare();

int main(int argc, char *argv[]);

#endif //ZAD2_CHILD_H
