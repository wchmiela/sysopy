#include "main.h"

double system_start;
double system_last;

double user_start;
double user_last;

void print_test(double user_time, double system_time)
{
    printf("user:\t %f [s]\t\t delta_start: %f [s]\t\t delta_last %f [s]\n",
           user_time, user_time - user_start, user_time - user_last);

    printf("system:\t %f [s]\t\t delta_start: %f [s]\t\t delta_last %f [s]\n",
           system_time, system_time - system_start, system_time - system_last);
}

void set_times()
{
    printf("%s\n", "---Mierzenie czasów---");

    struct tms buffer;
    times(&buffer);

    double user = buffer.tms_utime / (double) CLOCKS_PER_SEC;
    double sys = buffer.tms_stime / (double) CLOCKS_PER_SEC;

    user_start = user;
    user_last = user;

    system_start = sys;
    system_last = sys;
}

void update_times()
{
    struct tms buffer;
    times(&buffer);

    double user = buffer.tms_utime / (double) CLOCKS_PER_SEC;
    double sys = buffer.tms_stime / (double) CLOCKS_PER_SEC;

    print_test(user, sys);

    system_last = sys;
    user_last = user;
}

void print_help()
{
    printf("%s\n", "Opcje");
    printf("%s\n", "--generate : Generuj plik z n blokow o m rozmiarze");
    printf("%s\n", "--sort : Posortuj plik");
    printf("%s\n", "--copy : Kopiuj plik");
    printf("%s\n", "--name : Nazwa pliku wejsciowego");
    printf("%s\n", "--target : Nazwa pliku wyjsciowego");
    printf("%s\n", "-n : Liczba blokow");
    printf("%s\n", "-m : Rozmiar blokow");
}

void sys_generate(const char *file_name, const int n, const int m)
{
    int file_dev_random = open(DEV_RANDOM, O_RDONLY);
    int file = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);

    if( file_dev_random == -1 || file == -1 )
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char *buffer = malloc(sizeof(unsigned char) * m);
    for( int i = 0; i < n; ++i )
    {
        if( read(file_dev_random, buffer, (size_t) m) == -1 )
        {
            perror("Error reading file");
            exit(EXIT_FAILURE);
        }

        if( write(file, buffer, (size_t) m) == -1 )
        {
            perror("Error writing file");
            exit(EXIT_FAILURE);
        }
    }

    free(buffer);

    close(file);
    close(file_dev_random);
}

void sys_sort(const char *file_name, const int records_count, const int record_size)
{
    int file = open(file_name, O_RDWR);

    if( file == -1 )
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char *buffer1 = malloc(sizeof(unsigned char) * record_size);
    char *buffer2 = malloc(sizeof(unsigned char) * record_size);

    for( int i = 1; i < records_count; ++i )
    {
        lseek(file, i * record_size, SEEK_SET);
        read(file, buffer1, (size_t) record_size);
        int j = i - 1;

        while( j >= 0 )
        {
            lseek(file, j * record_size, SEEK_SET);
            read(file, buffer2, (size_t) record_size);

            if( buffer1[0] < buffer2[0] )
            {
                lseek(file, (j + 1) * record_size, SEEK_SET);
                write(file, buffer2, (size_t) record_size);

                j--;
            } else
            {
                break;
            }
        }
        write(file, buffer1, (size_t) record_size);
    }

    free(buffer2);
    free(buffer1);

    close(file);
}

