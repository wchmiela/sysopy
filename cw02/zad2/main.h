#ifndef ZAD2_MAIN_H
#define ZAD2_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <ftw.h>

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

void print_file_data_nftw(char *file_path, struct stat *stat, unsigned int proper_size);

void list_files(char *directory_name, char *date, char relation);

void run_standard(char *directory_name, char *date, char relation);

int nftw(const char *dirpath, int (*fun)(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf),
         int nopenfd, int flags);

void run_nftw(char *directory_name, char *date, char relation);

int main(int argc, char *argv[]);

#endif //ZAD2_MAIN_H
