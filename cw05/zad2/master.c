#include "master.h"

void print_help()
{
    printf("%s\n", "Podaj sciezke do potoku");
    printf("%s\n", "--file [NAZWA POTOKU]");
}

int main(int argc, char *argv[])
{
    char *pipe_name = malloc(sizeof(char) * 256);

    int next_option = -1;
    const char *short_opt = "";
    struct option long_opt[] =
        {
            {"file", required_argument, NULL, 'f'},
            {"help", no_argument, NULL, 'h'}
        };

    do {
        next_option = getopt_long(argc, argv, short_opt, long_opt, NULL);

        if (next_option == 'f') {
            strcpy(pipe_name, optarg);
        }
        else if (next_option == 'h') {
            print_help();
            exit(EXIT_SUCCESS);
        }

    }
    while (next_option != -1);

    unlink(( const char * ) pipe_name);

    if (mkfifo(( const char * ) pipe_name, 0777) == -1) {
        printf("%s\n", "Blad z utworzeniem potoku");
        exit(EXIT_FAILURE);
    }

    int file = open(pipe_name, O_RDONLY | O_NONBLOCK);
    if (file == -1) {
        printf("%s\n", "Blad z otwarciem pliku");
        exit(EXIT_FAILURE);
    }

    char *buffer = malloc(sizeof(char) * 256);
    bool flag = false;
    while (1) {
        int res = ( int ) read(file, buffer, 256);
        if (res > 0) {
            flag = true;
            write(STDOUT_FILENO, buffer, strlen(buffer));
        }

        if (res == 0 && flag) break;
    }

    free(buffer);
    close(file);
}
