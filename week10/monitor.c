#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdbool.h>

#define PATH_MAX 1024
#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

char* path;

void print_stat_info(const char* path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        printf("Stat info for \"%s\":\n", path);
        printf(" - Inode: %lu\n", (unsigned long) st.st_ino);
        printf(" - Size: %ld bytes\n", (long) st.st_size);
    } else {
        perror("Error getting stat info");
    }
    printf("\n");
}

bool is_event_to_track(struct inotify_event* event) {
    if (event->mask
        | IN_CREATE
        | IN_DELETE
        | IN_MODIFY
        | IN_OPEN
        | IN_ATTRIB
        | IN_ACCESS)
        return true;
    return false;
}

void handle_interruption(int signum) {
    // handle SIGINT by printing stat info for all entries and then interrupting
    if (signum == SIGINT) {
        printf("Received SIGINT. Printing stat info for all entries:\n");
        print_stat_info(path);
        sleep(3);
        exit(0);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // extract path to watch
    path = argv[1];

    // set up signal handling for SIGINT
    signal(SIGINT, handle_interruption);

    int inotify_fd = inotify_init();
    if (inotify_fd == -1) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }

    // add watch for the specified directory
    int watch_desc = inotify_add_watch(inotify_fd, path, IN_ALL_EVENTS);
    if (watch_desc == -1) {
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }

    // print stat info for all files and folders in the directory on startup
    printf("Stat info for all entries on startup:\n");
    print_stat_info(path);

    char buffer[EVENT_BUF_LEN];
    while (1) {
        ssize_t len = read(inotify_fd, buffer, EVENT_BUF_LEN);
        if (len == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        for (char* ptr = buffer; ptr < buffer + len; ptr += sizeof(struct inotify_event) + ((struct inotify_event*) ptr)->len) {
            struct inotify_event* event = (struct inotify_event*) ptr;

            // prevent some strange null events
            // if (strcmp(event->name, "") == 0 || event->len == 0) {
            if (!is_event_to_track(event) || event->name == NULL) {
                continue;
            }

            // print event type
            printf("Event: ");
            // printf("Event: %d\n", event->mask);
            if (event->mask & IN_CREATE) {
                printf("File/directory created: %s\n", event->name);
            } else if (event->mask & IN_DELETE) {
                printf("File/directory deleted: %s\n", event->name);
            } else if (event->mask & IN_MODIFY) {
                printf("File modified: %s\n", event->name);
            } else if (event->mask & IN_OPEN) {
                printf("File/directory opened: %s\n", event->name);
            } else if (event->mask & IN_ATTRIB) {
                printf("Metadata changed: %s\n", event->name);
            } else if (event->mask & IN_ACCESS) {
                printf("File accessed: %s\n", event->name);
            }

            // print stat info for the changed item
            char path[PATH_MAX];
            snprintf(path, PATH_MAX, "%s/%s", argv[1], event->name);
            print_stat_info(path);
        }
    }

    // remove watch and close descriptor
    inotify_rm_watch(inotify_fd, watch_desc);
    close(inotify_fd);

    return 0;
}
