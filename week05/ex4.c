#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

// mutexes for synchronization
pthread_mutex_t k_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t c_lock = PTHREAD_MUTEX_INITIALIZER;

// global variables
int k = 0; // temp number to check for primality
int c = 0; // the number of primes discovered so far
int n = 0; // higher bound of numbers range

// primes counter in [a,b)
bool is_prime(int x) {
    if (x <= 1) return false;
    for (int i = 2; i * i <= x; i++) {
        if (x % i == 0) return false;
    }
    return true;
}

// get next prime candidate
int get_number_to_check() {
    int ret;
    pthread_mutex_lock(&k_lock); // enters dangerous region
    ret = k;
    if (k != n) k++;
    pthread_mutex_unlock(&k_lock); // leaves it
    return ret;
}

// increment prime counter
void increment_primes() {
    pthread_mutex_lock(&c_lock);
    c++; // increment dangerous region safely
    pthread_mutex_unlock(&c_lock);
}

// thread routine
void* check_primes(void* arg) {
    while (true) {
        int num = get_number_to_check();
        if (num >= n)
            break;

        if (is_prime(num))
            increment_primes();
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <n> <m>\n", argv[0]);
        return 1;
    }

    // get parameters
    n = atoi(argv[1]);
    int m = atoi(argv[2]);

    if (n <= 0 || m <= 0) {
        printf("n and m should be positive integers.\n");
        return 1;
    }

    pthread_t threads[m]; // threads array
    for (int i = 0; i < m; i++) {
        if (pthread_create(&threads[i], NULL, check_primes, NULL) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    // close all threads
    for (int i = 0; i < m; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Number of prime numbers in [0, %d) with %d threads: %d\n", n, m, c);

    return 0;
}
