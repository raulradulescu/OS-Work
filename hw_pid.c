#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void create_child_processes(int depth) {
    if (depth <= 0) return;  //base case

    pid_t pid = fork();  // create a new child process
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // inside child process
        printf("Process: PID=%d, Parent PID=%d\n", getpid(), getppid());

        // Recursive call in the child process with decremented depth
        create_child_processes(depth - 1);

        exit(0);  // terminate the child process after recursion
    } else {
        // parent process waits
        wait(NULL);
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <depth>\n", argv[0]);
        return 1;
    }

    int depth = atoi(argv[1]);  // Convert argument to integer
    if (depth <= 0) {
        fprintf(stderr, "Depth should be a positive integer.\n");
        return 1;
    }

    printf("Root Process: PID=%d, Parent PID=%d\n", getpid(), getppid());
    create_child_processes(depth);

    return 0;
}

