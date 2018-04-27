#include "server.h"

void register_client(msgbuf_t buf)
{
    key_t key = atol(buf.mtext);

    int res = msgget(key, 0);

    if (res != -1) {

        clients[clients_count].pid = buf.pid;
        clients[clients_count].res = res;

        sprintf(buf.mtext, "%d", clients_count);

        buf.mtype = buf.pid;
        buf.pid = getpid();

        msgsnd(clients[clients_count].res, &buf, MSGBUF_SIZE, 0);

        clients_count++;

        printf("Zarejestrowano Klienta o PID:%d\n", ( int ) buf.mtype);
    }
    else {
        println(MSG_ERROR_REGISTER_CLIENT);
    }
}

int find_client(int pid)
{
    int queue_id = -1;

    int i;
    for (i = 0; i < clients_count; i++) {
        if (clients[i].pid == pid) {

            queue_id = clients[i].res;
            break;

        }
    }

    return queue_id;
}

bool remove_client(int pid)
{
    int i;
    for (i = 0; i < clients_count; i++) {

        if (clients[i].pid == pid) {
            clients[i].pid = -1;
            return true;
        }

    }
    return false;
}

char *get_time()
{
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    return asctime(timeinfo);
}

void mirror_service(msgbuf_t buf)
{
    int queue_id = find_client(buf.pid);

    if (queue_id != -1) {
        buf.mtype = buf.pid;
        buf.pid = getpid();

        int len = ( int ) strlen(buf.mtext);;
        reverse(buf.mtext, len);
        msgsnd(queue_id, &buf, MSGBUF_SIZE, 0);

        printf("Serwer usluga lustra - pid: %d\n", ( int ) buf.mtype);
    }
    else {
        println(MSG_MISSING_IPC);
    }
}

void get_server_time_service(msgbuf_t msg)
{
    int queue_id = find_client(msg.pid);

    if (queue_id != -1) {
        msg.mtype = msg.pid;
        msg.pid = getpid();

        msg.mtext[0] = 0;

        strcpy(msg.mtext, get_time());

        msgsnd(queue_id, &msg, MSGBUF_SIZE, 0);

        printf("Serwer usluga czasu - pid: %d\n", ( int ) msg.mtype);
    }
    else {
        println(MSG_MISSING_IPC);
    }
}

void calc_service(msgbuf_t buf)
{
    int queue_id = find_client(buf.pid);
    char *buff = malloc(sizeof(char) * 128);
    strcpy(buff, buf.mtext);

    if (queue_id != -1) {
        buf.mtype = buf.pid;
        buf.pid = getpid();
        buf.mtext[0] = 0;

        char *operation = strsep(&buff, " ");
        int op1 = atoi(strsep(&buff, " "));
        int op2 = atoi(buff);
        int result = perform(operation, op1, op2);
        if (result != -1) {
            sprintf(buf.mtext, "%d", result);
        }
        else {
            sprintf(buf.mtext, "%s", "Niepoprawna operacja");
        }

        msgsnd(queue_id, &buf, MSGBUF_SIZE, 0);

        printf("Serwer usluga kalkulatora - pid: %d\n", ( int ) buf.mtype);
    }
    else {
        println(MSG_MISSING_IPC);
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

void terminate_server_service(msgbuf_t buf)
{
    terminator = true;

    int i;
    for (i = 0; i < clients_count; i++) {
        if (clients[i].pid != -1) {

            buf.mtype = TERMINATE_QUEUE;
            buf.pid = getpid();

            msgsnd(clients[i].res, &buf, MSGBUF_SIZE, 0);

            printf("Serwer - zamkniecie kolejki - pid: %d\n", ( int ) clients[i].pid);
        }
    }

    println(MSG_CLOSED_SERVER);
}

void terminate_queue_service(msgbuf_t buf)
{
    int queue_id = find_client(buf.pid);

    remove_client(buf.pid);

    if (queue_id == -1) {

        buf.mtype = TERMINATE_QUEUE;
        buf.pid = getpid();

        msgsnd(queue_id, &buf, MSGBUF_SIZE, 0);

        println(MSG_CLOSED_IPC);
    }
    else {
        println(MSG_MISSING_IPC);
    }
}

bool remove_queue(key_t key)
{
    if (msgctl(key, IPC_RMID, NULL) == 0)
        return true;
    else
        return false;
}

int main(int argc, char *argv[])
{
    char *env_home = getenv(HOME);
    key_t key_server = ftok(env_home, SERVER);

    atexit(( void (*)(void) ) remove_queue(key_server));

    int server_queue = msgget(key_server, IPC_CREAT | 0666);

    if (server_queue < 0) {
        println(MSG_ERROR_CREATE_IPC);
        exit(EXIT_FAILURE);
    }

    msgbuf_t msg;
    println(MSG_CREATED_IPC);

    while (!terminator) {
        sleep(1);

        int res = msgrcv(server_queue, &msg, MSGBUF_SIZE, 0, IPC_NOWAIT);
        if (res >= 0) {

            printf("Wiadomosc: %s\n", msg.mtext);

            switch (msg.mtype) {
                case HANDSHAKE:
                    if (clients_count >= MAX_CLIENTS) {
                        println(MSG_ERROR_TOO_MANY_CLIENTS);
                    }
                    else register_client(msg);
                    break;

                case MIRROR:
                    mirror_service(msg);
                    break;

                case CALC:
                    calc_service(msg);
                    break;

                case GET_SERVER_TIME:
                    get_server_time_service(msg);
                    break;

                case TERMINATE_SERVER:
                    terminate_server_service(msg);
                    break;

                case TERMINATE_QUEUE:
                    terminate_queue_service(msg);
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
        println(MSG_ERROR_MSTCL);
        exit(EXIT_FAILURE);
    }
}
