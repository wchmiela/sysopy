#ifndef CW06_CONSTS_H_H
#define CW06_CONSTS_H_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <memory.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <assert.h>

#define MAX_CLIENTS 10

#define SERVER 0
#define CLIENT 1

#define STATEMENT_LENGTH 64

#define MIRROR 001
#define CALC 002
#define GET_SERVER_TIME 003
#define TERMINATE_SERVER 004
#define HANDSHAKE 005
#define TERMINATE_QUEUE 006

typedef struct msgbuf_t
{
    long mtype;                       /* message type, must be > 0 */
    pid_t pid;
    char mtext[STATEMENT_LENGTH];    /* message data */
} msgbuf_t;

int MSGBUF_SIZE = sizeof(msgbuf_t) - sizeof(long);

typedef struct client_info
{
    int pid;
    int res;
} client_info_t;

static const char *HOME = "HOME";

static const char *MSG_ERROR_CREATE_IPC = "Blad w tworzeniu kolejki komunikatow";

static const char *MSG_ERROR_CONNECTION = "Blad w komunikacji miedzy klientem a serwerem.";

static const char *MSG_ERROR_TOO_MANY_CLIENTS = "Nie mozna zarejestrowac wiecej klientow.";

static const char *MSG_ERROR_REGISTER_CLIENT = "Wystapil blad w rejestracji klienta.";

static const char *MSG_ERROR_MSTCL = "Wystapil blad z funkcja mstcl";

static const char *MSG_ERROR_ARGS = "Podano niepoprawne argumenty.";

static const char *MSG_CREATED_IPC = "Utworzono kolejke komunikatow";

static const char *MSG_CONNECTION_ESTABISHED = "Serwer potwierdzil polaczenie";

static const char *MSG_CONNECTION_ERROR = "Blad w komunikacji miedzy klientem a serwerem.";

static const char *MSG_SEND_MESSAGE = "Wystapil blad w wyslaniu wiadomosci.";

static const char *MSG_READ_MESSAGE = "Wczytaj wiadomosc";

static const char *MSG_READ_OPTION = "Wybierz opcje (1|2|3|4|5)";

static const char *MSG_CLOSED_IPC = "Zamknieto kolejke IPC";

static const char *MSG_MISSING_IPC = "Kolejka IPC nie istnieje";

static const char *MSG_CLOSED_SERVER = "Zamknieto kolejke IPC";

static const char *START_MSG = "Wybierz jedna z opcji\n"
                               "\t[1]Usluga lustra\n"
                               "\t[2]Usluga kalkulatora (add|sub|mul|div)\n"
                               "\t[3]Usluga czasu\n"
                               "\t[4]Nakaz zakonczenia\n"
                               "\t[5]Zamknij program\n";

static const char *OPERATION_ADD = "ADD";

static const char *OPERATION_SUB = "SUB";

static const char *OPERATION_MUL = "MUL";

static const char *OPERATION_DIV = "DIV";

void println(const char *str)
{
    printf("%s\n", str);
}

void to_upper(char *A, size_t size)
{
    assert(A != NULL);

    int i;
    for (i = 0; i < size; i++) {
        if (A[i] >= 97 && A[i] <= 122) {
            A[i] -= 32;
        }
    }
}

void reverse(char *A, int size)
{
    char *B = calloc(size, sizeof(char));

    int i;
    for (i = size - 1; i >= 0; i--) {
        B[size - i - 1] = A[i];
    }

    for (int i = 0; i < size; ++i) {
        A[i] = B[i];
    }

    free(B);
}

#endif //CW06_CONSTS_H_H
