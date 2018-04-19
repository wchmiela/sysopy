#include "main.h"

bool running = true;

void handler_SIG_INT(int signo) {
    if (signo == SIGINT) {
        printf("\tOdebrano sygnal SIGINT. Program zostanie zamkniety.\n");
        exit(EXIT_SUCCESS);
    }
}

void handler_SIG_TSTP(int signo) {
    if (signo == SIGTSTP) {
        printf("Odebrano sygnal SIGTSTP.\n");
        running = !running;
    }
}

int main(int argc, char *argv[]) {

    while (1) {
        if (running) {
            int pid = fork();

            if (pid == 0) {
                execlp("date", "");
            } else {
                int res;
                waitpid(pid, &res, 0);
                if (res != 0) {
                    exit(EXIT_FAILURE);
                }
            }

            signal(SIGINT, handler_SIG_INT);

            struct sigaction act;
            act.sa_sigaction = (void (*)(int, siginfo_t *, void *)) handler_SIG_TSTP;
            sigemptyset(&act.sa_mask);
            act.sa_flags = 0;
            sigaction(SIGTSTP, &act, NULL);

            sleep(1);
        }
    }

    return 0;
}