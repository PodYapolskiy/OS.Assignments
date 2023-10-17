#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>

#define PS_MAX 10

// holds the scheduling data of one process
typedef struct {
    int idx;   // process idx (index)
    int at,    // arrival time
        bt,    // burst time
        rt,    // response time
        wt,    // waiting time
        ct,    // completion time
        tat;   // turnaround time
    int burst; // remaining burst (this should decrement when the process is being executed)
} ProcessData;

// the idx of the running process
int running_process = -1; // -1 means no running processes

// the total time of the timer
unsigned total_time; // should increment one second at a time by the scheduler

// data of the processes
ProcessData data[PS_MAX]; // array of process data

// array of all process pids
pid_t ps[PS_MAX]; // zero valued pids - means the process is terminated or not created yet

// size of data array
unsigned data_size;

// extract the data of processes from the {file} 
// and store them in the array {data}
void read_file(FILE* file) {
    char line[10];
    fscanf(file, "%[^\n]", line); // read the header line

    data_size = 0; // explicitly indicate that size of data is 0

    int result, idx, at, bt;
    while ((result = fscanf(file, "%d %d %d", &idx, &at, &bt)) != EOF) {
        data[data_size].idx = idx;
        data[data_size].at = at;
        data[data_size].bt = bt;
        data[data_size].burst = bt; // this field will change
        data_size++;
    }

    // initialize ps array to zeros (the process is terminated or not created yet)
    for (int i = 0; i < data_size; i++) {
        ps[i] = 0;
    }

    fclose(file);
}

// send signal SIGCONT to the worker process
void resume(pid_t process) {
    int status;
    waitpid(process, &status, 0); // determine status of the process

    // if it is not in one of the states (1.not created yet or 2.terminated)
    if (process != 0 && !WIFEXITED(status)) {
        kill(process, SIGCONT);
    }
}

// send signal SIGTSTP to the worker process
void suspend(pid_t process) {
    int status;
    waitpid(process, &status, 0); // determine status of the process

    // if it is not in one of the states (1.not created yet or 2.terminated)
    if (process != 0 && !WIFEXITED(status)) {
        kill(process, SIGTSTP);
    }
}

// send signal SIGTERM to the worker process
void terminate(pid_t process) {
    int status;
    waitpid(process, &status, 0); // determine status of the process

    // if it is not in one of the states (1.not created yet or 2.terminated)
    if (process != 0 && !WIFEXITED(status)) {
        kill(process, SIGTERM);
    }
}

// create a process using fork
void create_process(int new_process) {
    // stop the running process
    suspend(ps[running_process]); // ?
//    kill(ps[running_process], SIGSTOP);

    // fork a new process and add it to ps array
    ps[new_process] = fork();

    if (ps[new_process] != 0) {
        // now the idx of the running process is new_process 
        running_process = new_process;

        // the scheduler process runs the program "./worker {new_process}" 
        char program_name[] = "./worker";

        // convert process number to a string
        char process_number[16];
        sprintf(process_number, "%d", new_process);

        // compose arguments and use execvp
        char* args[] = {program_name,  process_number};
        execvp(program_name, args);
    }
}

// find next process for running
ProcessData find_next_process() {
    int location = 0; // location of next process in {data} array

    for (int i = 0; i < data_size; i++) {
        // find location of the next process to run from the {data} array
        // considering the scheduling algorithm FCFS
        if (data[i].at <= total_time) { // TODO: probably check on completness
            location = i;
            break;
        }
    }

    // if next_process did not arrive so far, 
    // then we recursively call this function after incrementing total_time
    if (data[location].at > total_time) {
        printf("Scheduler: Runtime: %u seconds.\nProcess %d: has not arrived yet.\n", total_time, location);

        total_time++; // increment time
        return find_next_process();
    }

    return data[location]; // data of the next process
}

