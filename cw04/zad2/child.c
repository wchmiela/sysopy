#include "child.h"

double time_stamp;

void handler_SIG_USR2(int signo, siginfo_t *siginfo, void *context)
{
    if (signo == SIGUSR2) {
        double now = (clock() / ( double ) CLOCKS_PER_SEC);
        printf("\t\tOdebrano sygnal USR2 od procesu: %d  \tRoznica czasu: %f\n", siginfo->si_pid, now - time_stamp);
        fflush(stdout);
        kill(getppid(), SIGRTMIN + rand() % 32);
        exit(EXIT_SUCCESS);
    }
}

void prepare()
{
    struct sigaction act;
    act.sa_sigaction = ( void (*)(int, siginfo_t *, void *) ) handler_SIG_USR2;
    act.sa_flags = SA_SIGINFO;

    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SA_NODEFER);

    sigaction(SIGUSR2, &act, NULL);
}

int main(int argc, char *argv[])
{
    prepare();

    srand(( unsigned int ) time(NULL));
    sleep(( unsigned int ) (rand() % 3));

    time_stamp = clock() / ( double ) CLOCKS_PER_SEC;

    kill(getppid(), SIGUSR1);

    while (1);

    return 0;
}
