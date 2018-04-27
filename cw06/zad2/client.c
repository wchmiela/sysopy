#include "client.h"

void send_msg_wrapper(int queue_server, char buf[])
{
    mq_send(queue_server, buf, MQ_MSGSIZE, 0);
}

void listen(int client_queue, int server_queue, char *client_queue_name)
{
    char buf[MQ_MSGSIZE];
    unsigned int priop;

    while (1) {
        sleep(1);

        ssize_t res = mq_receive(client_queue, buf, MQ_MSGSIZE, &priop);

        if (res > 0) {

            if (buf[0] == TERMINATE_QUEUE) {
                mq_close(server_queue);
                mq_close(client_queue);
                mq_unlink(client_queue_name);
                kill(getppid(), SIGKILL);

                printf("Client: %s\n", buf + 2);

                exit(EXIT_SUCCESS);
            }
            else {
                printf("Wiadomosc z serwera o tresci: %s\n", buf + 2);
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

mqd_t create_client_queue(char *name)
{
    srand(time(NULL));
    name[4] = 0;
    name[0] = '/';
    for (int i = 1; i < 4; i++) {
        name[i] = 97 + rand() % 25;
    }
    struct mq_attr attr;
    attr.mq_flags = MQ_FLAGS;
    attr.mq_maxmsg = MQ_MAXMSG;
    attr.mq_msgsize = MQ_MSGSIZE;
    attr.mq_curmsgs = MQ_CURMSGS;

    mqd_t queue = mq_open(name, O_RDWR | O_CREAT, 0777, &attr);
    if (queue < 0) {
        //blad
        printf("%s\n", name);
        printf("Could't create queue\n");
        exit(EXIT_FAILURE);
    }
    return queue;
}

int main()
{
    printf("%s", START_MSG);

    char client_queue_name[5];

    char *buf = ( char * ) calloc(MQ_CURMSGS, sizeof(char));
    char *buf_input = ( char * ) calloc(MQ_CURMSGS, sizeof(char));

    mqd_t server_queue = mq_open(NAME, O_RDWR);
    if (server_queue < 0) {
        println(MSG_ERROR_CONNECTION);
        exit(EXIT_FAILURE);
    }

    mqd_t client_queue = create_client_queue(client_queue_name);
    if (client_queue < 0) {
        exit(EXIT_FAILURE);
    }


    buf[0] = HANDSHAKE;
    strcpy(buf + 1, client_queue_name);
    if (mq_send(server_queue, buf, MQ_MSGSIZE, 0) == -1) {
        //blad
        exit(EXIT_FAILURE);
    }
    unsigned int priop;
    if (mq_receive(client_queue, buf, MQ_MSGSIZE, &priop) == -1) {
        //blad
        exit(EXIT_FAILURE);
    }
    if (buf[1] == -1) {
        //blad
        exit(EXIT_FAILURE);
    }

    client_id = buf[1];
    println(MSG_CONNECTION_ESTABISHED);

    pid_t listener_pid = fork();
    if (listener_pid == 0)
        listen(client_queue, server_queue, client_queue_name);

    char *str;
    char option = 0;
    println(MSG_READ_OPTION);
    do {
        scanf(" %c", &option);

        str = calloc(( size_t ) MQ_MSGSIZE, sizeof(char));

        switch (option) {
            case '1':
                println(MSG_READ_MESSAGE);
                str = calloc(( size_t ) MQ_MSGSIZE, sizeof(char));

                str[0] = MIRROR;
                str[1] = ( char ) client_id;

                scanf("%s", str + 2);

                send_msg_wrapper(server_queue, str);

                break;

            case '2':
                println(MSG_READ_MESSAGE);
                str = calloc(( size_t ) MQ_MSGSIZE, sizeof(char));

                char temp[2];
                temp[0] = CALC;
                temp[1] = ( char ) client_id;

                scanf("%s", str);
                int op1, op2;
                scanf("%d", &op1);
                scanf("%d", &op2);

                char buffer[MQ_MSGSIZE];
                sprintf(buffer, "%s %s %d %d", temp, str, op1, op2);

                send_msg_wrapper(server_queue, buffer);
                break;

            case '3':
                str[0] = GET_SERVER_TIME;
                str[1] = ( char ) client_id;

                send_msg_wrapper(server_queue, str);
                break;

            case '4':
                str[0] = TERMINATE_SERVER;
                str[1] = ( char ) client_id;

                send_msg_wrapper(server_queue, str);
                break;

            case '5':
                str[0] = TERMINATE_QUEUE;
                str[1] = client_id;

                send_msg_wrapper(server_queue, str);
                goto
                    finish;

            default:
                break;
        }

        option = 0;
    }
    while (1);


    finish:;

    while (1);
}
