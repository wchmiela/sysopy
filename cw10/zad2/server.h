#ifndef ZAD1_SERVER_H
#define ZAD1_SERVER_H

#define UNIX_PATH_MAX 108
#define CLIENTS_MAX 200
#define EVENTS_MAX 20
#define NAME_SIZE_MAX 128
#define OP_SEND_NAME 11 //client -> server
#define OP_ADD 1
#define OP_SUB 2 //1-4 server -> client
#define OP_MUL 3
#define OP_DIV 4
#define OP_RES 5 //client -> server
#define OP_EXIT 6
#define OP_REJECT 7 //server -> client
#define OP_PING 8
#define OP_FPE 9
#define OP_ID 12

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <memory.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <signal.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <getopt.h>

void clear_and_exit();

void *listen_socket(void *useless);

void *ping_clients(void *useless);

#endif //ZAD1_SERVER_H
