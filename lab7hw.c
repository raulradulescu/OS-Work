#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>

#define BUFSIZE 4096

void reverse_string(char *str, int length) {
    int i;
    for (i = 0; i < length / 2; i++) {
        char temp = str[i];
        str[i] = str[length - i - 1];
        str[length - i - 1] = temp;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file-path>\n", argv[0]);
        return 1;
    }

    char *file_path = argv[1]; 

    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    //put the file size into a buffer
    char buffer[BUFSIZE];
    size_t file_size = fread(buffer, 1, BUFSIZE, file);
    fclose(file);

    if (file_size == 0) {
        fprintf(stderr, "File is empty or read failed.\n");
        return 1;
    }

    //reverse the buffer content
    reverse_string(buffer, file_size);

    //create pipe for
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("Failed to create pipe");
        return 1;
    }
    
    //spawning a child process
    pid_t pid = fork();
    if (pid == -1) {
        perror("Failed to fork");
        return 1;
    }

    if (pid == 0) { // Child process
        close(pipe_fd[1]); //close the right end of pipe

        //read content from the pipe
        char received_buffer[BUFSIZE];
        ssize_t read_size = read(pipe_fd[0], received_buffer, BUFSIZE);
        received_buffer[read_size] = '\0';
        close(pipe_fd[0]);

        //process content
        int digit_count = 0, upper_count = 0, lower_count = 0;
        for (ssize_t i = 0; i < read_size; i++) {
            if (isdigit(received_buffer[i])) {
                digit_count++;
            } else if (isupper(received_buffer[i])) {
                upper_count++;
            } else if (islower(received_buffer[i])) {
                lower_count++;
            }
        }

        //response buffer
        char digits[BUFSIZE], uppers[BUFSIZE], lowers[BUFSIZE];
        memset(digits, 0, BUFSIZE);
        memset(uppers, 0, BUFSIZE);
        memset(lowers, 0, BUFSIZE);

        int digit_idx = 0, upper_idx = 0, lower_idx = 0;
        for (ssize_t i = 0; i < read_size; i++) {
            if (isdigit(received_buffer[i])) {
                digits[digit_idx++] = received_buffer[i];
            } else if (isupper(received_buffer[i])) {
                uppers[upper_idx++] = received_buffer[i];
            } else if (islower(received_buffer[i])) {
                lowers[lower_idx++] = received_buffer[i];
            }
        }

        //format the response
        char response[BUFSIZE];
        snprintf(response, BUFSIZE, "%d%s%d%s%d%s%.*s",
                 digit_count, digits,
                 upper_count, uppers,
                 lower_count, lowers,
                 (int)(read_size - digit_count - upper_count - lower_count), received_buffer);

        //write the response back to the parent
        write(STDOUT_FILENO, response, strlen(response));
        _exit(0);
    } else { //parent process
        close(pipe_fd[0]); //close the read end of the pipe
        //write the reversed content to the pipe
        write(pipe_fd[1], buffer, file_size);
        close(pipe_fd[1]);

        //wait for the child process to finish
        wait(NULL);

        //read the response from the child process
        char output_buffer[BUFSIZE];
        ssize_t output_size = read(STDIN_FILENO, output_buffer, BUFSIZE);
        output_buffer[output_size] = '\0';

        printf("%s\n", output_buffer);
    }

    return 0;
}