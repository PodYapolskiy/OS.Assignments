#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>

int main(void) {
    int pipe_fd[2];
    pipe(pipe_fd);

    pid_t subscriber = fork();

    char message[1024];
    if (subscriber == 0) {
        close(pipe_fd[1]); // close the write end of the pipe in the child process

        while (true) {
            read(pipe_fd[0], message, sizeof(message));
            printf("%s\n", message);
        }

        close(pipe_fd[0]); // close the read end of the pipe in the parent process
    } else {
        close(pipe_fd[0]); // close the read end of the pipe in the parent process

        while (true) {
            fgets(message, sizeof(message), stdin);
            write(pipe_fd[1], message, sizeof(message));
        }

        close(pipe_fd[1]); // close the write end of the pipe in the parent process
    }

    return 0;
}
