#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void signal_handler(int signal) {
    static int count = 0;
    static double a_n = 0;
    double r = atof(getenv("R"));

    a_n = a_n + 1.0 / r;
    printf("a[%d] = %f\n", count + 1, a_n);
    count++;

    if (count == 2) {
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s a0 r\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    double a0 = atof(argv[1]);
    double r = atof(argv[2]);

    //set a0 and r
    char a0_str[20], r_str[20];
    snprintf(a0_str, sizeof(a0_str), "%.10f", a0);
    snprintf(r_str, sizeof(r_str), "%.10f", r);
    setenv("A0", a0_str, 1);
    setenv("R", r_str, 1);

    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        signal(SIGUSR1, signal_handler);
        double a_n = atof(getenv("A0"));
        printf("a[0] = %f\n", a_n);
        while (1) {
            pause(); // Wait for SIGUSR1
        }
    } else if (pid > 0) {
        // Parent process
        printf("*****\n");
        int print_flag = 0;
        for (int i = 0; i < 3; i++) {
            sleep(1);
            if (print_flag == 0) {
                printf("+++++\n");
                print_flag = 1;
            } else {
                printf("------\n");
                print_flag = 0;
            }
            kill(pid, SIGUSR1);
        }

        int status;
        waitpid(pid, &status, 0);
        printf("Child exited with status %d\n", WEXITSTATUS(status));
    } else {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    return 0;
}
