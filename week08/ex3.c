#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <unistd.h>

int main() {
    struct rusage usage;
    int size = 10 * 1024 * 1024; // 10 MB

    for (int i = 0; i < 10; i++) {
        // allocate memory
        char* mem = (char*) malloc(size);
        if (mem == NULL) {
            perror("Memory allocation failed");
            return 1;
        }

        // fill memory with zeros
        memset(mem, 0, size);

        // print memory usage
        if (getrusage(RUSAGE_SELF, &usage) == 0) {
            printf("Memory usage: %d (sec) - %ld (MB)\n\n", i + 1, usage.ru_maxrss / 1024);
        }

        // sleep for 1 second
        sleep(1);
    }

    return 0;
}

