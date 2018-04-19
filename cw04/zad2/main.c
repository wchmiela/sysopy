#include <wait.h>
#include "main.h"

int *pid_array;

int *received_pids;

int n = -1;

int m = -1;

int signals_received;

bool run;

void handler_SIG_INT(int signo)
{
    if (signo == SIGINT) {
        printf("\tOdebrano sygnal SIGINT. Program zostanie zamkniety wraz z potomnymi procesami.\n");
        for (int i = 0; i < signals_received; i++) {
            kill(pid_array[i], SIGINT);
            printf("\tZabito proces %d", pid_array[i]);
        }
        exit(EXIT_SUCCESS);
    }
}

void handler_SIG_USR1(int signo, siginfo_t *siginfo, void *context)
{
    if (signo == SIGUSR1) {
        printf("\t\tOdebrano sygnal USR1 od procesu: %d\n", siginfo->si_pid);
        fflush(stdout);

        received_pids[signals_received++] = siginfo->si_pid;

        if (signals_received == m) {
            int i;
            for (i = 0; i < signals_received; ++i) {
                kill(received_pids[i], SIGUSR2);
            }
        }
        else if (signals_received > m) {
            kill(received_pids[signals_received - 1], SIGUSR2);
        }
        run = false;
    }
}

void handler_SIG_DEFAULT(int signo, siginfo_t *siginfo, void *context)
{
    printf("\t\tOdebrano sygnal %d od procesu: %d\n", signo, siginfo->si_pid);
}

void set_SIGUSR1_handler()
{
    struct sigaction act;
    act.sa_sigaction = ( void (*)(int, siginfo_t *, void *) ) handler_SIG_USR1;
    act.sa_flags = SA_SIGINFO;

    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SA_NODEFER);

    sigaction(SIGUSR1, &act, NULL);
}

void set_SIGDEFAULT_handler()
{
    struct sigaction act;
    act.sa_sigaction = ( void (*)(int, siginfo_t *, void *) ) handler_SIG_DEFAULT;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SA_NODEFER);

    for (int i = 0; i < 33; i++) {
        sigaction(SIGRTMIN + i, &act, NULL);
    }
}

void set_SIGINT_handler()
{
    struct sigaction act;
    act.sa_sigaction = ( void (*)(int, siginfo_t *, void *) ) handler_SIG_INT;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SA_NODEFER);

    sigaction(SIGINT, &act, NULL);
}

void prepare()
{
    set_SIGDEFAULT_handler();
    set_SIGUSR1_handler();
    set_SIGINT_handler();
}

void print_help()
{
    printf("Skladnia: ./zad2 -n [POTOMKOW] -m [PROSB]\n");
    printf("-n ilosc potomkow\n");
    printf("-m ilosc prosb\n");
}

int main(int argc, char *argv[])
{
    prepare();

    int next_option = -1;

    const char *short_opt = "n:m:";
    struct option long_opt[] =
        {
            {"help", no_argument, NULL, 'h'}
        };

    do {
        next_option = getopt_long(argc, argv, short_opt, long_opt, NULL);

        if (next_option == 'n') {
            n = atoi(optarg);
        }
        else if (next_option == 'm') {
            m = atoi(optarg);
        }
        else if (next_option == 'h') {
            print_help();
            exit(EXIT_SUCCESS);
        }

    }
    while (next_option != -1);

    if (n <= 0 || m <= 0) {
        printf("Podano niepoprawne argumenty\n");
        exit(EXIT_FAILURE);
    }

    printf("Argumenty n = %d | m = %d | Moj pid = %d \n", n, m, getpid());

    static char *file[32];
    file[0] = "./child";

    pid_array = calloc(sizeof(int), ( size_t ) n);
    received_pids = calloc(sizeof(int), ( size_t ) n);

    for (int i = 0; i < n; ++i) {
        int pid = fork();

        if (pid < 0) {
            perror("Blad zwiazany z forkiem");
        }

        if (pid == 0) {
            if (execvp(file[0], file) < 0) {
                perror("Blad zwiazany z exec");
                exit(EXIT_FAILURE);
            }
        }
        else {
            pid_array[i] = pid;
            run = true;
            while (run);
        }
    }

    int return_value;
    for (int i = 0; i < n; i++) {
        waitpid(pid_array[i], &return_value, 0);
        printf("Process finished:%d  code:%d\n", pid_array[i], return_value);
    }

    return 0;
}
