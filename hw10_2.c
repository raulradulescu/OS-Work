#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

volatile sig_atomic_t stop_switching = false;

void signal_handler(int signal) {
    if (signal == SIGUSR1) {
        stop_switching = true;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s N cmd1 cmd2\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int N = atoi(argv[1]);
    char *cmd1 = argv[2];
    char *cmd2 = argv[3];

    pid_t pid1 = fork();
    if (pid1 == 0) {
        // First child process
        execlp(cmd1, cmd1, (char *)NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    } else if (pid1 < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        // Second child process
        execlp(cmd2, cmd2, (char *)NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    } else if (pid2 < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    signal(SIGUSR1, signal_handler);

    kill(pid2, SIGSTOP);

    while (!stop_switching) {
        kill(pid1, SIGCONT);
        sleep(N);
        kill(pid1, SIGSTOP);
        printf("Process %d was put on hold, Process %d started\n", pid1, pid2);

        kill(pid2, SIGCONT);
        sleep(N);
        kill(pid2, SIGSTOP);
        printf("Process %d was put on hold, Process %d started\n", pid2, pid1);
    }
    kill(pid1, SIGKILL);
    kill(pid2, SIGKILL);

    int status1, status2;
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);

    printf("Parent process finished\n");

    return 0;
}
