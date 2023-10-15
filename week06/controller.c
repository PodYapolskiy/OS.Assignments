#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// agent_pid should be global to be able to send SIGTERM signal to it
int agent_pid;

void handle_sigint(int signum) {
    kill(agent_pid, SIGTERM);
    exit(EXIT_SUCCESS);
}

int main() {
    // handle SIGINT on the controller to send SIGTERM to the agent and exit
    signal(SIGINT, handle_sigint);

    // check for a running agent by fetching the PID from /var/run/agent.pid
    int pid_file = open("/var/run/agent.pid", O_RDONLY);
    if (pid_file == -1) {
        printf("Error: No agent found.\n"); // required error message
        exit(EXIT_FAILURE);
    }

    char pid_str[16];
    ssize_t bytes_read = read(pid_file, pid_str, sizeof(pid_str));
    if (bytes_read == -1) {
        perror("Error reading .pid file");
        close(pid_file);
        exit(EXIT_FAILURE);
    }
    pid_str[bytes_read] = '\0';
    close(pid_file);

    agent_pid = atoi(pid_str);
    if (agent_pid > 0) {
        printf("Agent found.\n"); // the required success message
    } else { // additional error message to ease debugging
        printf("Failed to read Agent PID from .pid file.\n");
    }

    while (1) {
        // prompt for user input
        printf("Choose a command {\"read\", \"exit\", \"stop\", \"continue\"} to send to the agent: ");
        char command[20];
        scanf("%s", command);

        if (strcmp(command, "read") == 0) {
            kill(agent_pid, SIGUSR1); // send SIGUSR1 signal to the agent
        } else if (strcmp(command, "exit") == 0) {
            kill(agent_pid, SIGUSR2); // send SIGUSR2 signal to the agent
            exit(0); // controller is also being terminated
        } else if (strcmp(command, "stop") == 0) {
            kill(agent_pid, SIGSTOP); // send SIGSTOP signal to the agent
        } else if (strcmp(command, "continue") == 0) {
            kill(agent_pid, SIGCONT); // send SIGCONT signal to the agent
        } else {
            printf("Invalid command.\n");
        }
    }

    return 0;
}
