#ifndef ZAD1_MAIN_H
#define ZAD1_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>

const char *CMP_DOT = ".";

const char *CMP_DOT_DOT = "..";

const char *STR_PATH = "Bezwgledna sciezka pliku:";

const char *STR_SIZE = "Rozmiar w bajtach:";

const char *STR_MOD = "Prawa dostepu:";

const char *STR_LAST_MODIFICATION = "Ostatnia modyfikacja:";

void print_help();

int number_from_char(char c);

char compare_dates(char *date1, char *date2);

char *make_mods(__mode_t mode);

char *format_date(struct timespec *val);

void print_file_data(char *file_path, struct stat stat);

void list_files(char *directory_name, char *date, char relation, int argc, char *argv[]);

void run_standard(char *directory_name, char *date, char relation, int argc, char *argv[]);

int main(int argc, char *argv[]);

#endif //ZAD1_MAIN_H
