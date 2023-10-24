#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MEMORY_SIZE 10000000

// constants to distinct allocation algorithms
#define FIRST_FIT_ALGORITHM 0
#define BEST_FIT_ALGORITHM 1
#define WORST_FIT_ALGORITHM 2

unsigned int memory[MEMORY_SIZE]; // global memory array

// function to initialize memory to all zeros
void initializeMemory() {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        memory[i] = 0;
    }
}

/////////////////////////
// First Fit Algorithm //
/////////////////////////
void allocate_first_fit(unsigned int adrs, int size) {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (memory[i] == 0) { // try only if it is free
            int j;
            for (j = i; j < i + size; j++) {
                if (memory[j] != 0) {
                    i = j; // jump to skip repetitions
                    break;  // not enough contiguous free cells
                }
            }
            if (j == i + size) { // success and allocation
                for (j = i; j < i + size; j++) {
                    memory[j] = adrs;
                }
                return; // we can leave
            }
        }
    }
}

/////////////////////////
// Best Fit Algorithm //
/////////////////////////
void allocate_best_fit(unsigned int adr, int size) {
    int best_fit_index = -1;
    int best_fit_size = MEMORY_SIZE + 1;

    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (memory[i] == 0) {  // check if the block is free
            int block_size = 0;

            // calculate the size of the free block
            while (i < MEMORY_SIZE && memory[i] == 0) {
                block_size++;
                i++;
            }

            // check if this block is a better fit
            if (block_size >= size && block_size < best_fit_size) {
                best_fit_size = block_size;
                best_fit_index = i - block_size;
            }
        }
    }

    if (best_fit_index >= 0) { // allocate the block
        for (int i = best_fit_index; i < best_fit_index + size; i++) {
            memory[i] = adr;
        }
    }
}

/////////////////////////
// Worst Fit Algorithm //
/////////////////////////
void allocate_worst_fit(unsigned int adr, int size) {
    int worst_fit_index = -1;
    int worst_fit_size = -1;

    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (memory[i] == 0) {  // check if the block is free
            int block_size = 0;

            // calculate the size of the free block
            while (i < MEMORY_SIZE && memory[i] == 0) {
                block_size++;
                i++;
            }

            // check if this block is the worst fit
            if (block_size > worst_fit_size) {
                worst_fit_size = block_size;
                worst_fit_index = i - block_size;
            }
        }
    }

    if (worst_fit_index >= -1) { // allocate the block
        for (int i = worst_fit_index; i < worst_fit_index + size; i++) {
            memory[i] = adr;
        }
    }
}

// function to clear memory associated with a given address
void clear(unsigned int adrs) {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (memory[i] == adrs) {
            memory[i] = 0;
        }
    }
}

int main(void) {
    // iterate through all possible allocation algorithms
    for (int algo_num = 0; algo_num < 3; algo_num++) {
        char algorithm_name[64]; // string to output in console
        void (*algorithm) (unsigned int, int); // pointer to algorithm to choose

        switch (algo_num) {
            {
            case FIRST_FIT_ALGORITHM:
                algorithm = allocate_first_fit;
                strcpy(algorithm_name, "First Fit");
                break;

            case BEST_FIT_ALGORITHM:
                algorithm = allocate_best_fit;
                strcpy(algorithm_name, "Best Fit");
                break;

            case WORST_FIT_ALGORITHM:
                algorithm = allocate_worst_fit;
                strcpy(algorithm_name, "Worst Fit");
                break;

            default:
                algorithm = allocate_first_fit;
                strcpy(algorithm_name, "First Fit");
                break;
            }
        }

        // open the input file
        FILE* file = fopen("queries.txt", "r");
        if (file == NULL) {
            printf("Error: Could not open the input file.\n");
            return 1;
        }
        initializeMemory(); // clean it up and fill with zeros

        char line[128]; // temp line
        int queries_amount = 0; // counter for queries invoked
        clock_t start_time = clock(); // start a clock for particular algo

        // read and process queries from the file
        while (fgets(line, sizeof(line), file)) {
            if (strncmp(line, "allocate", 8) == 0) {
                unsigned int adrs;
                int size;
                sscanf(line + 9, "%u %d", &adrs, &size);

                algorithm(adrs, size); // apply chosen algorithm
                queries_amount++; // increment accordingly

            } else if (strncmp(line, "clear", 5) == 0) {
                unsigned int adrs;
                sscanf(line + 6, "%u", &adrs);
                clear(adrs);
            } else if (strncmp(line, "end", 3) == 0) {
                break;
            }
        }

        // end particular time and calculate metrics
        clock_t end_time = clock();
        double execution_time = (double) (end_time - start_time) / CLOCKS_PER_SEC;
        double throughput = queries_amount / execution_time;

        // printf("Execution time of %s algorithm: %f\n", algorithm_name, execution_time);
        printf("Throughput of %s algorithm: %f\n", algorithm_name, throughput);

        fclose(file);
    }

    return 0;
}