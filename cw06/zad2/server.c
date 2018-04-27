#include "server.h"

void register_client(char *buf)
{
    mqd_t queue = mq_open(buf + 1, O_RDWR);

    buf[1] = ( char ) clients_count;
    buf[2] = 0;

    clients[clients_count].id = queue;

    if (mq_send(queue, buf, MQ_MSGSIZE, 0) != -1) {
        printf("Zarejestrowano Klienta o ID:%d\n", clients[clients_count].id);
        clients_count++;
    }
    else {
        println(MSG_ERROR_REGISTER_CLIENT);
    }
}

char *get_time()
{
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    return asctime(timeinfo);
}

void echo_service(char *buf)
{
    if (mq_send(clients[( int ) buf[1]].id, buf, MQ_MSGSIZE, 0) != -1) {
        printf("Usluga echa - Client o ID: %d\n", clients[buf[1]].id);
    }
}

void get_server_time_service(char *buf)
{
    strcpy(buf + 2, get_time());

    if (mq_send(clients[( int ) buf[1]].id, buf, MQ_MSGSIZE, 0) != -1) {
        printf("Usluga czasu - Client o ID: %d\n", clients[( int ) buf[1]].id);
    }
}

void mirror_service(char *buf)
{
    if (buf[1] == 0) {
        int len = ( int ) strlen(buf + 2);
        reverse(buf + 2, len);
    }
    else {
        int len = ( int ) strlen(buf);
        reverse(buf, len);
    }

    if (mq_send(clients[( int ) buf[1]].id, buf, MQ_MSGSIZE, 0) != -1) {
        printf("Serwer usluga lustra - Client o ID: %d\n", clients[( int ) buf[1]].id);
    }
}

int perform(char *operation, int op1, int op2)
{
    to_upper(operation, strlen(operation));
    if (strcmp(OPERATION_ADD, operation) == 0) {
        return op1 + op2;
    }
    else if (strcmp(OPERATION_SUB, operation) == 0) {
        return op1 - op2;
    }
    else if (strcmp(OPERATION_MUL, operation) == 0) {
        return op1 * op2;
    }
    else if (strcmp(OPERATION_DIV, operation) == 0) {
        return op1 / op2;
    }
    else {
        return -1;
    }
}

void calc_service(char *buf)
{
    char *buff = malloc(sizeof(char) * MQ_MSGSIZE);
    strcpy(buff, buf + 2);

    char *operation = strsep(&buff, " ");
    int op1 = atoi(strsep(&buff, " "));
    int op2 = atoi(buff);
    int result = perform(operation, op1, op2);
//    char buffer[32];
//    if (result != -1) {
//        sprintf(buffer, "%d", result);
//    }
//    else {
//        sprintf(buffer, "%s", "Niepoprawna operacja");
//    }
//    //"\002 3"
//    "\003"
    strcpy(buf + 2, "4");

    if (mq_send(clients[( int ) buf[1]].id, buf, MQ_MSGSIZE, 0) != -1) {
        printf("Serwer usluga kalkulatora - Client o ID: %d\n", clients[( int ) buf[1]].id);
    }
    else {
        perror("dadada");
    }
}

void terminate_server_service(char *buf)
{
    terminator = true;

    buf[0] = TERMINATE_QUEUE;

    int i;
    for (i = 0; i < clients_count; i++) {
        if (clients[i].id != -1) {
            if (mq_send(clients[i].id, buf, MQ_MSGSIZE, 0) != -1) {
                printf("Rozlaczono klienta o ID: %d\n", i);
                mq_close(clients[i].id);
            }
        }
    }

    println(MSG_CLOSED_SERVER);
    mq_close(server_queue);
    mq_unlink(NAME);
}

void terminate_queue_service(char *buf)
{

    if (mq_send(clients[( int ) buf[1]].id, buf, MQ_MSGSIZE, 0) != -1) {
        mq_close(clients[( int ) buf[1]].id);
        printf("Rozlaczono klienta o ID: %d\n", buf[1]);

        clients[buf[1]].id = -1;
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

mqd_t create_queue(char *name)
{
    struct mq_attr attr;

    attr.mq_flags = MQ_FLAGS;
    attr.mq_maxmsg = MQ_MAXMSG;
    attr.mq_msgsize = MQ_MSGSIZE;
    attr.mq_curmsgs = MQ_CURMSGS;

    mode_t omask;
    omask = umask(0);
    mqd_t queue = mq_open(name, O_RDONLY | O_CREAT, (S_IRWXU | S_IRWXG | S_IRWXO), &attr);
    umask(omask);

    if (queue < 0) {
        println(MSG_ERROR_CREATE_QUEUE);
        exit(EXIT_FAILURE);
    }

    return queue;
}

int main(int argc, char *argv[])
{
    server_queue = create_queue(( char * ) NAME);

    if (server_queue < 0) {
        println(MSG_ERROR_CREATE_IPC);
        exit(EXIT_FAILURE);
    }

    unsigned int priop;
    char buffor[MQ_MSGSIZE];

    println(MSG_CREATED_QUEUE);

    while (!terminator) {
        sleep(1);

        ssize_t res = mq_receive(server_queue, buffor, MQ_MSGSIZE, &priop);

        if (res > 0) {

            switch (buffor[0]) {
                case HANDSHAKE:
                    if (clients_count >= MAX_CLIENTS) {
                        println(MSG_ERROR_TOO_MANY_CLIENTS);
                    }
                    else {
                        register_client(buffor);
                    }
                    break;

                case MIRROR:
                    mirror_service(buffor);
                    break;

                case CALC:
                    calc_service(buffor);
                    break;

                case GET_SERVER_TIME:
                    get_server_time_service(buffor);
                    break;

                case TERMINATE_SERVER:
                    terminate_server_service(buffor);
                    break;

                case TERMINATE_QUEUE:
                    terminate_queue_service(buffor);
                    break;

                default:
                    break;
            }

        }
    }

    if (remove_queue(server_queue)) {
        exit(EXIT_SUCCESS);
    }
    else {
        exit(EXIT_FAILURE);
    }
}
