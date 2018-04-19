#ifndef ZAD2_MAIN_H
#define ZAD2_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

void handler_SIG_INT(int signo);

void handler_SIG_USR1(int signo, siginfo_t *siginfo, void *context);

void handler_SIG_DEFAULT(int signo, siginfo_t *siginfo, void *context);

void set_SIGUSR1_handler();

void set_SIGDEFAULT_handler();

void set_SIGINT_handler();

void prepare();

void print_help();

int main(int argc, char *argv[]);


#endif //ZAD2_MAIN_H
