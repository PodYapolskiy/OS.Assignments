#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <subscriber_index>\n", argv[0]);
        return 1;
    }

    int subscriber_index = atoi(argv[1]);
    if (subscriber_index < 1 || subscriber_index > 3) {
        fprintf(stderr, "Subscriber index must be between 1 and 3.\n");
        return 1;
    }

    char pipe_path[20];
    snprintf(pipe_path, sizeof(pipe_path), "/tmp/ex1/s%d", subscriber_index);

    int pipe_fd = open(pipe_path, O_RDONLY);
    if (pipe_fd == -1) {
        perror("open");
        return 1;
    }

    char message[1024];
    ssize_t bytes_read;

    while ((bytes_read = read(pipe_fd, message, sizeof(message))) > 0) {
        message[bytes_read] = '\0';
        printf("Subscriber %d received: %s\n", subscriber_index, message);
    }

    close(pipe_fd);
    return 0;
}