void lib_sort(const char *file_name, const int records_count, const int record_size)
{
    FILE *file = fopen(file_name, "r+");

    if( file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char *buffer1 = malloc(sizeof(unsigned char) * record_size);
    char *buffer2 = malloc(sizeof(unsigned char) * record_size);

    for( int i = 1; i < records_count; ++i )
    {
        fseek(file, i * record_size, SEEK_SET);
        fread(buffer1, sizeof(unsigned char), (size_t) record_size, file);
        int j = i - 1;

        while( j >= 0 )
        {
            fseek(file, j * record_size, SEEK_SET);
            fread(buffer2, sizeof(unsigned char), (size_t) record_size, file);

            if( buffer1[0] < buffer2[0] )
            {
                fseek(file, (j + 1) * record_size, SEEK_SET);
                fwrite(buffer2, sizeof(unsigned char), (size_t) record_size, file);

                j--;
            } else
            {
                break;
            }
        }
        fwrite(buffer1, sizeof(unsigned char), (size_t) record_size, file);
    }

    free(buffer2);
    free(buffer1);

    fclose(file);
}

void lib_copy(char *file_name, char *target_file_name, int records_count, int record_size)
{
    FILE *source = fopen(file_name, "r");
    FILE *target = fopen(target_file_name, "w+");

    if( source == NULL || target == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char *buffer = malloc(sizeof(unsigned char) * record_size);

    for( int i = 0; i < records_count; ++i )
    {
        if( fread(buffer, sizeof(unsigned char), (size_t) record_size, source) == -1 )
        {
            perror("Error reading file");
            exit(EXIT_FAILURE);
        }
        if( fwrite(buffer, sizeof(unsigned char), (size_t) record_size, target) == -1 )
        {
            perror("Error writing to file");
            exit(EXIT_FAILURE);
        }
    }

    free(buffer);
}

void sys_copy(char *file_name, char *target_file_name, int records_count, int record_size)
{
    int source = open(file_name, O_RDONLY);
    int target = open(target_file_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);

    if( source == -1 || target == -1 )
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char *buffer = malloc(sizeof(unsigned char) * record_size);

    for( int i = 0; i < records_count; ++i )
    {
        if( read(source, buffer, (size_t) record_size) == 1 )
        {
            perror("Error reading file");
            exit(EXIT_FAILURE);
        }
        if( write(target, buffer, (size_t) record_size) == -1 )
        {
            perror("Error writing to file");
            exit(EXIT_FAILURE);
        }
    }

    free(buffer);
}

int main(int argc, char *argv[])
{
    int records_count = -1;
    int record_size = -1;
    bool sys = false;
    bool lib = false;
    bool generate = false;
    bool sort = false;
    bool copy = false;
    bool perform_tests = false;

    char *file_name = malloc(sizeof(unsigned char) * 64);
    char *target_file_name = malloc(sizeof(unsigned char) * 64);

    const char *short_opt = "n:m:";
    struct option long_opt[] =
            {
                    { "lib",      no_argument,       NULL, 'l' },
                    { "sys",      no_argument,       NULL, 's' },
                    { "generate", no_argument,       NULL, 'g' },
                    { "sort",     no_argument,       NULL, 'o' },
                    { "copy",     no_argument,       NULL, 'c' },
                    { "name",     required_argument, NULL, 'f' },
                    { "target",   required_argument, NULL, 't' },
                    { "help",     no_argument,       NULL, 'h' },
                    { "test",     no_argument,       NULL, 'e' }
            };

    int next_option;
    do
    {
        next_option = getopt_long(argc, argv, short_opt, long_opt, NULL);

        if( next_option == 'n' )
        {
            records_count = atoi(optarg);
        } else if( next_option == 'm' )
        {
            record_size = atoi(optarg);
        } else if( next_option == 's' )
        {
            sys = true;
        } else if( next_option == 'l' )
        {
            lib = true;
        } else if( next_option == 'g' )
        {
            generate = true;
        } else if( next_option == 'o' )
        {
            sort = true;
        } else if( next_option == 'c' )
        {
            copy = true;
        } else if( next_option == 't' )
        {
            strcpy(target_file_name, optarg);
        } else if( next_option == 'f' )
        {
            strcpy(file_name, optarg);
        } else if( next_option == 'h' )
        {
            print_help();
            exit(EXIT_SUCCESS);
        } else if( next_option == 'e' )
        {
            perform_tests = true;
        }
    } while( next_option != -1 );

    if( records_count <= 0 || record_size <= 0 || strlen(file_name) == 0 || (copy && strlen(target_file_name) == 0))
    {
        perror("Invalid args");
        exit(EXIT_FAILURE);
    }

    if( generate )
    {
        sys_generate(file_name, records_count, record_size);
    }
    if( sort )
    {
        if( perform_tests ) set_times();
        if( sys )
        {
            sys_sort(file_name, records_count, record_size);
        } else if( lib )
        {
            lib_sort(file_name, records_count, record_size);
        }
        if( perform_tests ) update_times();
    }
    if( copy )
    {
        if( perform_tests ) set_times();
        if( sys )
        {
            sys_copy(file_name, target_file_name, records_count, record_size);
        } else if( lib )
        {
            lib_copy(file_name, target_file_name, records_count, record_size);
        }
        if( perform_tests ) update_times();
    }

    return 0;
}