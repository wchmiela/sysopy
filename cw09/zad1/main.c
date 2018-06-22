#include "main.h"
#include "producer.h"
#include "consumer.h"

pthread_mutex_t *b_mutex;

pthread_cond_t w_cond;

pthread_cond_t r_cond;

int P, K, N, L, nk;

char *config_file_path;

char *file_name;

int search_mode;

int print_mode;

bool finished = false;

pthread_t *p_threads;

pthread_t *k_threads;

FILE *file;

char **buffer;

int production_index = 0;

int consumption_index = 0;

int length_search(int line_length)
{
    return search_mode == (line_length > L ? 1 : line_length < L ? -1 : 0);
}

void *producer(void *pVoid)
{
    int index;
    char line[LINE_MAX];
    while (fgets(line, LINE_MAX, file) != NULL) {
        if (print_mode) {
            printf("Producer[%ld]: Wczytuje linie\n", pthread_self());
        }

        pthread_mutex_lock(&b_mutex[N]);

        while (buffer[production_index] != NULL)
            pthread_cond_wait(&w_cond, &b_mutex[N]);

        index = production_index;
        if (print_mode) {
            printf("Producer[%ld]: Zajmuje index [%d]\n", pthread_self(), index);
        }

        production_index = (production_index + 1) % N;

        pthread_mutex_lock(&b_mutex[index]);

        buffer[index] = malloc((strlen(line) + 1) * sizeof(char));
        strcpy(buffer[index], line);
        if (print_mode) {
            printf("Producer[%ld]: Kopiuje linie do bufora pod indeksem [%d]\n", pthread_self(), index);
        }

        pthread_cond_broadcast(&r_cond);
        pthread_mutex_unlock(&b_mutex[index]);
        pthread_mutex_unlock(&b_mutex[N]);
    }

    if (print_mode) {
        printf("Producer[%ld]: Zakonczono\n", pthread_self());
    }

    return NULL;
}

void *consumer(void *pVoid)
{
    char *line;
    int index;
    while (1) {
        pthread_mutex_lock(&b_mutex[N + 1]);

        while (buffer[consumption_index] == NULL) {
            if (finished) {
                pthread_mutex_unlock(&b_mutex[N + 1]);
                if (print_mode) {
                    printf("Consumer[%ld]: Zakonczono\n", pthread_self());
                }
                return NULL;
            }
            pthread_cond_wait(&r_cond, &b_mutex[N + 1]);
        }

        index = consumption_index;
        if (print_mode) {
            printf("Consumer[%ld]: Zajmuje index [%d]\n", pthread_self(), index);
        }

        consumption_index = (consumption_index + 1) % N;

        pthread_mutex_lock(&b_mutex[index]);
        pthread_mutex_unlock(&b_mutex[N + 1]);

        line = buffer[index];
        buffer[index] = NULL;
        if (print_mode) {
            printf("Consumer[%ld]: Biore linie z bufora o indeksie [%d]\n", pthread_self(), index);
        }

        pthread_cond_broadcast(&w_cond);
        pthread_mutex_unlock(&b_mutex[index]);

        if (length_search(( int ) strlen(line))) {
            if (print_mode) {
                printf("Consumer[%ld]: Znaleziono linie o dlugosci %d %c %d\n", pthread_self(), ( int ) strlen(line),
                       search_mode == 1 ? '>' : search_mode == -1 ? '<' : '=', L);
            }

            printf("Consumer[%ld]: Index(%d), %s", pthread_self(), index, line);
        }

        free(line);
        usleep(10);
    }
}

void start_threads()
{
    if (nk > 0) {
        alarm(( unsigned int ) nk);
    }

    for (int i = 0; i < P; ++i) {
        pthread_create(&p_threads[i], NULL, producer, NULL);
    }

    for (int i = 0; i < K; ++i) {
        pthread_create(&k_threads[i], NULL, consumer, NULL);
    }
}

void join_threads()
{
    for (int p = 0; p < P; ++p)
        pthread_join(p_threads[p], NULL);

    finished = true;
    pthread_cond_broadcast(&r_cond);

    for (int k = 0; k < K; ++k)
        pthread_join(k_threads[k], NULL);
}

void sig_handler(int signo)
{
    printf("Odebrano sygnal: %s. ", signo == SIGINT ? "SIGINT" : "SIGALARM");

    for (int i = 0; i < P; i++) {
        pthread_cancel(p_threads[i]);
    }

    for (int i = 0; i < K; i++) {
        pthread_cancel(k_threads[i]);
    }

    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
    int next_option = -1;

    const char *short_opt = "";
    struct option long_opt[] =
        {
            {"config", required_argument, NULL, 'c'},
            {"help", no_argument, NULL, 'h'}
        };

    do {
        next_option = getopt_long(argc, argv, short_opt, long_opt, NULL);

        if (next_option == 'c') {
            config_file_path = malloc(sizeof(char) * 512);
            strcpy(config_file_path, optarg);
        }
    }
    while (next_option != -1);

    FILE *config_file = fopen(config_file_path, "r");

    if (config_file == NULL) {
        perror("file not exists");
        exit(EXIT_FAILURE);
    }

    int length;
    char *buffer2 = malloc(sizeof(char) * 256);

    getline(&buffer2, &length, config_file);
    P = atoi(strsep(&buffer2, ":"));
    K = atoi(strsep(&buffer2, ":"));
    N = atoi(strsep(&buffer2, ":"));
    file_name = strsep(&buffer2, ":");
    L = atoi(strsep(&buffer2, ":"));
    search_mode = atoi(strsep(&buffer2, ":"));
    print_mode = atoi(strsep(&buffer2, ":"));
    nk = atoi(strsep(&buffer2, ":"));

    printf("%s\n", "Wczytano opcje");
    printf("P:%d K:%d N:%d Nazwa pliku:%s L:%d Tryb wyszukiwania:%d Tryb wypisywania:%d nk:%d\n",
           P, K, N, file_name, L, search_mode, print_mode, nk);

    fclose(config_file);

    file = fopen(file_name, "r");

    if (file == NULL) {
        perror("file not exists");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, sig_handler);
    if (nk > 0) signal(SIGALRM, sig_handler);

    buffer = calloc(( size_t ) N, sizeof(char *));

    b_mutex = malloc((N + 2) * sizeof(pthread_mutex_t));
    for (int i = 0; i < N + 2; ++i)
        pthread_mutex_init(&b_mutex[i], NULL);

    pthread_cond_init(&w_cond, NULL);
    pthread_cond_init(&r_cond, NULL);

    p_threads = malloc(P * sizeof(pthread_t));
    k_threads = malloc(K * sizeof(pthread_t));

    start_threads();

    join_threads();

    if (file != NULL) {
        fclose(file);
    }

    for (int i = 0; i < N; ++i) {
        if (buffer[i]) {
            free(( void * ) buffer[i]);
        }
    }
    free(buffer);

    for (int i = 0; i < N + 2; ++i) {
        pthread_mutex_destroy(&b_mutex[i]);
    }
    free(b_mutex);


    pthread_cond_destroy(&w_cond);
    pthread_cond_destroy(&r_cond);

    return 0;
}
