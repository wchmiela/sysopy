#include "child.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>

static sig_atomic_t signals_received;

static void handler_SIG_INT(int signo, siginfo_t *siginfo, void *context)
{
    if (signo == SIGINT) {
        printf("\tOdebrano sygnal SIGINT nr: %d\n", signals_received);
        fflush(stdout);
        exit(EXIT_SUCCESS);
    }
}

static void handler_SIG(int signo, siginfo_t *siginfo, void *context)
{
    if (signo == SIGUSR1) {
        printf("\tDostarczono sygnal SIGUSR1 do potomka nr: %d\n", ++signals_received);
        kill(getppid(), SIGUSR1);
        fflush(stdout);
    }

    if (signo == SIGRTMIN) {
        printf("\tDostarczono sygnal czasu rzeczywistego SIGRTMIN do potomka nr: %d\n", ++signals_received);
        kill(getppid(), SIGRTMIN);
        fflush(stdout);
    }
}

static void handler_SIG_RT_kill(int signo, siginfo_t *siginfo, void *context)
{

    if (signo == SIGUSR2) {
        printf("\tDostarczono sygnal SIGUSR2 do potomka nr: %d\n", ++signals_received);
        fflush(stdout);
        exit(EXIT_SUCCESS);
    }

    if (signo == SIGRTMAX) {
        printf("\tDostarczono sygnal czasu rzeczywistego SIGRTMAX do potomka nr: %d\n", ++signals_received);
        fflush(stdout);
        exit(EXIT_SUCCESS);
    }

}

void _set()
{
    struct sigaction act;
    act.sa_sigaction = ( void (*)(int, siginfo_t *, void *) ) handler_SIG;
    act.sa_flags = SA_SIGINFO;

    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGRTMIN, &act, NULL);

    act.sa_sigaction = ( void (*)(int, siginfo_t *, void *) ) handler_SIG_RT_kill;

    sigaction(SIGUSR2, &act, NULL);
    sigaction(SIGRTMAX, &act, NULL);
}

int main(int argc, char *argv[])
{
    signals_received = 0;

    _set();

    while (1);
}
