#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

bool is_prime(int x) {
    if (x <= 1) return false;
    for (int i = 2; i * i <= x; i++) {
        if (x % i == 0) return false;
    }
    return true;
}

// primes counter in [a,b)
int primes_count(int a, int b) {
    int ret = 0;
    for (int i = a; i < b; i++)
        if (is_prime(i))
            ret++;
    return ret;
}

// argument to the start_routine of the thread
typedef struct PrimeStruct {
    int a, b;
} PrimeStruct;

int total_count = 0; // global variable to hold the total count

// thread function to count primes in a subinterval
void* prime_counter(void* arg) {
    PrimeStruct* req = (PrimeStruct*) arg;
    int count = primes_count(req->a, req->b);

    total_count += count; // increase total count

    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <n> <m>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);

    if (n <= 0 || m <= 0) {
        printf("Invalid input. n and m should be positive integers.\n");
        return 1;
    }

    int interval_size = n / m;
    int remainder = n % m;

    pthread_t threads[m];
    PrimeStruct primes[m];

    for (int i = 0; i < m; i++) {
        // assign boundaries also noticing case with remainder
        primes[i].a = i * interval_size;
        primes[i].b = (i + 1) * interval_size + (i == m - 1 ? remainder : 0);

        if (pthread_create(&threads[i], NULL, prime_counter, &primes[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    // terminate all threads
    for (int i = 0; i < m; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Number of prime numbers in [0, %d) with %d threads: %d\n", n, m, total_count);

    return 0;
}
