#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

// --------- //
// CONSTANTS //
// --------- //
#define EVENT_FILE "/dev/input/by-path/platform-i8042-serio-0-event-kbd"
#define OUTPUT_FILE "ex1.txt"

#define PE_MESSAGE "I passed the Exam!\n"
#define CAP_MESSAGE "Get some cappuccino!\n"
#define OS_MESSAGE "✨ LOVE OS COURSE ✨\n"

// ------- //
// HISTORY //
// ------- //
#define HISTORY_LENGTH 10
static int history[HISTORY_LENGTH] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/**
 * @brief prints history array
 */
void print_history() {
    printf("History: [");
    for (int i = 0; i < HISTORY_LENGTH; i++) {
        if (i != HISTORY_LENGTH - 1) {
            printf("%d, ", history[i]);
        } else {
            printf("%d", history[i]);
        }
    }
    printf("]\n");
}

/**
 * @brief add event code to the history
 *
 * @param event input event structure
 */
void add_to_history(struct input_event* event) {
    int key = event->code;
    for (int i = HISTORY_LENGTH - 1; i > 0; i--) {
        history[i] = history[i - 1];
    }
    history[0] = key;
}

/**
 * @brief erase current event->code value from history
 * in_row times
 *
 * @param event input event structure
 * @param in_row number of occurences
 */
void erase_from_history(struct input_event* event, int in_row) {
    int key = event->code;

    int i = 0;
    while (in_row > 0) {
        if (i == HISTORY_LENGTH) return; // if we count more char in_row than history can fit    
        if (history[i] == key) { // check if it's the char to be deleted
            history[i] = 0; // set this history cell to 0
            in_row--; // devrement in_row chars to be removed
        }
        i++; // incement index of history iteration
    }
}

// -------- //
// PRINTING //
// -------- //

/**
 * @brief outputs event to the console
 */
void print_key_event(struct input_event* event) {
    if (event->value == 0) printf("RELEASED ");
    else if (event->value == 1) printf("PRESSED ");
    else if (event->value == 2) printf("REPEATED ");
    printf("0x%04x (%d)\n", event->code, event->code);
}

/**
 * @brief outputs event to the file
 */
void output_key_event(struct input_event* event, FILE* output) {
    if (event->value == 0) fprintf(output, "RELEASED ");
    else if (event->value == 1) fprintf(output, "PRESSED ");
    else if (event->value == 2) fprintf(output, "REPEATED ");
    fprintf(output, "0x%04x (%d)\n", event->code, event->code);
}

// ------- //
// HADLING //
// ------- //

/**
 * @brief handles shortcut combinations
 * using history array absraction
 */
int handle_shortcuts(struct input_event* event) {
    // if not pressed
    if (event->value != 1) return -1;

    // termination shortcut (E+X)
    if (history[1] == KEY_E &&
        history[0] == KEY_X
        ) return 0;

    // first given shortcut (P+E)
    else if (history[1] == KEY_P &&
             history[0] == KEY_E
             ) return 1;

    // second given shortcut (C+A+P)
    else if (history[2] == KEY_C &&
             history[1] == KEY_A &&
             history[0] == KEY_P
             ) return 2;

    // custom shortcut (O+S)
    else if (history[1] == KEY_O &&
             history[0] == KEY_S
             ) return 3;

    // just not found output
    return -1;
}

int main() {
    // file descriptor of keybord actions
    int fd = open(EVENT_FILE, O_RDONLY);
    if (fd == -1) {
        perror("Error opening device");
        return EXIT_FAILURE;
    }

    // output file
    FILE* output = fopen(OUTPUT_FILE, "a");
    if (output == NULL) {
        perror("Error opening output file");
        close(fd);
        return EXIT_FAILURE;
    }

    // print available shortcuts
    printf("Available Shortcuts:\n\tE+X (%d+%d)\n\tP+E (%d+%d)\n\tC+A+P (%d+%d+%d)\n\tO+S (%d+%d)\n",
           KEY_E, KEY_X,
           KEY_P, KEY_E,
           KEY_C, KEY_A, KEY_P,
           KEY_O, KEY_S
    );

    // variable to count the number of repeating chars
    int in_row = 1;

    // main infinite loop
    while (true) {
        struct input_event event; // stucture for keep info of input event
        ssize_t bytesRead = read(fd, &event, sizeof(struct input_event));

        // if it's not a key event, skip it
        if (event.type != EV_KEY) continue;

        // check whether we should consider this key event or not
        if (event.value != 0 && // if not RELEASED
            event.value != 1 && // if not PRESSED
            event.value != 2    // if not REPEATED
            ) continue;

        // if read bytes don't correspond to input_event type
        if (bytesRead != sizeof(struct input_event)) continue;

        // transform read bytes to 
        if (event.value == 1) { // pressed
            add_to_history(&event);
        } else if (event.value == 0) { // released
            erase_from_history(&event, in_row); // erase the last value when it's being removed
            in_row = 1;
        } else if (event.value == 2) { // repeated
            add_to_history(&event);
            in_row++;
        }

        // we are able to see the history on this moment
        // print_history();

        // print event to stdout
        print_key_event(&event);

        // save to output file
        output_key_event(&event, output);

        // handle proper shortcuts
        int res = handle_shortcuts(&event);
        if (res == 0) {
            break; // get out of infinite loop -> terminatation
        } else if (res == 1) {
            printf(PE_MESSAGE);
            fprintf(output, PE_MESSAGE);
        } else if (res == 2) {
            printf(CAP_MESSAGE);
            fprintf(output, CAP_MESSAGE);
        } else if (res == 3) {
            printf(OS_MESSAGE);
            fprintf(output, OS_MESSAGE);
        }

        // flush the standart output
        fflush(stdout);

        // flush the file
        fflush(output);
    }

    // close file and descriptor
    fclose(output);
    close(fd);

    return EXIT_SUCCESS;
}
