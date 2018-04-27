#ifndef CW06_CLIENT_H
#define CW06_CLIENT_H

#include "consts.h"

int server_queue;

int client_queue;

int client_id;

void send_handshake(int queue_server, int key);

void receive_handshake(int queue_server);

void send_msg_wrapper(int queue_server, char buf[]);

bool send_msg(int queue_server, int mtype, char mtext[]);

void listen(int client_queue, int server_queue, char *client_queue_name);

void clean();

int main();

#endif //CW06_CLIENT_H
