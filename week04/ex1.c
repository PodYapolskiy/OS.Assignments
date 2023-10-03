#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

int main() {
    pid_t child_1, child_2;
    clock_t t, t_1, t_2;
    t = clock();

    child_1 = fork();
    t_1 = clock();
    if (child_1 == 0) {
        printf("PID 1: %d\n", getpid());
        printf("PPID 1: %d\n", getppid());
        t_1 = clock() - t_1;
        printf("Time: %f\n\n", (double) t_1 / CLOCKS_PER_SEC);
    } else {
        child_2 = fork();
        t_2 = clock();

        if (child_2 == 0) {
            printf("PID 2: %d\n", getpid());
            printf("PPID 2: %d\n", getppid());
            t_2 = clock() - t_2;
            printf("T: %f\n\n", (double) t_2 / CLOCKS_PER_SEC);
        } else {
            // wait for 2 child processes
            wait(NULL);
            wait(NULL);

            printf("PID 0: %d\n", getpid());
            printf("PPID 0: %d\n", getppid());
            t = clock() - t;
            printf("T: %f\n", (double)t / CLOCKS_PER_SEC);
        }
    }


    return 0;
}