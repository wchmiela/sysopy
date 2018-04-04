#include "main.h"

int number_from_char(char c)
{
    return c - 48;
}

char compare_dates(char *date1, char *date2)
{
    int d1 = number_from_char(date1[0]) * 10 + number_from_char(date1[1]);
    int d2 = number_from_char(date2[0]) * 10 + number_from_char(date2[1]);

    int m1 = number_from_char(date1[3]) * 10 + number_from_char(date1[4]);
    int m2 = number_from_char(date2[3]) * 10 + number_from_char(date2[4]);

    int y1 = number_from_char(date1[6]) * 1000 + number_from_char(date1[7]) * 100 + number_from_char(date1[8]) * 10 +
             number_from_char(date1[9]);
    int y2 = number_from_char(date2[6]) * 1000 + number_from_char(date2[7]) * 100 + number_from_char(date2[8]) * 10 +
             number_from_char(date2[9]);

    if( d1 == d2 && m1 == m2 && y1 == y2 ) return '=';
    if( y1 > y2 ) return '>';
    if( y1 == y2 && m1 > m2 ) return '>';
    if( y1 == y2 && m1 == m2 && d1 > d2 ) return '>';

    return '<';
}

void print_file_data(char *file_path, struct stat stat)
{
    printf("%s ", STR_PATH);
    printf("%s\n", file_path);
    printf("%s ", STR_SIZE);
    printf("%d\n", (int) stat.st_size);
    printf("%s ", STR_MOD);
    printf("%s\n", make_mods(stat.st_mode));
    printf("%s ", STR_LAST_MODIFICATION);
    printf("%s\n", format_date(&stat.st_mtim));
}

char *make_mods(__mode_t mode)
{
    char *str = calloc(11, sizeof(char));

    strcat(str, S_ISDIR(mode) ? "d" : "-");
    strcat(str, mode & S_IRUSR ? "r" : "-");
    strcat(str, mode & S_IWUSR ? "w" : "-");
    strcat(str, mode & S_IXUSR ? "x" : "-");
    strcat(str, mode & S_IRGRP ? "r" : "-");
    strcat(str, mode & S_IWGRP ? "w" : "-");
    strcat(str, mode & S_IXGRP ? "x" : "-");
    strcat(str, mode & S_IROTH ? "r" : "-");
    strcat(str, mode & S_IWOTH ? "w" : "-");
    strcat(str, mode & S_IXOTH ? "x" : "-");

    return str;
}

char *format_date(struct timespec *val)
{
    char *str = calloc(36, sizeof(char));

    strftime(str, 36, "%d.%m.%Y %H:%M:%S", localtime((const time_t *) val));
    return str;
}

void list_files(char *directory_name, char *date, char relation, int argc, char *argv[])
{
    DIR *root = opendir(directory_name);

    if( root != NULL)
    {
        struct dirent *file = readdir(root);

        while( file != NULL)
        {
            if( strcmp(file->d_name, CMP_DOT) == 0 || strcmp(file->d_name, CMP_DOT_DOT) == 0 )
            {
                file = readdir(root);
                continue;
            }

            char *new_path = malloc(sizeof(char) * 512);
            strcpy(new_path, directory_name);
            strcat(new_path, file->d_name);

            struct stat stat_buffer;

            if( stat(new_path, &stat_buffer) < 0 )
            {
                printf("Dla pliku : %s %s\n", new_path, "Wystapil blad przy przetwarzaniu szczegolow");
            }

            if( compare_dates(format_date(&stat_buffer.st_mtim), date) == relation )
            {
                print_file_data(new_path, stat_buffer);
            }

            if( S_ISDIR(stat_buffer.st_mode))
            {
                strcat(new_path, "/");

                int pid = fork();
                if( pid < 0 )
                {
                    perror("Blad przy forkowaniu procesu");
                    exit(EXIT_FAILURE);
                }

                if( pid == 0 )
                {
                    //podmiana directory
                    for( int i = 0; i < argc; ++i )
                    {
                        if( strcmp(argv[i], "--directory") == 0 )
                        {
                            if( i + 1 < argc )
                            {
                                argv[i + 1] = calloc(64, sizeof(char));
                                strcpy(argv[i + 1], new_path);
                                break;
                            }
                        }
                    }

                    if( execvp("./zad1", argv) < 0 )
                    {
                        perror("Blad przy wykonaniu nowego programu");
                        exit(EXIT_FAILURE);
                    }
                } else
                {
                    int status;
                    waitpid(pid, &status, 0);
                    if( status != 0 )
                    {
                        exit(EXIT_FAILURE);
                    }
                }
            }

            file = readdir(root);
            free(new_path);
        }
    }
}

void run_standard(char *directory_name, char *date, char relation, int argc, char *argv[])
{
    list_files(directory_name, date, relation, argc, argv);
}

void print_help()
{
    printf("%s\n", "--standard");
    printf("%s\n", "--directory - nazwa katalogu");
    printf("%s\n", "--date - data graniczna");
    printf("%s\n", "--relation - relacja {<,>,=}");
    printf("%s\n", "--help");
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    bool standard = false;
    char *directory_name = malloc(sizeof(char) * 64);
    char *date = malloc(sizeof(char) * 64);
    char relation = 0;

    int next_option = -1;

    const char *short_opt = "";
    struct option long_opt[] =
            {
                    { "standard",  no_argument,       NULL, 's' },
                    { "directory", required_argument, NULL, 'd' },
                    { "date",      required_argument, NULL, 'a' },
                    { "relation",  required_argument, NULL, 'r' },
                    { "help",      no_argument,       NULL, 'h' }
            };

    do
    {
        next_option = getopt_long(argc, argv, short_opt, long_opt, NULL);

        if( next_option == 's' )
        {
            standard = true;
        } else if( next_option == 'd' )
        {
            strcpy(directory_name, optarg);
        } else if( next_option == 'a' )
        {
            strcpy(date, optarg);
        } else if( next_option == 'r' )
        {
            relation = optarg[0];
        } else if( next_option == 'h' )
        {
            print_help();
        }

    } while( next_option != -1 );

    if( standard )
    {
        run_standard(directory_name, date, relation, argc, argv);
    }

    return 0;
}

