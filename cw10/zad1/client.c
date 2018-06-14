#include "client.h"

int socket_desc;

char *client_name;

char *ipv4address;

int port;

char *socket_path;

int main(int argc, char *argv[])
{
    socket_desc = -1;
    client_name = NULL;
    ipv4address = NULL;
    socket_path = NULL;

    int next_option = -1;

    const char *short_opt = "p:";
    struct option long_opt[] =
        {
            {"name", required_argument, NULL, 'n'},
            {"address", required_argument, NULL, 'a'},
            {"spath", required_argument, NULL, 's'},
            {"help", no_argument, NULL, 'h'}
        };

    do {
        next_option = getopt_long(argc, argv, short_opt, long_opt, NULL);

        if (next_option == 'p') {
            port = atoi(optarg);
        }
        else if (next_option == 'n') {
            client_name = malloc(sizeof(char) * 256);
            strcpy(client_name, optarg);
        }
        else if (next_option == 'a') {
            ipv4address = malloc(sizeof(char) * 256);
            strcpy(ipv4address, optarg);
        }
        else if (next_option == 's') {
            socket_path = malloc(sizeof(char) * 256);
            strcpy(socket_path, optarg);
        }
        else if (next_option == 'h') {
            printf("%s\n", "Przykladowe uruchomienia");
            printf("%s\n", "./client --name [NAME] --address [IPv4 ADDRESS] -p [PORT]");
            printf("%s\n", "./client --name [NAME] --spath [UNIX SOCKET PATH]");
        }

    }
    while (next_option != -1);

    signal(SIGINT, &clear_and_exit);

    if (client_name != NULL && socket_path != NULL) {
        local_loop(client_name, socket_path);
    }
    else if (client_name != NULL && ipv4address != NULL && port != -1) {
        net_loop(client_name, ipv4address, port);
    }

    return 0;
}

void local_loop(char *client_name, char *socket_path)
{
    socket_desc = socket(AF_UNIX, SOCK_STREAM, 0);

    if (socket_desc == -1) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un unix_addr;
    unix_addr.sun_family = AF_UNIX;
    strcpy(unix_addr.sun_path, socket_path);

    if (connect(socket_desc, ( struct sockaddr * ) &unix_addr, sizeof(unix_addr)) == -1) {
        perror("connect error");
        exit(EXIT_FAILURE);
    }

    receive_compute_send_loop(client_name);
}

void net_loop(char *client_name, char *ipv4address, int port)
{
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        perror("create socket error");
        exit(EXIT_FAILURE);
    }

    struct in_addr sin_addr;
    struct sockaddr_in inet_addr;
    inet_addr.sin_family = AF_INET;
    inet_addr.sin_port = ( in_port_t ) port;

    inet_pton(AF_INET, ipv4address, &inet_addr.sin_addr);

    if (connect(socket_desc, ( struct sockaddr * ) &inet_addr, sizeof(inet_addr)) == -1) {
        perror("connect error");
        exit(EXIT_FAILURE);
    }

    fflush(stdout);
    receive_compute_send_loop(client_name);
}

void receive_compute_send_loop(char *client_name)
{
    long int result;
    char *buf = ( char * ) calloc(NAME_SIZE_MAX, sizeof(char));
    char sign[5];
    sign[1] = '+';
    sign[2] = '-';
    sign[3] = '*';
    sign[4] = '/';
    sprintf(buf, "%c%c%s", OP_SEND_NAME, ( int ) sizeof(client_name), client_name);
    fflush(stdout);

    if (write(socket_desc, ( void * ) buf, strlen(client_name) + 2) == -1) {
        printf("Sending name err - %s\n", strerror(errno));
    }
    fflush(stdout);

    while (1) {
        free(buf);
        buf = ( char * ) calloc(NAME_SIZE_MAX, sizeof(char));

        if (read(socket_desc, ( void * ) buf, NAME_SIZE_MAX) == -1) {
            perror("read error");
        }

        if (buf[0] == OP_EXIT) {
            printf("[EXIT] Terminating client\n");
            break;
        }

        if (buf[0] == OP_REJECT) {
            printf("[REJECT] This name is occupied\n");
            break;
        }

        if (buf[0] == OP_PING) {
            printf("[PING]\n");
            write(socket_desc, ( void * ) buf, 1);
            continue;
        }

        //0 - operation// 1 - id // 2 - len1 // 3 - len2
        int a = atoi(buf + 4);
        int b = atoi(buf + 5 + buf[2]);
        result = -1;
        if (buf[0] == OP_ADD) result = a + b;
        if (buf[0] == OP_SUB) result = a - b;
        if (buf[0] == OP_MUL) result = a * b;
        if (buf[0] == OP_DIV) {
            if (b == 0) {
                printf("[%d] FLOATING POINT EXCEPTION\n", buf[1]);
                buf[0] = OP_FPE;
                buf[2] = 0;
                write(socket_desc, ( void * ) buf, 3);
                continue;
            }
            result = a / b;
        }
        if (result == -1) {
            //printf("[UNKNOWN] Client error unknown msg_id - %d - ignoring\n", buf[0]);
            continue;
        }
        else {
            printf("[%d] %d %c %d = %ld\n", buf[1], a, sign[buf[0]], b, result);
        }
        buf[0] = OP_RES;
        sprintf(buf + 3, "%ld", result);
        buf[2] = strlen(buf + 3);


        if (write(socket_desc, ( void * ) buf, strlen(buf + 3) + 3) == -1) {
            perror("write error");
        }
    }

    free(buf);

    if (shutdown(socket_desc, SHUT_RDWR) == -1) {
        perror("shutdown error");
    }
    if (close(socket_desc) == -1) {
        perror("close error");
    }
}
void clear_and_exit()
{
    char buf[2];
    buf[0] = OP_EXIT;
    buf[1] = 0;

    if (write(socket_desc, ( void * ) buf, 2) == -1) {
        perror("write error");
    }
    if (shutdown(socket_desc, SHUT_RDWR) == -1) {
        perror("shutdown error");
    }
    if (close(socket_desc) == -1) {
        perror("close error");
    }

    exit(EXIT_SUCCESS);
}