#ifndef CW06_SERVER_H
#define CW06_SERVER_H

#include "consts.h"

//variables
client_info_t clients[MAX_CLIENTS];

int clients_count;

bool terminator = false;

//declarations

void register_client(msgbuf_t buf);

int find_client(int pid);

bool remove_client(int pid);

char *get_time();

void mirror_service(msgbuf_t buf);

void get_server_time_service(msgbuf_t msg);

void calc_service(msgbuf_t msgbuf);

int perform(char *operation, int op1, int op2);

void terminate_server_service(msgbuf_t buf);

void terminate_queue_service(msgbuf_t buf);

bool remove_queue(key_t key);

int main(int argc, char *argv[]);

#endif //CW06_SERVER_H
