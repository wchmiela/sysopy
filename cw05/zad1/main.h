#ifndef CW05_MAIN_H
#define CW05_MAIN_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <getopt.h>

const char *MSG_OPENING_FAILED = "Blad w otwieraniu pliku.";

const char *MSG_RUN_ERROR = "Blad w uruchamianiu procesu.";

const char *MSG_FORK_ERROR = "Blad w forkowaniu procesu.";

void fix(char *t);

void print_help();

int main(int argc, char *argv[]);

#endif //CW05_MAIN_H
