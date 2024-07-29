# include <stdio.h>
# include <stdlib.h>
# include <limits.h>
# include <string.h>

// Define structure for process
typedef struct {
    // Process fields given
    int processID;
    int arrivalTime;
    int burstTime;
    int priority;
    int timeQuantum;

    // Process fields to be computed
    int waitingTime;
    int turnaroundTime;
    int remainingTime;
    int completionTime;
} Process;

// Function to sort Processes by arrival time
int sortProcessArrival(const void *a, const void *b) {
    Process *p1 = (Process*)a;
    Process *p2 = (Process*)b;
    if (p1->arrivalTime == p2->arrivalTime) {
        return p1->processID - p2->processID;
    }
    return p1->arrivalTime - p2->arrivalTime;
}

// Function to sort Processes by priority
int sortProcessPriority(const void *a, const void *b) {
    Process *p1 = (Process*)a;
    Process *p2 = (Process*)b;
    if (p1->arrivalTime == p2->arrivalTime) {
        return p1->priority - p2->priority;
    }
    return p1->arrivalTime - p2->arrivalTime;
}

// Function to sort Processes in descending order
int sortProcessDescend(const void *A, const void *B) {
    Process *p1 = (Process*)A;
    Process *p2 = (Process*)B;
    return p1->processID - p2->processID;
}

// Function to calculate average waiting/turnaround times and throughput of Processes
void findAvgTimes(Process p[], int n, float *avgWaitingTime, float *avgTurnaroundTime) {
    int totalWaitingTime = 0, totalTurnaroundTime = 0;
    for (int i = 0; i < n; i++) {
        totalWaitingTime += p[i].waitingTime;
        totalTurnaroundTime += p[i].turnaroundTime;
    }
    *avgWaitingTime = (float)totalWaitingTime / (float)n;
    *avgTurnaroundTime = (float)totalTurnaroundTime / (float)n;
}

// Function to print Gantt Chart of Processes
void printGanttChart(Process p[], int n) {
    int time = 0;
    printf("Gantt Chart: \n");
    for (int i = 0; i < n; i++) {
        if (time < p[i].arrivalTime) {
            printf("[ %d ] -- IDLE -- [ %d ] \n", time, p[i].arrivalTime);
            time = p[i].arrivalTime;
        }
        printf("[ %d ] -- %d -- [ %d ] \n", time, p[i].processID, time + p[i].burstTime);
        time += p[i].burstTime;
    }
}

// Function to print Gantt Chart of Processes
void printGanttChartRR(Process p[], int n, int quantum) {
    int time = 0;
    int processesRemaining = n;
    int *remainingBurstTimes = (int *)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        remainingBurstTimes[i] = p[i].burstTime;
    }
    printf("Gantt Chart is:\n");
    while (processesRemaining > 0) {
        int allProcessesIdle = 1;
        for (int i = 0; i < n; i++) {
            if (remainingBurstTimes[i] > 0 && p[i].arrivalTime <= time) {
                allProcessesIdle = 0;
                if (remainingBurstTimes[i] > quantum) {
                    printf("[ %d ]-- %d --[ %d ]\n", time, p[i].processID, time + quantum);
                    time += quantum;
                    remainingBurstTimes[i] -= quantum;
                } else {
                    printf("[ %d ]-- %d --[ %d ]\n", time, p[i].processID, time + remainingBurstTimes[i]);
                    time += remainingBurstTimes[i];
                    remainingBurstTimes[i] = 0;
                    processesRemaining--;
                }
            }
        }
        if (allProcessesIdle) {
            printf("[ %d ]-- IDLE --[ %d ]\n", time, time + 1);
            time++;
        }
    }
    free(remainingBurstTimes);
}

// Function declaration for "First Come First Serve (FCFS)"
void findFCFS(Process p[], int n) {
    float avgWaitingTime, avgTurnaroundTime, throughput;
    qsort(p, n, sizeof(Process), sortProcessArrival);

    // Waiting time for first process is 0
    p[0].waitingTime = 0;

    // Calculate waiting time
    p[0].completionTime = p[0].arrivalTime + p[0].burstTime;
    for (int i = 1; i < n; i++) {
        p[i].waitingTime = p[i - 1].completionTime - p[i].arrivalTime;
        if (p[i].waitingTime < 0) {
            p[i].waitingTime = 0;
        }
        p[i].completionTime = p[i].arrivalTime + p[i].waitingTime + p[i].burstTime;
    }

    // Calculate turnaround time
    for (int i = 0; i < n; i++) {
        p[i].turnaroundTime = p[i].burstTime + p[i].waitingTime;
    }

    // Call function to calculate average waiting/turnaround times and throughput
    findAvgTimes(p, n, &avgWaitingTime, &avgTurnaroundTime);

    // Print Process information
    Process temp[n];
    memcpy(temp, p, sizeof(Process) * n);
    qsort(temp, n, sizeof(Process), sortProcessDescend);
    printf("\n------------------- FCFS -------------------\n");
    printf("Process ID | Waiting Time | Turnaround Time \n");
    for (int i = 0; i < n; i++) {
        printf("%10d | %12d | %15d \n", temp[i].processID, temp[i].waitingTime, temp[i].turnaroundTime);
    }

    // Print Gantt Chart
    printGanttChart(p, n);

    // Print calculated averages and throughput
    throughput = (float)n / (p[n - 1].completionTime);
    // throughput = (float)n / (p[n - 1].turnaroundTime + p[n - 1].arrivalTime);
    printf("Average Waiting Time: %.2f \n", avgWaitingTime);
    printf("Average Turnaround Time: %.2f \n", avgTurnaroundTime);
    printf("Throughput: %.2f \n\n", throughput);
}

