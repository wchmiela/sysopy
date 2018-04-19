#include "main.h"

void fix(char *t)
{
    int len = ( int ) strlen(t);

    if (t[len - 1] == ' ' || t[len - 1] == '\n') {
        t[len - 1] = '\0';
    }
}

void print_help()
{
    printf("%s\n", "--file [Nazwa Pliku]");
}

int main(int argc, char *argv[])
{
    char *file_name = malloc(sizeof(char) * 256);

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
            strcpy(file_name, optarg);
        }
        else if (next_option == 'h') {
            print_help();
            exit(EXIT_SUCCESS);
        }

    }
    while (next_option != -1);

    FILE *file = fopen(file_name, "r+");
    if (file == NULL) {
        printf("%s\n", MSG_OPENING_FAILED);
        exit(EXIT_FAILURE);
    }
    char *buffer = malloc(256 * sizeof(char));

    char *line = malloc(256 * sizeof(char));

    while (fgets(buffer, 256, file)) {
        int x = 0;
        char **list = malloc(sizeof(char *) * 256);

        char *buffer_copy = malloc(256 * sizeof(char));
        strcat(buffer_copy, buffer);

        int k = 0;
        while (buffer_copy != NULL) {
            char *t = strsep(&buffer_copy, "|");
            fix(t);

            if (t[0] == ' ') {
                char *r = strchr(t, ' ');
                strcpy(r, t + 1);
                list[k] = malloc(sizeof(char) * 256);
                strcpy(list[k++], r);
            }
            else {
                list[k] = malloc(sizeof(char) * 256);
                strcpy(list[k++], t);
            }
        }


        int fd[2];

        pipe(fd);

        int indesc = STDIN_FILENO;
        int outdesc = fd[1];

        for (char *c = list[x++]; c != NULL; c = list[x++]) {

            strcpy(line, c);

            char *prog_name = malloc(sizeof(char) * 256);
            char *line_copy = malloc(sizeof(char) * 256);
            strcpy(line_copy, line);

            strcpy(prog_name, strsep(&line_copy, " "));

            static char *buf[256];
            for (int z = 0; z < 256; ++z) {
                buf[z] = NULL;
            }
            buf[0] = prog_name;

            if (strlen(line_copy) > 0) {
                int i = 1;
                while (1) {
                    buf[i] = malloc(sizeof(char) * 64);
                    strcpy(buf[i++], strsep(&line_copy, " "));

                    if (line_copy == NULL) {
                        break;
                    }
                }

                buf[i] = NULL;
            }

            if (list[x] != NULL) {

                int pid = fork();

                if (pid < 0) {
                    printf("%s\n", MSG_FORK_ERROR);
                    exit(EXIT_FAILURE);
                }

                if (pid == 0) {
                    dup2(indesc, STDIN_FILENO);
                    close(indesc);

                    dup2(outdesc, STDOUT_FILENO);
                    close(outdesc);

                    if (execvp(buf[0], buf) < 0) {
                        printf("%s\n", MSG_RUN_ERROR);
                        exit(EXIT_FAILURE);
                    }

                }
                else {
                    int ret;
                    waitpid(pid, &ret, 0);
                }

            }
            else {

                int pid = fork();

                if (pid < 0) {
                    printf("%s\n", MSG_FORK_ERROR);
                    exit(EXIT_FAILURE);
                }

                if (pid == 0) {
                    dup2(indesc, STDIN_FILENO);
                    close(indesc);

                    if (execvp(buf[0], buf) < 0) {
                        printf("%s\n", MSG_RUN_ERROR);
                        exit(EXIT_FAILURE);
                    }

                }
                else {
                    int ret;
                    waitpid(pid, &ret, 0);
                }

            }

            close(fd[1]);
            indesc = fd[0];
        }
    }

    return 0;
}
