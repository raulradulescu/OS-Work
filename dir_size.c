#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

void traverse_directory(const char *path) {
    struct dirent *entry;
    struct stat file_stat;
    DIR *dir = opendir(path);

    if (dir == NULL) {
        perror("Unable to open directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        char full_path[1024];

        // Skip the current and parent directory entries ('.' and '..')
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        // Get the file stats (size, type, etc.)
        if (stat(full_path, &file_stat) == -1) {
            perror("Unable to get file stats");
            continue;
        }

        // If it's a regular file, print its size
        if (S_ISREG(file_stat.st_mode)) {
            printf("File: %s, Size: %ld bytes\n", full_path, file_stat.st_size);
        }

        // If it's a directory, recursively traverse it
        if (S_ISDIR(file_stat.st_mode)) {
            traverse_directory(full_path);
        }
    }

    closedir(dir);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *directory = argv[1];

    // Start recursive directory traversal
    traverse_directory(directory);

    return 0;
}