// reports the metrics and simulation results
void report() {
    printf("Simulation results.....\n");
    int sum_wt = 0;
    int sum_tat = 0;
    for (int i = 0; i < data_size; i++) {
        printf("process %d: \n", i);
        printf("	at=%d\n", data[i].at);
        printf("	bt=%d\n", data[i].bt);
        printf("	ct=%d\n", data[i].ct);
        printf("	wt=%d\n", data[i].wt);
        printf("	tat=%d\n", data[i].tat);
        printf("	rt=%d\n", data[i].rt);
        sum_wt += data[i].wt;
        sum_tat += data[i].tat;
    }

    printf("data size = %d\n", data_size);
    float avg_wt = (float) sum_wt / data_size;
    float avg_tat = (float) sum_tat / data_size;
    printf("Average results for this run:\n");
    printf("	avg_wt=%f\n", avg_wt);
    printf("	avg_tat=%f\n", avg_tat);
}

// check whether there are processes to execute
void check_burst() {
    for (int i = 0; i < data_size; i++)
        if (data[i].burst > 0)
            return;

    // report simulation results
    report();

    // terminate the scheduler :)
    exit(EXIT_SUCCESS);
}

// is called every one second as handler for SIGALRM signal
void schedule_handler(int signum) {
    total_time++; // increment the total time

    ProcessData process_data;
    if (running_process != -1) {
        /*
        1. If there is a worker process running, then decrement its remaining burst:
        */
        process_data = data[running_process];
        process_data.burst--;

        printf("Scheduler: Runtime: %d seconds\n", total_time);
        printf("Process %d is running with %d seconds left\n", running_process, process_data.burst);

        /* // TODO: make it work blyat
        1.A. If the worker process finished its burst time, then the scheduler should terminate the running process:
        */
        if (process_data.burst == 0) {
            printf("Scheduler: Terminating Process %d (Remaining Time: %d)\n", running_process, process_data.burst);
            terminate(ps[running_process]);
            // kill(ps[running_process], SIGTERM); // ?

            /*
            then the scheduler waits for its termination and there will be no running processes anymore.
            Since the process is terminated, we can calculate its metrics {ct, tat, wt}
            */
            int status;
            waitpid(ps[running_process], &status, 0);
            running_process = -1;

            if (WIFEXITED(status)) {
                process_data.ct = total_time;
                process_data.rt = total_time - process_data.bt - process_data.at;
                process_data.tat = process_data.ct - process_data.at;
                process_data.wt = process_data.tat - process_data.bt;
            }
        }
    }

    /*
    2. After that, we need to find the next process to run {next_process}.
    */
    check_burst(); // check the bursts of all processes
    process_data = find_next_process(); // find new process
    int new_process = process_data.idx;

    /*
    3. If {next_process} is not running, then we need to check the current process
    3.A. If the current process is running, then we should stop the current running process
    */
    // if (ps[new_process] == 0 && ps[running_process] != 0) {
    if (ps[new_process] == 0 && running_process != -1) {
        suspend(running_process);
        printf("Scheduler: Stopping Process %d (Remaining Time: %d)\n", running_process, data[running_process].burst);
    }

    /*
    3.B. set current process as the running process.
    */
    running_process = new_process; // ? dublicate

    /*
    3.C.1. then create a new process for {running_process}
    Here we have the first response to the process {running_process} and we can calculate the metric {rt}
    */
    create_process(running_process);
    printf("Scheduler: Starting Process %d (Remaining Time: %d)\n", running_process, data[running_process].burst);

    /*
    3.C.2. or resume the process {running_process} if it is stopped
    */
//    printf("huy\n");
    int status;
    waitpid(ps[running_process], &status, WUNTRACED); // breaks here
//    printf("huy\n");

    if (WIFSTOPPED(status)) {
        resume(running_process);
        printf("Scheduler: Resuming Process %d (Remaining Time: %d)\n", running_process, data[running_process].burst);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Data file was not specified\n");
        exit(EXIT_FAILURE);
    }

    // read the data file
    FILE* in_file = fopen(argv[1], "r");
    if (in_file == NULL) {
        printf("File is not found or cannot open it!\n");
        exit(EXIT_FAILURE);
    } else {
        read_file(in_file);
    }

    // set a timer
    struct itimerval timer;

    // the timer goes off 1 second after reset
    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;

    // timer increments 1 second at a time
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;

    // this counts down and sends SIGALRM to the scheduler process after expiration.
    setitimer(ITIMER_REAL, &timer, NULL);

    // register the handler for SIGALRM signal
    signal(SIGALRM, schedule_handler);

    // wait till all processes finish
    while (1); // infinite loop
}