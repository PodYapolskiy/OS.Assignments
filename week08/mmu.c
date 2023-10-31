#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h> // for S_IRUSR | S_IWUSR

// define the structure for a page table entry (PTE)
struct PTE {
    bool valid;     // page is in the physical memory
    bool dirty;     // page should be written to disk
    int frame;      // frame number of the page in the RAM
    int referenced; // page is referenced / requested
};

void print_separator() {
    printf("-------------------------\n");
}

void print_table(struct PTE* page_table, int num_pages) {
    for (int j = 0; j < num_pages; j++) {
        printf("Page %d: Valid=%d, Dirty=%d, Frame=%d, Referenced=%d\n",
               j,
               page_table[j].valid,
               page_table[j].dirty,
               page_table[j].frame,
               page_table[j].referenced
        );
    }
}

void print_request(char action, int page_number) {
    printf("%s Request for page %d\n", action == 'R' ? "Read" : "Write", page_number);
}

void handle_page_fault(int signo) {
    // Signal handler for page faults
    // This handler is empty, it just wakes up the MMU.
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <number of pages> <reference string> <pager PID>\n", argv[0]);
        return 1;
    }

    int num_pages = atoi(argv[1]);
    char* reference_string = argv[2];
    pid_t pager_pid = atoi(argv[3]);

    // set up signal handler for page faults (SIGCONT)
    signal(SIGCONT, handle_page_fault);

    // open the mapped file for the page table
    int fd = open("/tmp/ex2/pagetable", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Error opening the page table file");
        return 1;
    }

    // calculate the size of the page table
    size_t page_table_size = sizeof(struct PTE) * num_pages;

    // ensure the file is large enough to hold the page table
    if (ftruncate(fd, page_table_size) == -1) {
        perror("Error truncating the page table file");
        close(fd);
        return 1;
    }

    // map the page table into memory
    struct PTE* page_table = mmap(NULL, page_table_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (page_table == MAP_FAILED) {
        perror("Error mapping the page table into memory");
        close(fd);
        return 1;
    }

    print_separator();
    printf("Initialized page table\n");
    print_table(page_table, num_pages);

    // simulate memory accesses
    for (int i = 0; i < strlen(reference_string); i += 3) {
        char mode = reference_string[i];
        int page = reference_string[i + 1] - '0'; // get integer representation

        print_separator();
        print_request(mode, page);

        // check if the page is in RAM
        if (page_table[page].valid) {
            printf("It is a valid page\n");

            // if not a read access, set the dirty bit
            if (mode == 'W') {
                page_table[page].dirty = 1;
                printf("It is a write request then set the dirty field\n");
            }
        } else {
            // page fault
            printf("It is not a valid page --> page fault\n");
            printf("Ask pager to load it from disk (SIGUSR1 signal) and wait\n");

            page_table[page].referenced = getpid(); // set referenced field
            kill(pager_pid, SIGUSR1); // simulates a page fault by signaling the pager process with SIGUSR1
            pause(); // sleep until we receive a SIGCONT from the pager

            print_separator();
            printf("MMU resumed by SIGCONT signal from pager\n");
        }

        // print the updated page table
        printf("Page table\n");
        print_table(page_table, num_pages);
        printf("\n");
    }

    printf("Done all requests.\n");
    printf("MMU sends SIGUSR1 to the pager.\n");
    printf("MMU terminates.\n");
    
    kill(pager_pid, SIGUSR1); // signal the pager to indicate we're done

    // unmap the page table and close the file
    munmap(page_table, page_table_size);
    close(fd);

    return 0;
}
