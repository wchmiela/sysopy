#ifndef CW03_MAIN_H
#define CW03_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>

const char *MSG_SET_LIMIT_ERROR = "Wystapil blad w ustawieniu limitow programu.";

const char *MSG_NO_ARGS = "Nie podano argumentow programu.";

const char *MSG_RUN_ERROR = "Wystapil blad przy wykonywaniu polecenia ";

const char *MSG_OPENING_FAILED = "Wystapil blad przy otwieraniu pliku";

void remove_endline(char *str);

void print_help();

void execute(char *program_options, char *string, rlim_t cpulimit, rlim_t memlimit);

int main(int argc, char *argv[]);

#endif //CW03_MAIN_H
