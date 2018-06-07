#include "client.h"

int clients_count = -1;

int client_cuts = -1;

int semset_ID;

int shared_mem_ID;

int *shared_mem_addr;

int cuts_counter = 0;

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

    config();
    make_clients();

    return 0;
}

void sigint_handler(int sig, siginfo_t *siginfo, void *context)
{
    printf("Otrzymano SIGINT. Zamkniecie klienta.\n");
    clean();
    exit(EXIT_SUCCESS);
}

void config()
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

    key_t key = ftok(getenv(HOME), SEMG_KEY);

    semset_ID = semget(key, 0, 0);
    if (semset_ID == -1) {
        perror("semget error");
        exit(EXIT_FAILURE);
    }

    // get id of shared memory
    int smid = shmget(key, 0, 0);
    if (smid == -1) {
        perror("shmget error");
        exit(EXIT_FAILURE);
    }

    // include memory segment to process address space
    shared_mem_addr = shmat(smid, NULL, 0);
    if (shared_mem_addr == ( void * ) -1) {
        perror("shmat error");
        exit(EXIT_FAILURE);
    }
}

void make_clients()
{
    int clientsCounter = 0, status;

    for (int i = 0; i < clients_count; i++) {
        if (fork() == 0) {
            goto_barber();
            _exit(EXIT_SUCCESS);
        }
    }

    while (clientsCounter < clients_count) {
        wait(&status);
        clientsCounter++;

        if (status == EXIT_SUCCESS) {
            // printf("pid: %d\n", p);
        }
        else {
            printf("child error");
            exit(EXIT_SUCCESS);
        }
    }
}

int try_to_get_haircut()
{
    take_semaphore(semset_ID, FIFO_SEM_NUM);
    int status;
    int barberSemVal = semctl(semset_ID, BARBER_SEM_NUM, GETVAL);
    if (barberSemVal == -1) {
        perror("semctl error");
        exit(EXIT_FAILURE);
    }

    if (barberSemVal == 0) {
        printf("[%zu] CLIENT (%d): Budzenie fryzjera.", get_time(), getpid());
        give_semaphore(semset_ID, BARBER_SEM_NUM);
        give_semaphore(semset_ID, BARBER_SEM_NUM);
        bqueue_occupy_chair(shared_mem_addr, getpid());
        status = 0;
    }
    else {

        if (bqueue_put(shared_mem_addr, getpid()) == -1) {
            printf("[%zu] CLIENT (%d): Kolejka jest pelna.\n", get_time(), getpid());
            status = -1;
        }
        else {
            printf("[%zu] CLIENT (%d): Barber pracuje. Zajmuje miejsce w kolejce.\n", get_time(), getpid());
            status = 0;
        }
    }

    give_semaphore(semset_ID, FIFO_SEM_NUM);
    return status;
}

void clean()
{
    if (shmdt(shared_mem_addr) == -1) {
        perror("Blad w odlaczeniu bloku pamieci");
        exit(EXIT_FAILURE);
    }
}

void goto_barber()
{
    while (cuts_counter < client_cuts) {
        int status = try_to_get_haircut();

        if (status == 0) {
            take_semaphore(semset_ID, CUT_SEM_NUM);

            ++cuts_counter;
            printf("[%zu] CLIENT (%d): Obcinanie.\n", get_time(), getpid());
        }
    }

    clean();
}