// Function declaration for "Shortest Remaining Time First" (SJF with preemption)
void findSJF(Process p[], int n) {
    int completed = 0, currTime = 0, shortest = 0;
    int finishTime, minRemainingTime = INT_MAX, isCompleted[n];
    float avgWaitingTime, avgTurnaroundTime, throughput;
    qsort(p, n, sizeof(Process), sortProcessArrival);

    // Copy burst time into remaining time
    for (int i = 0; i < n; i++) {
        p[i].remainingTime = p[i].burstTime;
        isCompleted[i] = 0;
    }
    
    // Complete all processes
    while (completed != n) {
        for (int j = 0; j < n; j++) {
            if ((p[j].arrivalTime <= currTime) && (isCompleted[j] == 0) 
            && (p[j].remainingTime < minRemainingTime) && (p[j].remainingTime > 0)) {
                minRemainingTime = p[j].remainingTime;
                shortest = j;
            }
        }
        if (minRemainingTime == INT_MAX) {
            currTime++;
            continue;
        }

        // Reduce remaining time by one and update minimum
        p[shortest].remainingTime--;
        minRemainingTime = p[shortest].remainingTime;
        if (minRemainingTime == 0) {
            minRemainingTime = INT_MAX;
        }

        // If processes are fully exectued
        if (p[shortest].remainingTime == 0) {
            p[shortest].completionTime = currTime;
            p[shortest].turnaroundTime = p[shortest].completionTime - p[shortest].arrivalTime;
            // Increment completed counter
            isCompleted[shortest] = 1;
            completed++;
            finishTime = currTime + 1;

            // Calculate waiting time
            p[shortest].waitingTime = finishTime - p[shortest].burstTime - p[shortest].arrivalTime;
            if (p[shortest].waitingTime < 0) {
                p[shortest].waitingTime = 0;
            }
        }
        // Increment time counter
        currTime++;
    }

    // Calculate turnaround time
    for (int i = 0; i < n; i++) {
        p[i].turnaroundTime = p[i].burstTime + p[i].waitingTime;
    }

    // Call function to calculate average times and throughput
    findAvgTimes(p, n, &avgWaitingTime, &avgTurnaroundTime);

    // Print process information
    Process temp[n];
    memcpy(temp, p, sizeof(Process) * n);
    qsort(temp, n, sizeof(Process), sortProcessArrival);
    printf("\n------------------- SJF -------------------\n");
    printf("Process ID | Waiting Time | Turnaround Time \n");
    for (int i = 0; i < n; i++) {
        printf("%10d | %12d | %15d \n", p[i].processID, p[i].waitingTime, p[i].turnaroundTime);
    }
    
    // Print Gantt Chart
    printGanttChart(p, n);

    // Print calculated averages and throughput
    throughput = (float)n / currTime;
    printf("Average Waiting Time: %.2f \n", avgWaitingTime);
    printf("Average Turnaround Time: %.2f \n", avgTurnaroundTime);
    printf("Throughput: %.2f \n\n", throughput);
}

// Function declaration for "Priority Scheduling (PS)" without preemption
void findPriority(Process p[], int n) {
    float avgWaitingTime, avgTurnaroundTime, throughput;
    qsort(p, n, sizeof(Process), sortProcessPriority);

    // Calculate waiting time and turnaround time
    int currTime = 0;
    for (int i = 0; i < n; i++) {
        if (currTime < p[i].arrivalTime) {
            currTime = p[i].arrivalTime;
        }
        p[i].waitingTime = currTime - p[i].arrivalTime;
        currTime += p[i].burstTime;
        p[i].completionTime = currTime;
    }

    // Calculate turnaround time
    for (int i = 0; i < n; i++) {
        p[i].turnaroundTime = p[i].burstTime + p[i].waitingTime;
    }

    // Call function to calculate average waiting/turnaround times and throughput
    findAvgTimes(p, n, &avgWaitingTime, &avgTurnaroundTime);

    // Print Process information
    Process temp[n];
    memcpy(temp, p, sizeof(Process) * n);
    qsort(temp, n, sizeof(Process), sortProcessDescend);
    printf("\n------------------- Priority -------------------\n");
    printf("Process ID | Waiting Time | Turnaround Time \n");
    for (int i = 0; i < n; i++) {
        printf("%10d | %12d | %15d \n", temp[i].processID, temp[i].waitingTime, temp[i].turnaroundTime);
    }

    // Print Gantt Chart
    printGanttChart(p, n);

    // Print calculated averages and throughput
    throughput = (float)n / currTime;
    printf("Average Waiting Time: %.2f \n", avgWaitingTime);
    printf("Average Turnaround Time: %.2f \n", avgTurnaroundTime);
    printf("Throughput: %.2f \n\n", throughput);
}

