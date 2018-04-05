#include "main.h"

void remove_endline(char *str)
{
    int len = (int) strlen(str);

    if( str[len - 1] == '\n' )
    {
        str[len - 1] = '\0';
    }
}

void print_help()
{
    printf("%s\n", "--help");
    printf("%s\n", "--file nazwa pliku");
}

void execute(char *program_name, char *args)
{
    char *buffer = malloc(sizeof(char) * 256);
    strcpy(buffer, args);

    static char *buf[256];
    buf[0] = program_name;

    if( strlen(buffer) > 0 )
    {
        int i = 1;
        while( 1 )
        {
            buf[i] = malloc(sizeof(char) * 64);
            strcpy(buf[i++], strsep(&buffer, " "));

            if( buffer == NULL)
            {
                break;
            }
        }

        buf[i] = NULL;
    }

    remove_endline(buf[0]);

    int pid = fork();
    if( pid == 0 )
    {
        int result = execvp(buf[0], buf);
        if( result == -1 )
        {
            exit(EXIT_FAILURE);
        }
    } else
    {
        int status;
        waitpid(pid, &status, 0);
        if( status != 0 )
        {
            printf("%s - %s\n", program_name, MSG_RUN_ERROR);
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[])
{
    if( argc == 1 )
    {
        printf("%s\n", MSG_NO_ARGS);
        exit(EXIT_FAILURE);
    }

    char *file_name = malloc(sizeof(char) * 128);
    int next_option = -1;

    const char *short_opt = "";
    struct option long_opt[] =
            {
                    { "file", required_argument, NULL, 'f' },
                    { "help", no_argument,       NULL, 'h' }
            };

    do
    {
        next_option = getopt_long(argc, argv, short_opt, long_opt, NULL);

        if( next_option == 'f' )
        {
            strcpy(file_name, optarg);
        } else if( next_option == 'h' )
        {
            print_help();
            exit(EXIT_SUCCESS);
        }
    } while( next_option != -1 );

    FILE *file = fopen(file_name, "r");
    if( file == NULL)
    {
        printf("%s\n", MSG_OPENING_FAILED);
        exit(EXIT_FAILURE);
    }

    char buffer[255];

    while( fgets(buffer, 255, file))
    {
        char *program_name = calloc(sizeof(char), 256);
        char *args = calloc(sizeof(char), 256);

        char *buff = malloc(256 * sizeof(char));
        strcpy(buff, buffer);

        strcpy(program_name, strsep(&buff, " "));

        if( buff != NULL)
        {
            strcpy(args, strsep(&buff, "\n"));
        }

        execute(program_name, args);
    }

    fclose(file);

    exit(EXIT_SUCCESS);
}
