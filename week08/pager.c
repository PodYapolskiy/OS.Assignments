#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h> // for S_IRUSR | S_IWUSR
#include <time.h> // for time(NULL)

#define STR_SIZE 8

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
        printf("Page %d ---> valid=%d, dirty=%d, frame=%d, referenced=%d\n",
               j,
               page_table[j].valid,
               page_table[j].dirty,
               page_table[j].frame,
               page_table[j].referenced
        );
    }
}

void print_RAM(char RAM[][STR_SIZE], int num_frames) {
    printf("RAM array\n");
    for (int i = 0; i < num_frames; i++) {
        printf("Frame %d ---> %s\n", i, RAM[i]);
    }
}

void print_disk(char disk[][STR_SIZE], int num_pages) {
    printf("Disk array\n");
    for (int i = 0; i < num_pages; i++) {
        printf("Page %d ----> %s\n", i, disk[i]);
    }
}

char* generate_string() {
    char* result = (char*) malloc(STR_SIZE * sizeof(char));

    for (int i = 0; i < STR_SIZE - 1; i++) {
        result[i] = '!' + (abs(rand()) % 95);
    }
    result[STR_SIZE - 1] = '\0';

    return result;
}

int get_free_frame_index(char RAM[][STR_SIZE], int num_frames) {
    /**
     * @brief Return index of the first free frame in the RAM
     * if there are no such, return -1
     */
    for (int i = 0; i < num_frames; i++) {
        if (strcmp(RAM[i], "") == 0) // if RAM[i] == "" 
            return i;
    }
    return -1;
}

int get_victim_page_index(struct PTE* page_table, int num_pages) {
    /**
     * @brief Return index of the victim page which has a frame in the RAM
     */
    int index = rand() % num_pages;
    while (page_table[index].frame == -1) {
        index = rand() % num_pages;
    }
    return index;
}

void handle_page_request(int signo) {
    // Signal handler for page requests (SIGUSR1)
    // This handler handles page loading to RAM.
    // It's empty in this simulation.
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <number of pages> <number of frames>\n", argv[0]);
        return 1;
    }
    srand(time(NULL)); // to make random variables random

    int num_pages = atoi(argv[1]);
    int num_frames = atoi(argv[2]);

    // array of strings representing RAM and disk
    char RAM[num_frames][STR_SIZE];
    char disk[num_pages][STR_SIZE];

    // set up signal handler for page requests (SIGUSR1)
    signal(SIGUSR1, handle_page_request);

    // create and initialize the page table in a mapped file
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

    // initialize the page table
    for (int i = 0; i < num_pages; i++) {
        page_table[i].valid = false;
        page_table[i].dirty = false;
        page_table[i].frame = -1;
        page_table[i].referenced = 0;
    }
    print_separator();
    printf("Initialized page table\n");
    print_table(page_table, num_pages);

    // initialize the RAM
    for (int i = 0; i < num_frames; i++) {
        strcpy(RAM[i], "");
    }
    print_separator();
    printf("Initialized RAM\n");
    print_RAM(RAM, num_frames);

    // initialize the disk
    for (int i = 0; i < num_pages; i++) {
        strcpy(disk[i], generate_string());
    }
    print_separator();
    printf("Initialized disk\n");
    print_disk(disk, num_pages);

    // pause after initialization
    pause();

    // variable to assess performance and see the whole picture
    int total_disk_accesses = 0;

    while (true) { // main pager loop
        print_separator();
        printf("A disk access request from MMU Process (pid=%d)\n", getpid());

        // check if there is a page request from the MMU (non-zero referenced field)
        pid_t mmu_pid = -1;
        int page_index = -1;
        for (int i = 0; i < num_pages; i++) {
            if (page_table[i].referenced) {
                page_index = i;
                mmu_pid = page_table[i].referenced;

                printf("Page %d is referenced\n", page_index);
                break;
            }
        }

        // if no page requested, terminate the pager process
        if (page_index == -1 || mmu_pid == -1) {
            break;
        }

        // allocation, the most complicated part)
        int free_frame_index = get_free_frame_index(RAM, num_frames);
        if (free_frame_index != -1) {
            // allocate a free frame to the requested page
            printf("We can allocate it to free frame %d\n", free_frame_index);
        } else {
            // If there are no free frames, choose a random victim page
            printf("We do not have free frames in RAM\n");

            // randomly choose 
            int victim_page_index = get_victim_page_index(page_table, num_pages);
            printf("Chose a random victim page %d\n", victim_page_index);

            // in this moment can assume that frame of chosen victime page is available
            free_frame_index = page_table[victim_page_index].frame;
            printf("Replace/Evict it with page %d to be allocated to frame %d\n", page_index, free_frame_index);
            if (page_table[victim_page_index].dirty) {
                // simulate writing the page to disk
                total_disk_accesses++;
                printf("Writing Page %d to disk\n", victim_page_index);

                // writing the page to disk by copying the change from RAM to disk array
                strcpy(disk[page_index], RAM[free_frame_index]);
            }

            // update the victim page's info
            page_table[victim_page_index].valid = 0;
            page_table[victim_page_index].dirty = 0;
            page_table[victim_page_index].referenced = 0;
            page_table[victim_page_index].frame = -1;
        }

        // allocate the free frame to the requested page
        page_table[page_index].valid = 1;
        page_table[page_index].frame = free_frame_index;
        page_table[page_index].referenced = 0;

        // copying data from disk to RAM
        printf("Copy data from the disk (page=%d) to RAM (frame=%d)\n", page_index, free_frame_index);
        strcpy(RAM[free_frame_index], disk[page_index]);

        // print updated RAM
        print_RAM(RAM, num_frames);
        printf("disk accesses is %d so far\n", ++total_disk_accesses);

        // (debug purposes)
        // Print the updated page table
        // print_separator();
        // print_table(page_table, num_pages);

        // resume MMU
        kill(mmu_pid, SIGCONT);
        printf("Resume MMU\n");

        pause();
    }

    // Print the total number of disk accesses
    printf("Total Disk Accesses: %d\n", total_disk_accesses);
    printf("Pager terminated\n");

    // Unmap the page table and close the file
    munmap(page_table, page_table_size);
    close(fd);

    return 0;
}
