#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h> // dir

#define PATH_MAX 4096 // copy value from limits.h

int main(void) {
    DIR* dir;
    struct dirent* entry;
    char full_path[PATH_MAX];

    // try to open watched directory
    if ((dir = opendir("/")) == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    // iterate through content of "/"
    while ((entry = readdir(dir)) != NULL) {
        snprintf(full_path, sizeof(full_path), "/%s", entry->d_name);
        printf("%s\n", full_path);
    }

    return 0;
}
