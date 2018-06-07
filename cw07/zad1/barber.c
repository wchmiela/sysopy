#include "barber.h"

int chairs_count;

int semset_ID;

int shared_mem_ID;

int *shared_mem_addr;

int finish = 0;

int main(int argc, char *argv[])
{
    int next_option;
    do {
        next_option = getopt(argc, argv, "n:");

        if (next_option == 'n') {
            chairs_count = atoi(optarg);
        }
    }
    while (next_option != -1);

    if (chairs_count <= 0) {
        printf("%s\n", "Podano niepoprawne argumenty. Nie podano ilosci krzesel.");
        exit(EXIT_FAILURE);
    }

    config();
    work();

    return 0;
}

void make_haircut(int clientPid)
{
    printf("[%zu] BARBER: Obcinanie %d.\n", get_time(), clientPid);
    give_semaphore(semset_ID, CUT_SEM_NUM);
    printf("[%zu] BARBER: Skonczono obcinanie %d.\n", get_time(), clientPid);
}

void work()
{
    pid_t clientPid;

    printf("[%zu] BARBER: Spi." "\n", get_time());
    while (!finish) {
        take_semaphore(semset_ID, BARBER_SEM_NUM);
        take_semaphore(semset_ID, FIFO_SEM_NUM);
        clientPid = bqueue_get_customer_from_chair(shared_mem_addr);
        give_semaphore(semset_ID, FIFO_SEM_NUM);

        make_haircut(clientPid);

        while (1) {
            take_semaphore(semset_ID, FIFO_SEM_NUM);
//            bqueue_show(shared_mem_addr);
            clientPid = bqueue_get(shared_mem_addr);

            if (clientPid != -1) {
                bqueue_occupy_chair(shared_mem_addr, clientPid);
                make_haircut(clientPid);
                give_semaphore(semset_ID, FIFO_SEM_NUM);
            }
            else {
                printf("[%zu] BARBER: Spi.\n", get_time());
                take_semaphore(semset_ID, BARBER_SEM_NUM);
                give_semaphore(semset_ID, FIFO_SEM_NUM);
                break;
            }
        }
    }

    clean();
}

void config()
{
    atexit(clean);

    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = &sigint_handler;
    act.sa_flags = SA_SIGINFO;
    if (sigaction(SIGINT, &act, NULL) == -1) {
        printf("sigaction error\n");
        exit(EXIT_FAILURE);
    }

    // initialize semaphores and shared memory
    key_t key = ftok(getenv(HOME), SEMG_KEY);

    semset_ID = semget(key, 3, IPC_CREAT | 0600);
    if (semset_ID == -1) {
        perror("semget error");
        exit(EXIT_FAILURE);
    }

    int err = semctl(semset_ID, BARBER_SEM_NUM, SETVAL, 0)
        + semctl(semset_ID, FIFO_SEM_NUM, SETVAL, 1)
        + semctl(semset_ID, CUT_SEM_NUM, SETVAL, 0);

    if (err != 0) {
        perror("Wystapil blad w inicjalizacji semaforow");
        exit(EXIT_FAILURE);
    }

    // get id of shared memory
    shared_mem_ID = shmget(key, (chairs_count + 4) * sizeof(pid_t), IPC_CREAT | 0600);
    if (shared_mem_ID == -1) {
        printf("shmget error\n");
        exit(EXIT_FAILURE);
    }

    // include memory segment to process address space
    shared_mem_addr = shmat(shared_mem_ID, NULL, 0);
    if (shared_mem_addr == ( void * ) -1) {
        printf("shmat error\n");
        exit(EXIT_FAILURE);
    }

    bqueue_init(shared_mem_addr, chairs_count);
}

void sigint_handler(int sig, siginfo_t *siginfo, void *context)
{
    printf("%s\n", "Otrzymano SIGINT. Zamkniecie zakladu fryzjerskiego.");
    finish = 1;
}

void clean()
{
    if (shmdt(shared_mem_addr) == -1) {
        perror("Wystapil blad w odlaczeniu segmentu");
    }

    if (shmctl(shared_mem_ID, IPC_RMID, NULL) == -1) {
        perror("Wystapil blad w usuwaniu pamieci wspolnej z systemu.");
    }

    if (semctl(semset_ID, 0, IPC_RMID) == -1) {
        perror("Wystapil blad w usuwaniu zbioru semaforow.");
    }
}
