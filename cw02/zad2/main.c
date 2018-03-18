#include "main.h"

char global_relation;
char *global_date;

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

void print_file_data_nftw(char *file_path, struct stat *stat, unsigned int proper_size)
{
    printf("%s ", STR_PATH);
    printf("%s\n", file_path);
    printf("%s ", STR_SIZE);
    printf("%d\n", proper_size);
    printf("%s ", STR_MOD);
    printf("%s\n", make_mods(stat->st_mode));
    printf("%s ", STR_LAST_MODIFICATION);
    printf("%s\n", format_date(&stat->st_mtim));
}

char *format_date(struct timespec *val)
{
    char *str = calloc(36, sizeof(char));

    strftime(str, 36, "%d.%m.%Y %H:%M:%S", localtime((const time_t *) val));
    return str;
}

void list_files(char *directory_name, char *date, char relation)
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
                list_files(new_path, date, relation);
            }

            file = readdir(root);
            free(new_path);
        }
    }
}

void run_standard(char *directory_name, char *date, char relation)
{
    list_files(directory_name, date, relation);
}

void print_help()
{
    printf("%s\n", "Nie podano wszystkich argumentow.");
    printf("%s\n",
           "Nalezy podac tryb {standard|nftw}, sciezke wzgledna / bezwzgledna do katalogu i rozmiar pliku w bajtach.");
    exit(EXIT_SUCCESS);
}

int fun(const char *fpath, const struct stat *stat_buffer, int typeflag, struct FTW *ftwbuf)
{
    char *new_path = malloc(sizeof(char) * 512);
    strcpy(new_path, fpath);

    if( compare_dates(format_date(&stat_buffer->st_mtim), global_date) == global_relation )
    {
        print_file_data_nftw(new_path, stat_buffer, (unsigned int) stat_buffer->st_size);
    }

    return 0;
}

void run_nftw(char *directory_name, char *date, char relation)
{
    nftw(directory_name, fun, 10, 0);
}

int main(int argc, char *argv[])
{
    bool standard = false;
    bool nftw = false;
    char *directory_name = malloc(sizeof(char) * 64);
    char *date = malloc(sizeof(char) * 64);
    char relation = 0;
    global_date = malloc(sizeof(char) * 64);

    int next_option = -1;

    const char *short_opt = "";
    struct option long_opt[] =
            {
                    { "standard",  no_argument,       NULL, 's' },
                    { "nftw",      no_argument,       NULL, 'n' },
                    { "directory", required_argument, NULL, 'd' },
                    { "date",      required_argument, NULL, 'a' },
                    { "relation",  required_argument, NULL, 'r' },
                    { "help",      required_argument, NULL, 'h' }
            };

    do
    {
        next_option = getopt_long(argc, argv, short_opt, long_opt, NULL);

        if( next_option == 's' )
        {
            standard = true;
            nftw = false;
        } else if( next_option == 'n' )
        {
            nftw = true;
            standard = false;
        } else if( next_option == 'd' )
        {
            strcpy(directory_name, optarg);
        } else if( next_option == 'a' )
        {
            strcpy(date, optarg);
            strcpy(global_date, date);
        } else if( next_option == 'r' )
        {
            relation = optarg[0];
            global_relation = relation;
        } else if( next_option == 'h' )
        {
            print_help();
        }

    } while( next_option != -1 );

    if( standard )
    {
        run_standard(directory_name, date, relation);
    } else if( nftw )
    {
        run_nftw(directory_name, date, relation);
    }

    return 0;
}

