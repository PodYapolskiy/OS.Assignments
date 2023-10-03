#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SIZE 120
#define FILE_NAME "temp.txt"

/// @brief dot product of 2 subvectors of [i, j]
/// @param u first vector
/// @param v second vector
/// @param start index
/// @param end index
/// @return math dot product
int dot_product(int u[], int v[], int start, int end) {
    int result = 0;
    for (int i = start; i < end; i++) {
        result += u[i] * v[i];
    }
    return result;
}

int main() {
    remove(FILE_NAME); // remove if anything we same name existed before

    int n;
    printf("(n): ");
    scanf("%d", &n);

    // check constraints on n
    if (n % 2 || n > 10 || n < 2) return 1;

    int u[SIZE], v[SIZE];
    for (int i = 0; i < SIZE; i++) {
        u[i] = rand() % 100;
        v[i] = rand() % 100;
    }

    // multi processes part
    pid_t child_pids[n];
    for (int i = 0; i < n; i++) {
        child_pids[i] = fork(); // create a fork each time

        if (child_pids[i] == 0) { // child process
            printf("PID: %d, PPID: %d\n", getpid(), getppid());
            int start = i * (SIZE / n);
            int end = (i + 1) * (SIZE / n);
            int result = dot_product(u, v, start, end);

            // append the process result to the shared file
            FILE* file = fopen(FILE_NAME, "a");
            fprintf(file, "%d\n", result);
            fclose(file);

            // using exit(0) does not break the rules
            // when the child borns it goes here meanwhile
            // main is iterate over another and create them
            // so they all work simultaneously
            exit(0);
        } else if (child_pids[i] < 0) {
            perror("fork");
            return 1;
        }
    }

    // wait for all child processes to finish
    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

    // aggregate the results
    FILE* file = fopen(FILE_NAME, "r");
    int result, total_result = 0;
    while (fscanf(file, "%d", &result) != EOF) { total_result += result; }
    fclose(file);

    printf("u * v: %d\n", total_result);

    return 0;
}
