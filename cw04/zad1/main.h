#ifndef CW04_MAIN_H
#define CW04_MAIN_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>

static void handler_SIG_INT(int signo);

static void handler_SIG_TSTP(int signo);

int main(int argc, char *argv[]);

#endif //CW04_MAIN_H
