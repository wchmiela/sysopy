#include "main.h"

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

sem_t *b_sem;

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

        sem_wait(&b_sem[N]);
        sem_wait(&b_sem[N + 2]);

        index = production_index;
        if (print_mode) {
            printf("Producer[%ld]: Zajmuje index [%d]\n", pthread_self(), index);
        }

        production_index = (production_index + 1) % N;

        sem_wait(&b_sem[index]);
        sem_post(&b_sem[N]);

        buffer[index] = malloc((strlen(line) + 1) * sizeof(char));
        strcpy(buffer[index], line);
        if (print_mode) {
            printf("Producer[%ld]: Kopiuje linie do bufora pod indeksem [%d]\n", pthread_self(), index);
        }

        sem_post(&b_sem[index]);
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
        sem_wait(&b_sem[N + 1]);
        while (buffer[consumption_index] == NULL) {
            sem_post(&b_sem[N + 1]);
            if (finished) {
                if (print_mode) {
                    printf("Consumer[%ld]: Zakonczono\n", pthread_self());
                }
                return NULL;
            }
            sem_wait(&b_sem[N + 1]);
        }

        index = consumption_index;
        if (print_mode) {
            printf("Consumer[%ld]: Zajmuje index [%d]\n", pthread_self(), index);
        }

        consumption_index = (consumption_index + 1) % N;

        sem_wait(&b_sem[index]);

        line = buffer[index];
        buffer[index] = NULL;
        if (print_mode) {
            printf("Consumer[%ld]: Biore linie z bufora o indeksie [%d]\n", pthread_self(), index);
        }

        sem_post(&b_sem[N + 2]);
        sem_post(&b_sem[N + 1]);
        sem_post(&b_sem[index]);

        if (length_search(( int ) strlen(line))) {
            if (print_mode) {
                printf("Consumer[%ld]: Znaleziono linie o dlugosci %d %c %d\n", pthread_self(), ( int ) strlen(line),
                       search_mode == 1 ? '>' : search_mode == -1 ? '<' : '=', L);
            }

            printf("Consumer[%ld]: Index(%d), %s", pthread_self(), index, line);
        }
        free(line);
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

    file = fopen(file_name, "r");

    if (file == NULL) {
        perror("file not exists");
        exit(EXIT_FAILURE);
    }

    buffer = calloc(( size_t ) N, sizeof(char *));

    b_sem = malloc((N + 3) * sizeof(sem_t));
    for (int i = 0; i < N + 2; ++i) {
        sem_init(&b_sem[i], 0, 1);
    }
    sem_init(&b_sem[N + 2], 0, ( unsigned int ) N);

    p_threads = malloc(P * sizeof(pthread_t));
    k_threads = malloc(K * sizeof(pthread_t));

    start_threads();

    join_threads();

    if (file != NULL) {
        fclose(file);
    }

    for (int i = 0; i < N; ++i) {
        if (buffer[i]) free(buffer[i]);
    }
    free(buffer);

    for (int i = 0; i < N + 4; ++i) {
        sem_destroy(&b_sem[i]);
    }
    free(b_sem);

    return 0;
}
