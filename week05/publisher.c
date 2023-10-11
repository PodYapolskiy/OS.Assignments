#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number_of_subscribers>\n", argv[0]);
        return 1;
    }

    int num_subscribers = atoi(argv[1]);
    if (num_subscribers < 1 || num_subscribers > 3) {
        fprintf(stderr, "Number of subscribers must be between 1 and 3.\n");
        return 1;
    }

    // create directory to enable mkfifo creating file pipes
    mkdir("/tmp/ex1/", 0777);

    // Create named pipes for subscribers
    char pipe_path[20];
    int pipe_fd[num_subscribers];
    for (int i = 1; i <= num_subscribers; i++) {
        snprintf(pipe_path, sizeof(pipe_path), "/tmp/ex1/s%d", i);

        // delete if such file already exists
        if (access(pipe_path, F_OK) == 0) remove(pipe_path);

        if (mkfifo(pipe_path, 0666) == -1) {
            perror("mkfifo");
            return 1;
        }
        pipe_fd[i - 1] = open(pipe_path, O_WRONLY);
    }

    char message[1024];
    while (1) {
        // read a message from stdin
        if (fgets(message, sizeof(message), stdin) == NULL) break;

        // publish the message to all subscribers
        for (int i = 0; i < num_subscribers; i++) {
            write(pipe_fd[i], message, strlen(message));
        }
    }

    // close and remove the named pipes and directory
    for (int i = 0; i < num_subscribers; i++) {
        close(pipe_fd[i]);
        snprintf(pipe_path, sizeof(pipe_path), "/tmp/ex1/s%d", i + 1);
        unlink(pipe_path);
        remove(pipe_path);
    }
    rmdir("/tmp/ex1");

    return 0;
}
