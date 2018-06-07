#include "main.h"

int threads_count;

char *input_filename;

char *def_filename;

char *output_filename;

int W, H, M;

int **matrix;

double **filter_matrix;

int filter_matrix_size;

int **result_matrix;

double system_start;

double system_last;

double user_start;

double user_last;

int main(int argc, char *argv[])
{
    threads_count = -1;
    input_filename = malloc(256 * sizeof(char));
    def_filename = malloc(256 * sizeof(char));
    output_filename = malloc(256 * sizeof(char));

    int next_option = -1;

    const char *short_opt = "n:";
    struct option long_opt[] =
        {
            {"input", required_argument, NULL, 'i'},
            {"def", required_argument, NULL, 'd'},
            {"output", required_argument, NULL, 'o'}
        };

    do {
        next_option = getopt_long(argc, argv, short_opt, long_opt, NULL);

        if (next_option == 'n') {
            threads_count = atoi(optarg);
        }
        else if (next_option == 'i') {
            strcpy(input_filename, optarg);
        }
        else if (next_option == 'd') {
            strcpy(def_filename, optarg);
        }
        else if (next_option == 'o') {
            strcpy(output_filename, optarg);
        }

    }
    while (next_option != -1);

    FILE *input_file = fopen(input_filename, "r");
    int def_file = open(def_filename, O_CREAT | O_TRUNC | O_RDWR);
    int output_file = open(output_filename, O_CREAT | O_RDWR);

    if (threads_count <= 0 || input_file == NULL) {
        printf("%s\n", "Podano niepoprawne parametry");
        printf("%s",
               "PRZYKLAD\n"
               "-n \t\t\t[ILOSC WATKOW]\n"
               "--input \t[PLIK WEJSCIOWY]\n"
               "--def \t\t[PLIK Z DEFINICJAMI]\n"
               "--output \t[PLIK WYJSCIOWY]\n");
        exit(EXIT_FAILURE);
    }

    make_random_def_file(def_file);

    char *buffer = malloc(sizeof(char) * 256);
    size_t length;

    char *picture_name = malloc(sizeof(char) * 256);
    char *comment = malloc(sizeof(char) * 256);

    getline(&buffer, &length, input_file);
    strcpy(picture_name, buffer);

    //omit next line with comment
    getline(&buffer, &length, input_file);
    strcpy(comment, buffer);

    //read next line with W i H
    getline(&buffer, &length, input_file);
    W = atoi(strsep(&buffer, " "));
    H = atoi(strsep(&buffer, " "));

    //read next line with M
    getline(&buffer, &length, input_file);
    M = atoi(buffer);

    matrix = ( int ** ) calloc(( size_t ) H, sizeof(int *));
    for (int i = 0; i < H; ++i) {
        matrix[i] = ( int * ) calloc(( size_t ) W, sizeof(int));
    }

    int i = 0;
    while ((getline(&buffer, &length, input_file)) != -1) {
        int j = 0;
        for (int k = 0; k < strlen(buffer);) {
            char a = buffer[k];
            char b = ' ';
            if (k + 1 < strlen(buffer)) {
                b = buffer[k + 1];
            }
            char c = ' ';
            if (k + 2 < strlen(buffer)) {
                c = buffer[k + 2];
            }

            if ((a - 48) >= 0 && ((a - 48) <= 9)) {
                k++;
                int val = a - 48;
                if ((b - 48) >= 0 && ((b - 48) <= 9)) {
                    k++;
                    val = val * 10 + (b - 48);
                    if ((c - 48) >= 0 && ((c - 48) <= 9)) {
                        k++;
                        val = val * 10 + (c - 48);
                    }
                }
                matrix[i][j++] = val;
            }
            else {
                k++;
            }
        }

        i++;
    }

    fclose(input_file);
    close(def_file);

    result_matrix = ( int ** ) calloc(( size_t ) H, sizeof(int *));
    for (i = 0; i < H; ++i) {
        result_matrix[i] = ( int * ) calloc(( size_t ) W, sizeof(int));
    }

    pthread_t *threads = malloc(sizeof(pthread_t) * threads_count);

    set_times();
    printf("==Ilosc watkow: %d==\n", threads_count);

    for (i = 0; i < threads_count; ++i) {
        int *j = malloc(sizeof(*j));
        *j = i;

        pthread_create(&threads[i], NULL, thread, j);
    }

    for (i = 0; i < threads_count; i++) {
        pthread_join(threads[i], NULL);
    }

    update_times();

    save_picture(picture_name, comment, output_file);

    for (i = 0; i < H; ++i) {
        free(matrix[i]);
    }
    free(matrix);

    return 0;
}

