#include "main.h"

int l = -1;

int type = -1;

sig_atomic_t signals_sent = 0;

sig_atomic_t signals_received = 0;

pid_t child_pid;

union sigval val;

void print_help();

static void handler_SIG_INT(int signo)
{
    if (signo == SIGINT) {
        kill(child_pid, SIGINT);
        exit(EXIT_SUCCESS);
    }
}

static void handler_SIG(int signo, siginfo_t *siginfo, void *context)
{
    if (signo == SIGUSR1) {
        printf("\t\tOdebrano sygnal USR1 od potomka nr: %d\n", ++signals_received);
        fflush(stdout);
    }
    else if (signo == SIGRTMIN) {
        printf("\t\tOdebrano sygnal czasu rzeczywistego od potomka nr: %d\n", ++signals_received);
        fflush(stdout);
    }
}

void _set()
{
    struct sigaction act;
    act.sa_sigaction = ( void (*)(int, siginfo_t *, void *) ) handler_SIG;
    act.sa_flags = SA_SIGINFO;

    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SA_NODEFER);

    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGRTMIN, &act, NULL);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, handler_SIG_INT);

    int next_option = -1;

    const char *short_opt = "l:";
    struct option long_opt[] =
        {
            {"type", required_argument, NULL, 't'},
            {"help", no_argument, NULL, 'h'}
        };

    do {
        next_option = getopt_long(argc, argv, short_opt, long_opt, NULL);

        if (next_option == 't') {
            type = atoi(optarg);
        }
        else if (next_option == 'l') {
            l = atoi(optarg);
        }
        else if (next_option == 'h') {
            print_help();
            exit(EXIT_SUCCESS);
        }
    }
    while (next_option != -1);

    if (l == -1 || type >= 4 || type <= 0) {
        printf("%s\n", "Podano niepoprawne arugmenty");
        exit(EXIT_FAILURE);
    }

    static char *file[32];
    file[0] = "./child";

    child_pid = fork();

    if (child_pid < 0) {
        printf("%s\n", "Wystapil blad w tworzeniu procesu.");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) {

        if (execvp(file[0], file) < 0) {
            printf("%s\n", "Wystapil blad w uruchomieniu programu.");
            exit(EXIT_FAILURE);
        }
    }

    sleep(1);
    int i;
    for (i = 0; i < l; ++i) {

        switch (type) {
            case 1:printf("Wyslano sygnal USR1 do potomka nr: %d\n", ++signals_sent);
                fflush(stdout);

                kill(child_pid, SIGUSR1);
                break;
            case 2:printf("Wyslano sygnal USR1  do potomka nr: %d\n", ++signals_sent);
                fflush(stdout);

                val.sival_int = SIGUSR1;
                if (sigqueue(child_pid, SIGUSR1, val) < 0) {
                    printf("%s\n", "Wystapil blad wysylaniu sygnalu.");
                }

                break;
            case 3:printf("Wyslano sygnal czasu rzeczywistego SIGRTMIN do potomka nr: %d\n", ++signals_sent);
                fflush(stdout);

                kill(child_pid, SIGRTMIN);
                break;
        }

    }

    sleep(5);
    switch (type) {
        case 1:printf("Wyslano sygnal USR2 do potomka nr: %d\n", ++signals_sent);
            fflush(stdout);

            kill(child_pid, SIGUSR2);
            break;
        case 2:printf("Wyslano sygnal USR2 do potomka nr: %d\n", ++signals_sent);
            fflush(stdout);

            val.sival_int = SIGUSR2;
            if (sigqueue(child_pid, SIGUSR2, val) < 0) {
                printf("%s\n", "Wystapil blad wysylaniu sygnalu.");
            }

            break;
        case 3:printf("Wyslano sygnal czasu rzeczywistego SIGRTMAX do potomka nr: %d\n", ++signals_sent);
            fflush(stdout);

            kill(child_pid, SIGRTMAX);
            break;
    }

    return 0;
}

void print_help()
{
    printf("%s\n", "-l [Liczba] --type [Typ]");
    printf("%s\n", "-l\tliczba sygnalow do wyslania");
    printf("%s\n", "--type\t 1 lub 2 lub 3");
}