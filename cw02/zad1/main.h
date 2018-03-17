#ifndef FULL_APP_MAIN_H
#define FULL_APP_MAIN_H

#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/times.h>
#include <time.h>

const char *DEV_RANDOM = "/dev/urandom";

void print_test(double user_time, double system_time);

void set_times();

void update_times();

void print_help();

void sys_generate(const char *file_name, int n, int m);

void sys_sort(const char *file_name, int records_count, int record_size);

void lib_sort(const char *file_name, int records_count, int record_size);

void lib_copy(char *file_name, char *target_file_name, int records_count, int record_size);

void sys_copy(char *file_name, char *target_file_name, int records_count, int record_size);

int main(int argc, char *argv[]);

#endif //FULL_APP_MAIN_H
