#ifndef CW06_CLIENT_H
#define CW06_CLIENT_H

#include "consts.h"

int server_queue;

int client_queue;

void send_handshake(int queue_server, int key);

void receive_handshake(int queue_server);

void send_msg_wrapper(int queue_server, int mtype, char mtext[]);

bool send_msg(int queue_server, int mtype, char mtext[]);

void listen(int client_queue, pid_t msg_id);

void clean();

int main(int argc, char *argv[]);

#endif //CW06_CLIENT_H