void *thread(void *i)
{
    int ci = *(( int * ) i);
    free(i);

    int start = ci * W / threads_count;
    int end = (ci + 1) * W / threads_count;
    // [start, end)

    for (int j = start; j < end; ++j) {
        for (int k = 0; k < H; ++k) {
            result_matrix[k][j] = calculate_pixel(j, k);
        }
    }

    return NULL;
}

int calculate_pixel(int x, int y)
{
    double px = 0;
    for (int j = 0; j < filter_matrix_size; j++) {
        int b = ( int ) round(fmax(0, y - ceil(( double ) filter_matrix_size / 2) + j));
        b = b < H ? b : H - 1;
        for (int i = 0; i < filter_matrix_size; i++) {
            int a = ( int ) round(fmax(0, x - ceil(( double ) filter_matrix_size / 2) + i));
            a = a < W ? a : W - 1;
            double value = matrix[b][a] * filter_matrix[j][i];
            px += value;
        }
    }
    return ( int ) round(px);
}

void print_matrix(int **matrix, int H, int W)
{
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

void save_picture(char *picture_name, char *comment, int output_file)
{
    char *buffer = malloc(sizeof(char) * 256);

    strcpy(buffer, picture_name);
    write(output_file, buffer, strlen(buffer));

    strcpy(buffer, comment);
    write(output_file, buffer, strlen(buffer));

    sprintf(buffer, "%d %d\n", W, H);
    write(output_file, buffer, strlen(buffer));

    sprintf(buffer, "%d\n", M);
    write(output_file, buffer, strlen(buffer));

    for (int i = 0; i < H; ++i) {

        buffer = calloc(256, sizeof(char));
        char *buffer2 = calloc(64, sizeof(char));
        for (int j = 0; j < W; ++j) {
            sprintf(buffer2, "%d ", result_matrix[i][j]);
            strcat(buffer, buffer2);
        }

        strcat(buffer, "\n");

        write(output_file, buffer, strlen(buffer));
        free(buffer2);
    }

    free(buffer);
}

void make_random_def_file(int fd)
{
//    srand(( unsigned int ) time(NULL));
    int a = 3;
    int b = 65;
    int random = (rand() % (b - a)) + a;
    filter_matrix_size = random;

    filter_matrix = malloc(sizeof(double) * random);
    for (int i = 0; i < random; ++i) {
        filter_matrix[i] = malloc(sizeof(double) * random);
    }

    char *buffer = malloc(sizeof(char) * 128);

    sprintf(buffer, "%d\n", random);
    write(fd, buffer, strlen(buffer));
    buffer = malloc(sizeof(char) * random);

    for (int i = 0; i < random; ++i) {
        char *buffer2 = malloc(sizeof(char) * random);

        for (int j = 0; j < random; ++j) {
            double val = ( double ) 1 / (( double ) random * random);

            filter_matrix[i][j] = val;

            sprintf(buffer2, "%f ", val);
            strcat(buffer, buffer2);
        }

        strcat(buffer, "\n");

        write(fd, buffer, strlen(buffer));
        buffer = malloc(sizeof(char) * random);
    }

    free(buffer);
}

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

    double user = buffer.tms_utime / ( double ) CLOCKS_PER_SEC;
    double sys = buffer.tms_stime / ( double ) CLOCKS_PER_SEC;

    user_start = user;
    user_last = user;

    system_start = sys;
    system_last = sys;
}

void update_times()
{
    struct tms buffer;
    times(&buffer);

    double user = buffer.tms_utime / ( double ) CLOCKS_PER_SEC;
    double sys = buffer.tms_stime / ( double ) CLOCKS_PER_SEC;

    print_test(user, sys);

    system_last = sys;
    user_last = user;
}