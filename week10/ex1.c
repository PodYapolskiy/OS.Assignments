#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h> // dir
#include <signal.h>

#define PATH_MAX 4096

char* path;

void find_all_hlinks(const char* source) {
    DIR* dir;

    // entries
    struct dirent* entry;
    struct stat entry_stat, source_stat;

    // paths
    char full_path[PATH_MAX];
    char real_source[PATH_MAX];

    // collect info on source
    if (lstat(source, &source_stat) == -1) {
        perror("Error getting file information");
        exit(EXIT_FAILURE);
    }

    // construct real path
    if (realpath(source, real_source) == NULL) {
        perror("Error getting realpath");
        exit(EXIT_FAILURE);
    }

    // try to open watched directory
    if ((dir = opendir(path)) == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    printf("Hard links for %s (inode %lu):\n", real_source, (unsigned long) source_stat.st_ino);
    while ((entry = readdir(dir)) != NULL) {
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        if (lstat(full_path, &entry_stat) == -1) {
            perror("Error getting file information");
            exit(EXIT_FAILURE);
        }

        // conditions to be a hard link
        if (S_ISREG(entry_stat.st_mode) && entry_stat.st_ino == source_stat.st_ino &&
            strcmp(full_path, real_source) != 0) {
            char real_path[PATH_MAX];

            if (realpath(full_path, real_path) == NULL) {
                perror("Error getting realpath");
                exit(EXIT_FAILURE);
            }

            // copy content
            char content[256];
            FILE* out = fopen(real_path, "r");
            fscanf(out, "%[^\n]", content);
            fclose(out);

            // print needed info of hard link
            printf("%s\n", entry->d_name);
            printf("\tInode: %lu\n\tPath: %s\n\tContent: %s\n", (unsigned long) entry_stat.st_ino, real_path, content);
        }
    }

    closedir(dir);
}

void unlink_all(const char* source) {
    struct stat source_stat;
    char real_source[PATH_MAX];

    if (lstat(source, &source_stat) == -1) {
        perror("Error getting file information");
        exit(EXIT_FAILURE);
    }

    if (realpath(source, real_source) == NULL) {
        perror("Error getting realpath");
        exit(EXIT_FAILURE);
    }

    printf("Unlinking all duplicates for %s (inode %lu):\n", real_source, (unsigned long) source_stat.st_ino);

    // iterate through all hard links and unlink them, keeping only one
    struct stat entry_stat;
    char full_path[PATH_MAX];

    DIR* dir;
    struct dirent* entry;

    if ((dir = opendir(path)) == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        if (lstat(full_path, &entry_stat) == -1) {
            perror("Error getting file information");
            exit(EXIT_FAILURE);
        }

        if (S_ISREG(entry_stat.st_mode) && entry_stat.st_ino == source_stat.st_ino &&
            strcmp(full_path, real_source) != 0) {
            if (unlink(full_path) == -1) {
                perror("Error unlinking file");
                exit(EXIT_FAILURE);
            }
            printf("Unlinked: %s\n", full_path);
        }
    }

    // print the stat info of the last kept hard link
    printf("Last hard link: %s\n", real_source);

    closedir(dir);
}

void create_sym_link(const char* source, const char* link) {
    if (symlink(source, link) == -1) {
        perror("Error creating symbolic link");
        exit(EXIT_FAILURE);
    }

    printf("Created symbolic link from %s to %s\n", link, source);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <watched_directory_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char temp_path[PATH_MAX];
    path = argv[1];

    // strings temp
    char file1[] = "myfile1.txt";
    char file11[] = "myfile11.txt";
    char file12[] = "myfile12.txt";
    char file13[] = "myfile13.txt";

    // create and write to a file
    FILE* f1 = fopen(file1, "w");
    fprintf(f1, "Hello world. ");
    fclose(f1);

    // create hard links
    link(file1, file11);
    link(file1, file12);

    // add files to watched directory
    strcpy(temp_path, path);
    strcat(temp_path, "/");
    strcat(temp_path, file11);
    rename(file11, temp_path);

    strcpy(temp_path, path);
    strcat(temp_path, "/");
    strcat(temp_path, file12);
    rename(file12, temp_path);

    // find all hard links of "myfile1.txt"
    find_all_hlinks(file1);

    // move "myfile1.txt" to another folder /tmp/myfile1.txt
    strcpy(temp_path, "/tmp/");
    strcat(temp_path, file1);
    rename(file1, temp_path);

    // modify "myfile11.txt"
    strcpy(temp_path, path);
    strcat(temp_path, "/");
    strcat(temp_path, file11);

    FILE* f11 = fopen(temp_path, "a");
    fprintf(f11, "PUT YOUR PERSONAL LIFE AWAY, LET'S DO OS HOMEWORK! ");
    fclose(f11);

    // create a symbolic link "myfile13.txt" in watched to "/tmp/myfile1.txt"
    strcpy(temp_path, path);
    strcat(temp_path, "/");
    strcat(temp_path, file13);
    create_sym_link("/tmp/myfile1.txt", temp_path);

    // modify "/tmp/myfile1.txt"
    strcpy(temp_path, "/tmp/");
    strcat(temp_path, file1);
    f1 = fopen(temp_path, "a");
    fprintf(f1, "I'm masochist, I enjoy debugging C ");
    fclose(f1);

    // remove all duplicates of hard links to "myfile11.txt" only in watched directory
    strcpy(temp_path, path);
    strcat(temp_path, "/");
    strcat(temp_path, file11);
    unlink_all(temp_path);

    // check the number of hard links on "myfile11.txt"
    struct stat s;
    stat(temp_path, &s);
    printf("Number of hard links on \"myfile11.txt\": %ld\n", s.st_nlink);

    return 0;
}
