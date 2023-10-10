#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_THREADS 16

typedef struct {
    unsigned int id; // as real ids (>=0)
    int i;
    char message[256];
} Thread;

void* thread_function(void* arg) {
    Thread* thread_data = (Thread*) arg; // cast to a custom thread type
    printf("Thread %d prints message: %s\n", thread_data->id, thread_data->message);
    free(thread_data); // free the current thread pointer
    pthread_exit(NULL); // exit the thread
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <number of threads>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]); // get n from command line
    if (n < 1 || n > MAX_THREADS) {
        printf("Number of threads should be between 1 and %d\n", MAX_THREADS);
        return 1;
    }

    pthread_t threads[MAX_THREADS];
    for (int i = 0; i <= n; i++) {
        Thread* thread_data = (Thread*) malloc(sizeof(Thread));
        thread_data->id = i;
        thread_data->i = i;
        
        // put the message
        snprintf(thread_data->message, sizeof(thread_data->message), "Hello from thread %d", i);

        // create new i'th thread
        if (pthread_create(&threads[i - 1], NULL, thread_function, (void*) thread_data) != 0) {
            perror("pthread_create");
            return 1;
        }
        printf("Thread %d is created\n", i);

        // wait for the thread to finish before creating the next one
        pthread_join(threads[i - 1], NULL); // forces the order
    }

    return 0;
}
