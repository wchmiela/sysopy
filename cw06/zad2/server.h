#ifndef CW06_SERVER_H
#define CW06_SERVER_H

#include "consts.h"


//variables
client_info_t clients[MAX_CLIENTS];

int clients_count;

bool terminator = false;

mqd_t server_queue;

//declarations

void register_client(char *buffer);

char *get_time();

void echo_service(char *buff);

void get_server_time_service(char *buf);

void to_upper_case_service(char *buf);

void terminate_server_service(char *buf);

void terminate_queue_service(char *buf);

bool remove_queue(key_t key);

int main(int argc, char *argv[]);

#endif //CW06_SERVER_H
