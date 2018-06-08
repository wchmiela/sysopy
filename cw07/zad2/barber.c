#include "barber.h"

int chairs_count;

int finish = 0;

pid_t *smaddr;

sem_t *semarr[3];

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

    configure();
    work();

    return 0;
}

void make_haircut(int clientPid)
{
    printf("[%zu] BARBER: Obcinanie %d.\n", get_time(), clientPid);
    sem_post(semarr[CUT_SEM_NUM]);
    printf("[%zu] BARBER: Skonczono obcinanie %d.\n", get_time(), clientPid);
}

void work()
{
    pid_t clientPid;

    printf("[%zu] BARBER: Spi." "\n", get_time());
    while (!finish) {
        sem_wait(semarr[BARBER_SEM_NUM]);
        sem_wait(semarr[FIFO_SEM_NUM]);
        clientPid = bqueue_get_customer_from_chair(smaddr);
        sem_post(semarr[FIFO_SEM_NUM]);

        make_haircut(clientPid);

        while (1) {
            sem_wait(semarr[FIFO_SEM_NUM]);
//            bqueue_show(shared_mem_addr);
            clientPid = bqueue_get(smaddr);

            if (clientPid != -1) {
                bqueue_occupy_chair(smaddr, clientPid);
                make_haircut(clientPid);
                sem_post(semarr[FIFO_SEM_NUM]);
            }
            else {
                printf("[%zu] BARBER: Spi.\n", get_time());
                sem_wait(semarr[BARBER_SEM_NUM]);
                sem_post(semarr[FIFO_SEM_NUM]);
                break;
            }
        }
    }

    clean();
}

void configure()
{
    atexit(clean);

    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = &sigint_handler;
    act.sa_flags = SA_SIGINFO;
    if (sigaction(SIGINT, &act, NULL) == -1) {
        printf("sigaction error");
        exit(EXIT_FAILURE);
    }

    semarr[BARBER_SEM_NUM] = sem_open(BARBER_NAME, O_CREAT | O_RDWR | O_EXCL, 0600, 0);
    semarr[FIFO_SEM_NUM] = sem_open(FIFO_NAME, O_CREAT | O_RDWR | O_EXCL, 0600, 1);
    semarr[CUT_SEM_NUM] = sem_open(CUT_NAME, O_CREAT | O_RDWR | O_EXCL, 0600, 0);

    if (semarr[BARBER_SEM_NUM] == SEM_FAILED ||
        semarr[FIFO_SEM_NUM] == SEM_FAILED ||
        semarr[CUT_SEM_NUM] == SEM_FAILED) {
        perror("sem_open error: cannot retrieve semaphore");
        exit(EXIT_FAILURE);
    }

    int smid = shm_open(MEMORY_NAME, O_CREAT | O_RDWR | O_EXCL, 0600);
    if (smid == -1) {
        perror("shm_open error");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(smid, (chairs_count + 4) * sizeof(pid_t)) == -1) {
        perror("ftruncate error");
        exit(EXIT_FAILURE);
    }

    smaddr = mmap(NULL, (chairs_count + 4) * sizeof(pid_t), PROT_READ | PROT_WRITE, MAP_SHARED, smid, 0);
    if (smaddr == ( void * ) -1) {
        perror("mmap error");
        exit(EXIT_FAILURE);
    }

    bqueue_init(smaddr, chairs_count);
}

void sigint_handler(int sig, siginfo_t *siginfo, void *context)
{
    printf("%s\n", "Otrzymano SIGINT. Zamkniecie zakladu fryzjerskiego.");
    finish = 1;
}

void clean()
{
    if (munmap(smaddr, (chairs_count + 4) * sizeof(int)) == -1) {
        perror("munmap error");
    }

    if (shm_unlink(MEMORY_NAME) == -1) {
        perror("shm_unlink error");
    }

    if (sem_unlink(BARBER_NAME) == -1 ||
        sem_unlink(FIFO_NAME) == -1 ||
        sem_unlink(CUT_NAME) == -1) {
        perror("sem_unlink error");
    }
}
