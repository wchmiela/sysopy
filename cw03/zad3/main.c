#include "main.h"

int set_limit(int resource, rlim_t value)
{
    struct rlimit limit;
    limit.rlim_cur = value;
    limit.rlim_max = value;

    return setrlimit(resource, &limit);
}

void print_usage_report()
{
    struct rusage s_rusage;
    getrusage(RUSAGE_CHILDREN, &s_rusage);

    printf("user:\t %f [s]\t\t system: %f [s]\t\t \n",
           s_rusage.ru_utime.tv_sec + s_rusage.ru_utime.tv_usec / 1e6,
           s_rusage.ru_stime.tv_sec + s_rusage.ru_stime.tv_usec / 1e6);
}

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
    printf("%s\n", "--file nazwa_pliku");
    printf("%s\n", "--cpulimit limit_czasu_procesora");
    printf("%s\n", "--memlimit limit_rozmiar_pamieci");
}

void execute(char *program_name, char *args, rlim_t cpulimit, rlim_t memlimit)
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

    int status;
    int pid = fork();

    if( pid == 0 )
    {
        if( set_limit(RLIMIT_CPU, cpulimit) != 0 || set_limit(RLIMIT_AS, memlimit) != 0 )
        {
            printf("%s\n", MSG_SET_LIMIT_ERROR);
            exit(EXIT_FAILURE);
        }

        int result = execvp(buf[0], buf);
        if( result == -1 )
        {
            exit(EXIT_FAILURE);
        }
    } else
    {
        waitpid(pid, &status, 0);
        print_usage_report();

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
    rlim_t cpulimit = 0;
    rlim_t memlimit = 0;

    const char *short_opt = "";
    struct option long_opt[] =
            {
                    { "file",     required_argument, NULL, 'f' },
                    { "help",     no_argument,       NULL, 'h' },
                    { "cpulimit", required_argument, NULL, 'c' },
                    { "memlimit", required_argument, NULL, 'm' },
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
        } else if( next_option == 'c' )
        {
            cpulimit = (rlim_t) atoi(optarg);
        } else if( next_option == 'm' )
        {
            memlimit = (rlim_t) atoi(optarg);
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

        execute(program_name, args, cpulimit, memlimit);
    }

    fclose(file);

    exit(EXIT_SUCCESS);
}
