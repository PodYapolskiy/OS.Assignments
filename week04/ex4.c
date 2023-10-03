#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SIZE 1024

int main() {
    char input[SIZE];
    char* args[SIZE];

    while (1) {
        printf("~cutie shell~ ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("fgets");
            exit(1);
        }

        // remove newline character
        input[strlen(input) - 1] = '\0';

        // exit
        if (strcmp(input, "exit") == 0) { break; }

        int i = 0;
        char* token = strtok(input, " ");
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        // check if the command should run in the background
        int run_in_background = 0;
        if (i > 0 && strcmp(args[i - 1], "&") == 0) {
            run_in_background = 1;
            args[i - 1] = NULL; // remove the "&" argument
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) { // child process
            if (execvp(args[0], args) == -1) {
                perror("execvp");
                exit(1);
            }
        } else { // parent process
            if (!run_in_background) {
                // wait for the child process to complete
                int status;
                if (waitpid(pid, &status, 0) == -1) {
                    perror("waitpid");
                    exit(1);
                }
            }
        }
    }

    return 0;
}
