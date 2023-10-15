#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void handle_sigusr1(int signum) {
    // read and print the contents of text.txt
    char buffer[256];
    int fd = open("text.txt", O_RDONLY);
    if (fd == -1) {
        perror("Error opening text.txt");
        exit(1);
    }

    ssize_t bytes_read; // represents output of file descriptor (may contain negative result)
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        write(STDOUT_FILENO, buffer, bytes_read);
    }
    close(fd);
}

void handle_sigusr2(int signum) {
    // print termination message and exit
    printf("Process terminating...\n");
    exit(EXIT_SUCCESS);
}

int main() {
    // create and write PID to /var/run/agent.pid
    int pid = getpid();
    int pid_file = open("/var/run/agent.pid", O_CREAT | O_WRONLY, 0644);
    if (pid_file == -1) {
        perror("Error creating agent.pid");
        exit(1);
    }
    dprintf(pid_file, "%d", pid);
    close(pid_file);

    // set up signal handlers
    signal(SIGUSR1, handle_sigusr1);
    signal(SIGUSR2, handle_sigusr2);

    // read and print the contents of text.txt
    char buffer[256];
    int fd = open("text.txt", O_RDONLY);
    if (fd == -1) {
        perror("Error opening text.txt");
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        write(STDOUT_FILENO, buffer, bytes_read);
    }
    close(fd);

    // sleep indefinitely
    while (1) {
        sleep(1);
    }

    return 0;
}