// Function declaration for "Round Robin (RR)" with the specified quantum
void findRoundRobin(Process p[], int n, int quantum) {
    int currTime = 0, completed = 0;
    int *queue = (int*)malloc(n * sizeof(int));
    int front = 0, back = 0;
    qsort(p, n, sizeof(Process), sortProcessArrival);
    float avgWaitingTime, avgTurnaroundTime, throughput;

    // Initialize remaining time and waiting time for each process
    for (int i = 0; i < n; i++) {
        p[i].remainingTime = p[i].burstTime;
        p[i].waitingTime = 0;
    }
    
    // Start with first process in queue
    queue[back++] = 0; 

    // Loop until all processes are completed
    while (completed < n) {
        int pIndex = queue[front++];
        // Wrap around to avoid overflow
        front %= n;

        // Check if the remaining time is greater than quantum
        if (p[pIndex].remainingTime > quantum) {
            currTime += quantum;
            p[pIndex].remainingTime -= quantum;
        } else {
            // Complete process
            currTime += p[pIndex].remainingTime;
            p[pIndex].remainingTime = 0;
            p[pIndex].completionTime = currTime;
            p[pIndex].turnaroundTime = p[pIndex].completionTime - p[pIndex].arrivalTime;
            p[pIndex].waitingTime = p[pIndex].turnaroundTime - p[pIndex].burstTime;
            completed++;
        }

        // Add new processes to queue that have arrived by current time
        for (int i = 0; i < n; i++) {
            if (i != pIndex && p[i].arrivalTime <= currTime && p[i].remainingTime > 0) {
                int exists = 0;
                for (int j = front; j != back; j = (j + 1) % n) {
                    if (queue[j] == i) {
                        exists = 1;
                        break;
                    }
                }
                if (!exists) {
                    queue[back++] = i;
                    // Wrap around to avoid overflow
                    back %= n;
                }
            }
        }

        // Re-add current process to queue if it is not finished
        if (p[pIndex].remainingTime > 0) {
            queue[back++] = pIndex;
            back %= n;  // Wrap around to avoid overflow
        }

        // If the queue is empty, add the next available process
        if (front == back && completed < n) {
            for (int i = 0; i < n; i++) {
                if (p[i].remainingTime > 0) {
                    queue[back++] = i;
                    back %= n;
                    currTime = p[i].arrivalTime;
                    break;
                }
            }
        }
    }
    free(queue);

    // Call function to calculate average waiting/turnaround times and throughput
    findAvgTimes(p, n, &avgWaitingTime, &avgTurnaroundTime);

    // Print Process information
    Process temp[n];
    memcpy(temp, p, sizeof(Process) * n);
    qsort(temp, n, sizeof(Process), sortProcessArrival);
    printf("\n----------------- Round Robin -----------------\n");
    printf("Process ID | Waiting Time | Turnaround Time \n");
    for (int i = 0; i < n; i++) {
        printf("%10d | %12d | %15d \n", temp[i].processID, temp[i].waitingTime, temp[i].turnaroundTime);
    }

    // Print Gantt Chart
    printGanttChartRR(p, n, quantum);

    // Print calculated averages and throughput
    throughput = (float)n / currTime;
    printf("Average Waiting Time: %.2f \n", avgWaitingTime);
    printf("Average Turnaround Time: %.2f \n", avgTurnaroundTime);
    printf("Throughput: %.2f \n\n", throughput);
}

// Main function
int main(int argc, char *argv[]) {
    // Check if correct number of arguments is provided
    if (argc != 2) {
        fprintf(stderr, "ERROR: Incorrect Usage. Run as --> %s <filename> \n\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Open input file
    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        fprintf(stderr, "ERROR: File could not be opened. \n");
        exit(EXIT_FAILURE);
    }

    // Read all processes inside file
    int n = 0;
    while (!feof(file)) {
        if (fgetc(file) == '\n') {
            n++;
        }
    }
    rewind(file);

    // Allocate dynamic memory for structure
    Process *process = (Process*)malloc(n * sizeof(Process));
    if (process == NULL) {
        fprintf(stderr, "ERROR: Memory Allocation failed. \n\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Input process information and close file
    for (int i = 0; i < n; i++) {
        fscanf(file, "%d,%d,%d,%d,%d",
        &process[i].processID,
        &process[i].arrivalTime,
        &process[i].burstTime,
        &process[i].priority,
        &process[i].timeQuantum);
    }
    fclose(file);

    // Call CPU scheduling algorithms
    findFCFS(process, n);
    findSJF(process, n);
    findPriority(process, n);
    findRoundRobin(process, n, process[0].timeQuantum);

    // Free all processes
    free(process);
    return 0;
}