# CPU Scheduling Simulator

## Overview
This project simulates various CPU scheduling algorithms using a C program. It's designed to demonstrate the operational principles of different scheduling methods including First-Come, First-Serve (FCFS), Shortest Remaining Time First (SJF), Priority Scheduling, and Round Robin. This simulator helps in understanding how different processes are managed and executed by an operating system's scheduler based on different criteria.

## Features
  - **Multiple Scheduling Algorithms**: Supports FCFS, SJF (with preemption), Priority (non-preemptive), and Round Robin.
  - **Dynamic Process Handling**: Reads process data from a file, including attributes like arrival time, burst time, and priority.
  - **Detailed Outputs**: Provides Gantt charts, waiting times, turnaround times, and throughput calculations.
  - **Efficiency Analysis**: Calculates average waiting time and turnaround time to evaluate the efficiency of each scheduling algorithm.

## Usage
To compile and run all programs, use the provided Makefile:
  ```
  gcc -o cpu_scheduler cpu_scheduler.c
  ./cpu_scheduler test.txt
  ```

## Prerequisites
  - GCC compiler or any standard C compiler installed on your system.
  - Basic knowledge of C programming and operating system concepts related to process scheduling.

## Input
The input should be a CSV file containing the following process attributes in order:
  - Process ID
  - Arrival Time
  - Burst Time
  - Priority
  - Time Quantum
Each line in the file represents one process. Here is an example format:
  ```
  3,0,2,2,2
  2,0,5,4,2
  1,10,8,1,2
  4,10,5,3,2
  ```

## Output
  - Gantt Chart for each scheduling algorithm.
  - Process execution details like Process ID, Waiting Time, and Turnaround Time.
  - Average Waiting Time and Turnaround Time.
  - Throughput calculated for each algorithm.

## Notes
  - Ensure the input file exists and is correctly formatted as described in the Input section.
  - The program currently does not support dynamic changes in process attributes during execution.
  - Time Quantum should be consistent across processes for the Round Robin algorithm to function correctly.