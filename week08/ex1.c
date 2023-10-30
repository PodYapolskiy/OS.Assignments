#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define PASSWORD_LENGTH 8

int main() {
    pid_t pid = getpid(); // get the process ID

    // create and open the /tmp/ex1.pid file
    FILE* pid_file = fopen("/tmp/ex1.pid", "w");
    if (pid_file == NULL) {
        perror("Error opening /tmp/ex1.pid");
        return 1;
    }

    // write the pid to the file
    fprintf(pid_file, "%d\n", pid);
    fclose(pid_file);

    // Open /dev/urandom to generate a random password
    int urandom_fd = open("/dev/urandom", O_RDONLY);
    if (urandom_fd == -1) {
        perror("Error opening /dev/urandom");
        return 1;
    }

    // read random bytes from /dev/urandom
    char password[PASSWORD_LENGTH + 1];
    if (read(urandom_fd, password, PASSWORD_LENGTH) == -1) {
        perror("Error reading from /dev/urandom");
        close(urandom_fd);
        return 1;
    }

    // close /dev/urandom
    close(urandom_fd);

    // ensure that password consists of printable characters
    for (int i = 0; i < PASSWORD_LENGTH; i++) {
        // % 94 we got through considering [32, 126] from '!' to '~'
        password[i] = '!' + (abs(password[i]) % 94);
    }

    // add the "pass:" prefix
    char final_password[PASSWORD_LENGTH + 6];
    sprintf(final_password, "pass:%s", password);
    
    // Copy the password into the mapped memory
    char* password_ptr = (char*) malloc(PASSWORD_LENGTH + 6);
    strcpy(password_ptr, final_password);

    // printing for checking yourself
    // printf("pid: %d\n", getpid());
    // printf("%s\n", password_ptr);

    // infinite loop to keep the program running
    while (1) sleep(1);

    // freeing allocated memory is not necessary in this case
    // as the process will exit when it receives a SIGKILL.
    return 0;
}
