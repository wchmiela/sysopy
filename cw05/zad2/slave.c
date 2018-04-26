#include "slave.h"

void print_help()
{
    printf("%s\n", "Podaj sciezke do potoku");
    printf("%s\n", "--file [NAZWA POTOKU]");
    printf("%s\n", "-n [LICZBA NATURALNA]");
}

int main(int argc, char *argv[])
{
    char *pipe_name = malloc(sizeof(char) * 256);
    int n;

    int next_option = -1;
    const char *short_opt = "n:";
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
        else if (next_option == 'n') {
            n = atoi(optarg);
        }
        else if (next_option == 'h') {
            print_help();
            exit(EXIT_SUCCESS);
        }
    }
    while (next_option != -1);

    int file = open(pipe_name, O_WRONLY);
    if (file == -1) {
        printf("%s\n", "Blad z otwarciem pliku");
        exit(EXIT_FAILURE);
    }

    printf("%s - %d\n", "Moj PID", getpid());

    char *buffer1 = malloc(sizeof(char) * 256);
    char *buffer2 = malloc(sizeof(char) * 256);
    for (int i = 0; i < n; ++i) {
        FILE *date = popen("date", "r");

        fgets(buffer1, 256, date);
        sprintf(buffer2, "%d %s", getpid(), buffer1);

        write(file, buffer2, strlen(buffer2));
        sleep(( unsigned int ) (rand() % (5 - 2) + 2));
    }

    free(buffer2);
    free(buffer1);

    close(file);
}
