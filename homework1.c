#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int sum = 0;
    int c;
    opterr = 0;

    while ((c = getopt(argc, argv, "a:b:c:")) != -1) {
        switch (c) {
            case 'a':
            case 'b':
            case 'c':
                // Convert the argument to an integer and add it to the sum
                sum += atoi(optarg);
                break;
            case '?':
                if (optopt == 'a' || optopt == 'b' || optopt == 'c') {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                } else if (isprint(optopt)) {
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                } else {
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                }
                return 1;
            default:
                abort();
        }
    }

    printf("%d\n", sum);

    return 0;
}
