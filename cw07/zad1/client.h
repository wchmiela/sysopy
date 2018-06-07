#ifndef ZAD1_CLIENT_H
#define ZAD1_CLIENT_H

#include "commons.h"

int main(int argc, char **argv);

void sigint_handler(int sig, siginfo_t *siginfo, void *context);

void config();

void make_clients();

int try_to_get_haircut();

void clean();

void goto_barber();

#endif //ZAD1_CLIENT_H
