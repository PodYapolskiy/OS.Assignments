#include <stdio.h>
#include <string.h>

#define MAX_NAME 64
#define MAX_PATH 2048
#define MAX_DATA 1024
#define MAX_FILES 256 // because unsigned char nf max is 256
#define MAX_DIRS 256 // just random number (no specification)

struct Directory;
struct File;

typedef struct File {
    short int id;
    int size;
    char name[MAX_NAME];
    char data[MAX_DATA];
    struct Directory* directory; // parent directory
} File;

typedef struct Directory {
    unsigned char nf; // number of files
    unsigned char nd; // number of sub-directories
    char name[MAX_NAME];
    char path[MAX_PATH]; // full path
    File* files[MAX_FILES];
    struct Directory* sub_dirs[MAX_DIRS];
} Directory;

// --- manipulations on files ---
/// @brief overwrites file content with str
/// @param file file to overwrite
/// @param str string to write
void overwrite_file(File* file, const char* str) {
    strcpy(file->data, str);
    file->size = strlen(str);
}

/// @brief appends str content to the end of the file
/// @param file file to append content
/// @param str content
void append_to_file(File* file, const char* str) {
    strcat(file->data, str);
    file->size += strlen(str);
}

/// @brief prints the full file's path
/// @param file
void printp_file(File* file) {
    char temp_path[MAX_PATH];
    strcpy(temp_path, file->directory->path);
    strcat(temp_path, file->name);
    printf("%s\n", temp_path);
}

// --- adding ---
/// @brief adds file to the directory
/// @param file
/// @param dir
void add_file(File* file, Directory* dir) {
    dir->files[dir->nf++] = file;
    file->directory = dir;
}

/// @brief adds sub-directory dir1 to the directory dir2
/// @param dir1 
/// @param dir2
void add_dir(Directory* dir1, Directory* dir2) {
    if (dir1 && dir2) {
        dir2->sub_dirs[dir2->nd++] = dir1;
        char temp_path[MAX_PATH];
        if (strcmp(dir2->path, "/")) {
            strcpy(temp_path, dir2->path);
            // strcat(temp_path, "/");
            strcat(temp_path, dir1->name);
            strcpy(dir1->path, temp_path);
        } else {
            strcpy(temp_path, "/");
            strcat(temp_path, dir1->name);
            strcat(temp_path, "/");
            strcpy(dir1->path, temp_path);
        }
    }
}

// --- showing ---
void show_file(File* file) {
    printf("%s", file->name);
}

void show_dir(Directory* dir) {
    printf("\nDIRECTORY\n");
    printf("path: %s\n", dir->path);
    printf("files:\n");
    printf("[");
    for (int i = 0; i < dir->nf; i++) {
        show_file(dir->files[i]);
        if (i != dir->nf - 1) {
            printf(", ");
        }
    }
    printf("]\n");

    printf("directories:\n");
    printf("{");
    for (int i = 0; i < dir->nd; i++) {
        show_dir(dir->sub_dirs[i]);
    }
    printf("}\n");
}

int main() {
    static int GLOBAL_FILE_ID = 0;
    Directory root = {.name = "root", .path = "/"};
    Directory home = {.name = "home"};
    Directory bin = {.name = "bin"};
    add_dir(&home, &root);
    add_dir(&bin, &root);

    File bash = {.id = GLOBAL_FILE_ID++, .name = "bash"};
    add_file(&bash, &bin);

    File ex3_1 = {.id = GLOBAL_FILE_ID++, .name = "ex3_1.c"};
    File ex3_2 = {.id = GLOBAL_FILE_ID++, .name = "ex3_2.c"};
    add_file(&ex3_1, &home);
    add_file(&ex3_2, &home);

    overwrite_file(&ex3_1, "int printf(const char* format, ...);\n");
    overwrite_file(&ex3_2, "//This is a comment in C language\n");

    append_to_file(&bash, "Bourne Again Shell\n");
    append_to_file(&ex3_1, "int main(){printf(\"Hello World!\")}\n");

    show_dir(&root); // show "tree" starting with root
    printp_file(&ex3_1); // output ex3_1.c's full path
    printf("%s\n", ex3_1.data); // output bash's content

    return 0;
}
