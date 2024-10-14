#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Function to check if a number is prime
int is_prime(int num) {
    if (num <= 1) return 0;
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) return 0;
    }
    return 1;
}

// Function to calculate the average of the first `count` prime numbers
double calculate_prime_average(int count) {
    int sum = 0;
    int num = 2;
    int prime_count = 0;
    
    while (prime_count < count) {
        if (is_prime(num)) {
            sum += num;
            prime_count++;
        }
        num++;
    }
    return (double)sum / count;
}

int main(int argc, char **argv)
{
    int aflag = 0;
    int bflag = 0;
    int dflag = 0;
    char *cvalue = NULL;
    char *evalue = NULL;
    int index;
    int c;
    
    opterr = 0;

    while ((c = getopt(argc, argv, "abc:de:")) != -1) {
        switch (c) {
            case 'a':
                aflag = 1;
                break;
            case 'b':
                bflag = 1;
                break;
            case 'c':
                cvalue = optarg;
                break;
            case 'd':
                dflag = 1;
                break;
            case 'e':
                evalue = optarg;
                break;
            case '?':
                if (optopt == 'c' || optopt == 'e') {
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

    // Output aflag, bflag, and cvalue
    printf("aflag = %d, bflag = %d, cvalue = %s\n", aflag, bflag, cvalue);

    // If -d is enabled, calculate and print the average of the first 100 primes
    if (dflag) {
        double avg_100_primes = calculate_prime_average(100);
        printf("avg of the first 100 primes = %f\n", avg_100_primes);
    }

    // If -e is enabled, compute the average between the average of cvalue primes and evalue primes
    if (cvalue && evalue) {
        int cnum = atoi(cvalue);
        int enum_value = atoi(evalue);
        
        double avg_c_primes = calculate_prime_average(cnum);
        double avg_e_primes = calculate_prime_average(enum_value);
        double avg_combined = (avg_c_primes + avg_e_primes) / 2;

        printf("evalue = %s\n", evalue);
        printf("AVG(%f, %f) = %f\n", avg_c_primes, avg_e_primes, avg_combined);
    }

    return 0;
}
