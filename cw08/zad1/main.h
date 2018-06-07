#ifndef ZAD1_MAIN_H
#define ZAD1_MAIN_H

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <sys/times.h>
#include <time.h>

int main(int argc, char *argv[]);

void *thread(void *i);

int calculate_pixel(int x, int y);

void print_matrix(int **matrix, int H, int W);

void save_picture(char *picture_name, char *comment, int output_file);

void make_random_def_file(int fd);

void print_test(double user_time, double system_time);

void set_times();

void update_times();

#endif //ZAD1_MAIN_H
