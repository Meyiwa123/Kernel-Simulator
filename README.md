# Kernel Simulator

This project is a small simulator of an OS kernel that can be used for performance analysis of different scheduling algorithms. The simulator receives a list of processes as input and simulates their execution based on provided trace information. The goal is to track state transitions of the processes and output the results in a CSV format.

## Input Data
The simulator reads the process information from a Comma Separated Value (CSV) file. Information on the columns follows:
| PID| Floor | Arrival Time | Total CPU Time | I/O Frequency | I/O Duration | Priority | Memory Required |
|----|-------|--------------|----------------|---------------|--------------|----------|-----------------|
|
* Pid: a unique identifier for the process
* Arrival Time: the time when the process arrives (measured in milliseconds)
* Total CPU Time: the total time the process needs to complete (excluding I/O time)
* I/O Frequency: the frequency of input/output operations for the process
* I/O Duration: the duration of each I/O operation for the process
* Priority: determines the arrangement of processes within the priority queue
* Memory Required: the minimum memory required by the process to run


## Simulation Implementation

The simulation implementation involves the following steps:
1. Read the process information from the input CSV file and store it in a suitable data structure (e.g., array of structs or linked list). The necessary information includes PID, CPU and I/O information, and remaining CPU time.
2. Simulate the execution of processes based on the provided trace information.
3. Track state transitions of processes and record the following information for each transition: Time of transition, PID, Old State, and New State.
4. Output the state transitions in a CSV format.

The simulation follows multiple scheduling strategy. The output is formatted as a CSV file with an example below.containing the time of transition, PID, old state, and new state for each transition.

| PID| Old State | New State |
|----|-----------|-----------|
|

## Simulation Arguements

The simulation program (main.exe) takes a total of 8 arguments:

1. Input File: The path to the input CSV file containing the process information.
2. Output File: The path to the output CSV file to store the state transitions.
3. Scheduler Selection: An integer value representing the scheduler selection:
    * `1`: First-Come-First-Serve (FCFS)
    * `2`: Priority
    * `3`: Round Robin (with a specified time quantum)
    * `4`: Memory Management (with specified partition sizes)
4. Time Quantum: An integer value representing the time quantum for the Round 5. Robin scheduler (ignored for other schedulers).
6. Partition 1 Size: An integer value representing the size of partition 1 for the Memory Management simulation (ignored for other schedulers).
7. Partition 2 Size: An integer value representing the size of partition 2 for the Memory Management simulation (ignored for other schedulers).
8. Partition 3 Size: An integer value representing the size of partition 3 for the Memory Management simulation (ignored for other schedulers).
9. Partition 4 Size: An integer value representing the size of partition 4 for the Memory Management simulation (ignored for other schedulers).

## Test Cases
The project includes various test cases to evaluate different aspects of the kernel simulator. These test cases are automated using the run_test.sh shell script. Let's take a closer look at what the script does.

The run_test.sh script performs the following actions:

1. Compiles the main.c source code into an executable named main.exe using the gcc compiler.
2. Creates an empty file named average_time.txt (or clears its contents if it already exists).
3. Executes the kernel simulator program (main.exe) with different test cases and parameters:
    * FCFS test case: Runs the simulator with the FCFS scheduling algorithm (1 as the additional parameter).
    * Priority test case: Runs the simulator with the priority scheduling algorithm (2 as the additional parameter).
    * RR test case: Runs the simulator with the round-robin scheduling algorithm (3 as the additional parameter). The time quantum is set to 10.
    * MM test case: Runs the simulator with the memory management simulation (4 as the additional parameter). Various memory allocation and deallocation operations are performed using the provided values.

The output of each test case is appended to the average_time.txt file.
