#include "client.h"

key_t key_atexit;

void send_handshake(int queue_server, int key)
{
    char buf[MSGBUF_SIZE];
    sprintf(buf, "%d", key);

    send_msg_wrapper(queue_server, HANDSHAKE, buf);
}

void receive_handshake(int queue_server)
{
    sleep(1);

    msgbuf_t msg;
    int res = ( int ) msgrcv(queue_server, &msg, MSGBUF_SIZE, 0, IPC_NOWAIT);

    if (res >= 0) {
        println(MSG_CONNECTION_ESTABISHED);
    }
    else {
        println(MSG_CONNECTION_ERROR);

        exit(EXIT_FAILURE);
    }
}

void send_msg_wrapper(int queue_server, int mtype, char mtext[])
{
    if (send_msg(queue_server, mtype, mtext)) {

    }
    else {
        println(MSG_SEND_MESSAGE);
    }
}

bool send_msg(int queue_server, int mtype, char mtext[])
{
    msgbuf_t msg;
    msg.mtype = mtype;
    msg.pid = getpid();

    strcpy(msg.mtext, mtext);

    if (msgsnd(queue_server, &msg, MSGBUF_SIZE, 0) == -1) {
        return false;
    }

    return true;
}

void listen(int client_queue, pid_t msg_id)
{
    while (1) {
        sleep(1);

        msgbuf_t msg;
        int res = msgrcv(client_queue, &msg, MSGBUF_SIZE, 0, IPC_NOWAIT);

        if (res >= 0) {

            if (msg.mtype == msg_id) {

                printf("Wiadomosc z serwera o tresci: %s\n", msg.mtext);

            }
            else {
                clean();
            }
        }
    }
}

void clean()
{
    kill(getppid(), SIGKILL);

    if (msgctl(client_queue, IPC_RMID, NULL) != 0) {
        println(MSG_ERROR_MSTCL);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

void handler(int signo)
{
    if (signo == SIGINT) {
        exit(0);
    }
}

void remove_queue_2(void)
{
    if (msgctl(key_atexit, IPC_RMID, NULL) == 0) {
        printf("Usunieto kolejke!\n");
    }
    else {
        printf("Blad usuwania kolejki!\n");
    }
}

bool remove_queue(key_t key)
{
    if (msgctl(key, IPC_RMID, NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

int main(int argc, char *argv[])
{
    atexit(remove_queue_2);

    printf("%s", START_MSG);

    pid_t client_pid = getpid();
    char *env_home = getenv(HOME);

    key_t key_server = ftok(env_home, SERVER);
    key_t key_client = ftok(env_home, ( char ) client_pid);

    server_queue = msgget(key_server, 0);

    if (server_queue < 0) {
        println(MSG_ERROR_CONNECTION);
        exit(EXIT_FAILURE);
    }

    client_queue = msgget(key_client, IPC_CREAT | 0666);
    if (client_queue < 0) {
        exit(EXIT_FAILURE);
    }

    key_atexit = client_queue;

    signal(SIGINT, handler);

    send_handshake(server_queue, key_client);
    receive_handshake(client_queue);

    pid_t listener_pid = fork();
    if (listener_pid == 0) listen(client_queue, client_pid);

    char *str;
    char option = 0;
    println(MSG_READ_OPTION);
    do {
        scanf(" %c", &option);

        str = calloc(( size_t ) MSGBUF_SIZE, sizeof(char));

        switch (option) {
            case '1':
                println(MSG_READ_MESSAGE);

                str = calloc(( size_t ) MSGBUF_SIZE, sizeof(char));
                scanf("%s", str);

                send_msg_wrapper(server_queue, MIRROR, str);
                break;

            case '2':
                println(MSG_READ_MESSAGE);

                str = calloc(( size_t ) MSGBUF_SIZE, sizeof(char));
                scanf("%s", str);
                int op1, op2;
                scanf("%d", &op1);
                scanf("%d", &op2);

                char buffer[1000];
                sprintf(buffer, "%s %d %d", str, op1, op2);

                send_msg_wrapper(server_queue, CALC, buffer);
                break;

            case '3':
                send_msg_wrapper(server_queue, GET_SERVER_TIME, str);
                break;

            case '4':
                send_msg_wrapper(server_queue, TERMINATE_SERVER, str);
                break;

            case '5':
                send_msg_wrapper(server_queue, TERMINATE_QUEUE, str);
                goto finish;

            default:
                break;
        }

        option = 0;
    }
    while (1);

    finish:;

    while (1);
}
