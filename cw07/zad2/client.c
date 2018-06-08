#include "client.h"

int clients_count = -1;

int client_cuts = -1;

int cuts_counter = 0;

sem_t *semarr[3];

int *smaddr;

int smsize;

int main(int argc, char *argv[])
{
    int next_option;
    do {
        next_option = getopt(argc, argv, "n:m:");

        if (next_option == 'n') {
            clients_count = atoi(optarg);
        }
        else if (next_option == 'm') {
            client_cuts = atoi(optarg);
        }
    }
    while (next_option != -1);

    if (clients_count <= 0 || client_cuts <= 0) {
        printf("%s\n", "Podano niepoprawne argumenty. Nie podano ilosci krzesel.");
        exit(EXIT_FAILURE);
    }

    configure();
    make_clients();

    return 0;
}

void sigint_handler(int sig, siginfo_t *siginfo, void *context)
{
    printf("Otrzymano SIGINT. Zamkniecie klienta.\n");
    clean();
    exit(EXIT_SUCCESS);
}

void configure()
{
    // handle signals
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = &sigint_handler;
    act.sa_flags = SA_SIGINFO;
    if (sigaction(SIGINT, &act, NULL) == -1) {
        perror("sigaction error\n");
        exit(EXIT_FAILURE);
    }

    semarr[BARBER_SEM_NUM] = sem_open(BARBER_NAME, O_RDWR, 0600, 0);
    semarr[CUT_SEM_NUM] = sem_open(CUT_NAME, O_RDWR, 0600, 0);
    semarr[FIFO_SEM_NUM] = sem_open(FIFO_NAME, O_RDWR, 0600, 0);

    if (semarr[BARBER_SEM_NUM] == SEM_FAILED ||
        semarr[FIFO_SEM_NUM] == SEM_FAILED ||
        semarr[CUT_SEM_NUM] == SEM_FAILED) {
        perror("sem_open error: cannot retrieve semaphore");
        exit(EXIT_FAILURE);
    }

    // get id of shared memory
    int smid = shm_open(MEMORY_NAME, O_RDWR, 0600);
    if (smid == -1) {
        perror("shm_open error");
        exit(EXIT_FAILURE);
    }

    smaddr = mmap(NULL, sizeof(int), PROT_READ, MAP_SHARED, smid, 0);
    if (smaddr == ( void * ) -1) {
        perror("mmap error");
        exit(EXIT_FAILURE);
    }

    smsize = smaddr[0];

    if (munmap(smaddr, sizeof(int)) == -1) {
        perror("munmap error");
        exit(EXIT_FAILURE);
    }

    smaddr = mmap(NULL, smsize * sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, smid, 0);
    if (smaddr == ( void * ) -1) {
        perror("mmap error");
        exit(EXIT_FAILURE);
    }
}

void make_clients()
{
    int clientsCounter = 0, status;

    for (int i = 0; i < clients_count; i++) {
        if (fork() == 0) {
            goto_barber();
            exit(EXIT_SUCCESS);
        }
    }

    while (clientsCounter < clients_count) {
        wait(&status);
        clientsCounter++;

        if (status != EXIT_SUCCESS) {
            printf("%s\n", "child error");
            exit(EXIT_SUCCESS);
        }
    }
}

int try_to_get_haircut()
{
    sem_wait(semarr[FIFO_SEM_NUM]);
    int status;
    int barberSemVal;

    sem_getvalue(semarr[BARBER_SEM_NUM], &barberSemVal);

    if (barberSemVal == -1) {
        perror("semctl error");
        exit(EXIT_FAILURE);
    }

    if (barberSemVal == 0) {
        printf("[%zu] CLIENT (%d): Budzenie fryzjera.", get_time(), getpid());
        sem_post(semarr[BARBER_SEM_NUM]);
        sem_post(semarr[BARBER_SEM_NUM]);
        bqueue_occupy_chair(smaddr, getpid());
        status = 0;
    }
    else {

        if (bqueue_put(smaddr, getpid()) == -1) {
            printf("[%zu] CLIENT (%d): Kolejka jest pelna.\n", get_time(), getpid());
            status = -1;
        }
        else {
            printf("[%zu] CLIENT (%d): Barber pracuje. Zajmuje miejsce w kolejce.\n", get_time(), getpid());
            status = 0;
        }
    }

    sem_post(semarr[FIFO_SEM_NUM]);
    return status;
}

void clean()
{
    if (sem_close(semarr[BARBER_SEM_NUM]) == -1 ||
        sem_close(semarr[FIFO_SEM_NUM]) == -1 ||
        sem_close(semarr[CUT_SEM_NUM]) == -1) {
        printf("sem_close error\n");
        exit(EXIT_FAILURE);
    }

    if (munmap(smaddr, smsize * sizeof(int)) == -1) {
        printf("munmap error\n");
        exit(EXIT_FAILURE);
    }
}

void goto_barber()
{
    while (cuts_counter < client_cuts) {
        int status = try_to_get_haircut();

        if (status == 0) {
            sem_wait(semarr[CUT_SEM_NUM]);

            ++cuts_counter;
            printf("[%zu] CLIENT (%d): Obcinanie.\n", get_time(), getpid());
        }
    }

    clean();
}