// Header file for input output functions
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

// An enumerator (enum for short) to represent the state
enum STATE {
    STATE_NEW,
    STATE_READY,
    STATE_RUNNING,
    STATE_WAITING,
    STATE_TERMINATED
};
static const char *STATES[] = { "NEW", "READY", "RUNNING", "WAITING", "TERMINATED"};

// A structure containing all the relevant meta data for a process, this is the PCB like struct
// The io_time_remaining is used in two ways: 
// it counts how long until the next io call and how long until a current io call is complete
struct process
{
	int pid;
    int arrival_time;
    int total_cpu_time;
    int cpu_time_remaining;
    int io_frequency;
    int io_duration;
    int io_time_remaining;
    int priority;
	int memory_required;
    enum STATE s;
    int wait_time, turnaround_time;
};


// This structure is a linked list of processes and memory block
struct node {
    struct process *p;
    struct node *next;
};

// Here we use type def to create types for pointers to the preciously defined structures
typedef struct process *proc_t;
typedef struct node *node_t;

/* FUNCTION DESCRIPTION: create_proc
* This function creates a new process structure.
* The parameters are self descriptive: 
*    -pid
*    -arrival_time
*    -total_cpu_time
*    -io_frequency
*    -io_duration
*    -priority
*    -memory_requirement
*    -wait_time
*    -turnaround_time
* The return value is a pointer to new process structure
*/
proc_t create_proc(int pid, int arrival_time, int total_cpu_time, int io_frequency, int io_duration, int priority, int memory_required){
    // Initialize memory
    proc_t temp; 
    temp = (proc_t) malloc(sizeof(struct process)); 

    // Initialize contents
    // The cpu time remaining starts at total CPU time
    // the state starts as new
    temp->pid=pid;
    temp->arrival_time = arrival_time;
    temp->total_cpu_time = total_cpu_time;
    temp->cpu_time_remaining = total_cpu_time;
    temp->io_frequency = io_frequency;
    temp->io_duration = io_duration;
    temp->io_time_remaining = io_frequency;
    temp->priority = priority;
	temp->memory_required = memory_required;
    temp->s = STATE_NEW;
    temp->wait_time=0;
    temp->turnaround_time=0;
    return temp;
}

/* FUNCTION DESCRIPTION: create_node
* This function creates a new  list node.
* The parameters are: 
*    -p, a pointer to the process structure to be stored in this node
* The return value is a pointer to the new node
*/
node_t create_node(proc_t p){
    // Initialize memory
    node_t temp; 
    temp = (node_t) malloc(sizeof(struct node)); 

    // Initialize contents
    temp->next = NULL;
    temp->p = p;

    return temp;
}

/* FUNCTION DESCRIPTION: push_node
* This function adds a node to the back of the list (as though its a queue).
* The parameters are: 
*    -head points to the head in the list
*    -temp is the node to be added
* The return value is a pointer to the list
*/
node_t push_node(node_t head, node_t temp){
    // prev will be used to itterate through the list
    node_t prev;

    // If the list is empty then we return a list with only the new node
    if(head == NULL){
        head = temp;     
    } else {
        // Itterate through the list to add the new node at the end
        // The last node always points to NULL, so we get the next nodes until this happens
        prev = head;

        while(prev->next != NULL){
            prev = prev->next;
        }

        // Update the old final node to point to the new node
        prev->next = temp;
    }
    temp->next = NULL;
    return head;
}

/* FUNCTION DESCRIPTION: remove_node
* This function removes a node from within the linked list. 
* IT DOES NOT FREE THE MEMORY ALLOCATED FOR THE NODE.
* The parameters are: 
*    -head points to the pointer that is the front of the list
*    -to_be_removed points to the node that is to be removed
* The return value is an int indicating success or failure
*/
int remove_node(node_t *head, node_t to_be_removed){
    node_t temp, prev;
    if(to_be_removed == *head){
        *head = (*head)->next;
        to_be_removed->next = NULL;
        return 1;
    } else { 
        temp = *head;
        // Itterate through the list until we've checked every node
        while(temp->next != NULL){
            prev = temp;
            temp = temp->next;
            if(temp == to_be_removed){
                prev->next = temp->next;
                // NOTE:Calling function must free to_be_removed when finished with it.
                // Since the addresss of the node to be removed was passed to this function
                // the calling function must already have a reference to it
                to_be_removed->next = NULL;
                return 1;
            }
        }
    }
    return -1;
}

/* FUNCTION DESCRIPTION: read_proc_from_file
* Parse the CSV input file and load its contents into a list
* The parameters are: 
* The return value is a list of thes new prcesses
*/
node_t read_proc_from_file(char *input_file) {

	int MAXCHAR = 2000;
    char row[MAXCHAR];
    node_t new_list=NULL, node;
    proc_t proc;
    int pid, arrival_time, total_cpu_time, io_frequency, io_duration, priority, memory_required;

    FILE* f = fopen(input_file, "r");
    if(f == NULL){
        // file not opened, fail gracefully
        printf("NULL FILE\n\n\n\n");
      //  assert(false);
    } 
    // Get the first row, which has the header values
    //Pid;Arrival Time;Total CPU Time;I/O Frequency;I/O Duration
    fgets(row, MAXCHAR, f);
    // Read the remainder of the rows until you get to the end of the file
    do {
        // get the next data row
        fgets(row, MAXCHAR, f);
        // atoi turns a string into an integer
        // strtok(row,";") tokenizes the row around the ';' charaters
        // strtok(NULL, ";") gets the next token in the row
        // We are assuming that the file is setup as a CSV in the correct format
        pid = atoi(strtok(row, ","));
        arrival_time = atoi(strtok(NULL, ","));
        total_cpu_time = atoi(strtok(NULL, ","));
        io_frequency = atoi(strtok(NULL, ","));
        io_duration = atoi(strtok(NULL, ","));
        priority = atoi(strtok(NULL, ","));
        memory_required = atoi(strtok(NULL, ","));

        printf("%d, %d, %d, %d, %d, %d, %d\n", pid, arrival_time, total_cpu_time, io_frequency, io_duration, priority, memory_required);

        // We create a process struct and pass it too create node, then add this node to the new_list
        proc = create_proc(pid, arrival_time, total_cpu_time, io_frequency, io_duration, priority, memory_required);
        node = create_node(proc);
        new_list = push_node(new_list, node);
        
    } while(!feof(f));

    return new_list;
}

/* FUNCTION DESCRIPTION: clean_up
* This function frees all the dynamically allocated heap memory
* The parameters are: 
*    - list: the list of nodes to free
*/
void clean_up(node_t list){
    node_t temp;
    while(list != NULL){
        temp = list;
        list = list->next;
        free(temp->p);
        free(temp);
    }
}

/* FUNCTION DESCRIPTION: get_highest_priority
* Returns the process with the highest priority in head
*/
node_t get_highest_priority(node_t head) {
	node_t current = head, highest_priority = head;
    proc_t p; 

    while (current != NULL) {
    	p = current->p;
    	if(p->priority>highest_priority->p->priority) {
    		highest_priority = current;
    	}
        current = current->next;
    }

    return highest_priority;
}

/* FUNCTION DESCRIPTION: get_first_fit
* Returns the process with the highest priority in head
*/
node_t get_first_fit(node_t head, int size) {
	node_t current = head;
    proc_t p; 

    while (current != NULL) {
    	p = current->p;
    	if(size>=p->memory_required) {
            return current;
    	}
        current = current->next;
    }

    return NULL;
}

// A structure containing all the relevant meta data for a memory partition, this is the memory like struct
typedef struct Partition
{
	int size;
	bool occupied;
    node_t node_process;
} Partition;


/* FUNCTION DESCRIPTION: fcfs
* First Come First Serve algorith implimentation
*/
void fcfs(char *input_file, char *output_file) 
{
	int cpu_clock, avg_wt=0, avg_tt=0, proc_count=0;
	bool simulation_completed = false;
	node_t ready_list = NULL, new_list = NULL, waiting_list = NULL, terminated = NULL, temp, node;
    node_t running = NULL;

    // Process meta data should be read from a text file
    printf("FCFS Scheduler for %s\n", input_file);
    new_list = read_proc_from_file(input_file);

	//open output file and write heading to file
	FILE *file = fopen(output_file, "w");
    fprintf(file,"%s, %s, %s, %s\n","Time of transition","PID","Old State","New State");

    // Simulation loop
    for(cpu_clock=0; simulation_completed==false; cpu_clock++) 
    {
    	// Advance all the io timers for processes in waiting state
        node = waiting_list;
        while(node != NULL){
            if (node ==NULL) break;
            node->p->io_time_remaining --;
            if(node->p->io_time_remaining == 0){
                // This process is ready, it should change states from waiting to ready
                // Update the time of next io event to the frequency of its occurance
                // add it to the ready queue and remove it from waiting list
                node->p->s = STATE_READY;
                node->p->io_time_remaining = node->p->io_frequency;

                temp = node->next;
                remove_node(&waiting_list, node);
                ready_list = push_node(ready_list, node);
                fprintf(file, "%d,%d,%s,%s\n", cpu_clock, node->p->pid, STATES[STATE_WAITING], STATES[STATE_READY]);

                node = temp;
            } else {
                node = node->next;
            }
        }

    	// Check if any of the items in new queue should be moved to the ready queue
        node = new_list;
        while(node!= NULL) {
            // If the program has arrived change its state and add it to ready queue
            if(node->p->arrival_time == cpu_clock){
                node->p->s = STATE_READY;

                temp = node->next;
                remove_node(&new_list, node);
                ready_list = push_node(ready_list, node);
                fprintf(file,"%d,%d,%s,%s\n", cpu_clock, node->p->pid, STATES[STATE_NEW], STATES[STATE_READY]);
                
                node = temp;
            } else {
                node = node->next;
            }
        } 

        // Make sure the CPU is running a process
        if(running == NULL){
            // If it isn't, check if there is one ready
            if(ready_list!=NULL){
                running = ready_list;
                running->p->s = STATE_RUNNING;
                remove_node(&ready_list, running);
                fprintf(file, "%d,%d,%s,%s\n", cpu_clock, running->p->pid, STATES[STATE_READY], STATES[STATE_RUNNING]);
            } else{
                running = NULL; 
            } 
        } 
        else {
            // if it is then remove the time step from remaining time until process completetion and next io event
            running->p->cpu_time_remaining --;
            running->p->io_time_remaining --;
            // if(verbose) printf("%d: PID %d has %dms until completion and %dms until io block\n", cpu_clock,  running->p->pid, running->p->cpu_time_remaining,running->p->io_time_remaining);
            
            if(running->p->cpu_time_remaining == 0){
                // The process is finished running, terminate it
                running->p->s = STATE_TERMINATED;
                terminated = push_node(terminated,running);
                fprintf(file, "%d,%d,%s,%s\n", cpu_clock, running->p->pid, STATES[STATE_RUNNING], STATES[STATE_TERMINATED]);
                
                // Calculate waiting and turnaround time
                running->p->turnaround_time=cpu_clock-running->p->arrival_time;
                running->p->wait_time=running->p->turnaround_time-running->p->total_cpu_time;

                if(ready_list!=NULL){
                running = ready_list;
                running->p->s = STATE_RUNNING;
                remove_node(&ready_list, running);
                fprintf(file, "%d,%d,%s,%s\n", cpu_clock, running->p->pid, STATES[STATE_READY], STATES[STATE_RUNNING]);
            	} else{
                	running = NULL; 
            	} 
            } 
            else if(running->p->io_time_remaining == 0){
                // The process is blocked by io, update the timer and set state to waiting
                running->p->io_time_remaining = running->p->io_duration;
                running->p->s = STATE_WAITING;
                waiting_list = push_node(waiting_list,running);
                fprintf(file,"%d,%d,%s,%s\n", cpu_clock, running->p->pid, STATES[STATE_RUNNING], STATES[STATE_WAITING]);
                
                if(ready_list!=NULL){
                running = ready_list;
                running->p->s = STATE_RUNNING;
                remove_node(&ready_list, running);
                fprintf(file, "%d,%d,%s,%s\n", cpu_clock, running->p->pid, STATES[STATE_READY], STATES[STATE_RUNNING]);
            	} else{
                	running = NULL; 
            	} 
            }
        }

        // The simulation is completed when all the queues are empty, in otherwords, all programs have run to completion
        simulation_completed = (ready_list == NULL) && (new_list == NULL) && (waiting_list == NULL) && (running == NULL);
    }

    // Close the output file
    fclose(file);

    // Output average time
    node = terminated;
    while(node!= NULL) {
        avg_wt +=node->p->wait_time;
        avg_tt +=node->p->turnaround_time;
        proc_count++;
        node=node->next;
    } 
    printf("Average waiting time: %2.f, Average turnaround time: %2.f\n\n", (avg_wt*1.0)/proc_count, (avg_tt*1.0)/proc_count);

    // The simulation is done, all the nodes are in the terminated list, free them
    clean_up(terminated);
}

/* FUNCTION DESCRIPTION: priority
* Non-preemptive Priority algorith implimentation
*/
void priority(char *input_file, char *output_file) 
{
	int cpu_clock, avg_wt=0, avg_tt=0, proc_count=0;
	bool simulation_completed = false;
	node_t ready_list = NULL, new_list = NULL, waiting_list = NULL, terminated = NULL, temp, node;
    node_t running = NULL;

    // Process meta data should be read from a text file
    printf("Priority Scheduler for %s\n", input_file);
    new_list = read_proc_from_file(input_file);

	//open output file and write heading to file
	FILE *file = fopen(output_file, "w");
    fprintf(file,"%s, %s, %s, %s\n","Time of transition","PID","Old State","New State");

    // Simulation loop
    for(cpu_clock=0; simulation_completed==false; cpu_clock++) 
    {
    	// Check if ready list has higher priority process against running process
    	if(running!=NULL && ready_list!=NULL) {
    		if(get_highest_priority(ready_list)->p->priority>running->p->priority) {
    			running->p->s = STATE_WAITING;
                terminated = push_node(waiting_list,running);
                fprintf(file, "%d,%d,%s,%s\n", cpu_clock, running->p->pid, STATES[STATE_RUNNING], STATES[STATE_WAITING]);

    			running = get_highest_priority(ready_list);
                running->p->s = STATE_RUNNING;
                remove_node(&ready_list, running);
                fprintf(file, "%d,%d,%s,%s\n", cpu_clock, running->p->pid, STATES[STATE_READY], STATES[STATE_RUNNING]);
    		}
    	}

    	// Advance all the io timers for processes in waiting state
        node = waiting_list;
        while(node != NULL){
            if (node ==NULL) break;
            node->p->io_time_remaining --;
            if(node->p->io_time_remaining == 0){
                // This process is ready, it should change states from waiting to ready
                // Update the time of next io event to the frequency of its occurance
                // add it to the ready queue and remove it from waiting list
                node->p->s = STATE_READY;
                node->p->io_time_remaining = node->p->io_frequency;

                temp = node->next;
                remove_node(&waiting_list, node);
                ready_list = push_node(ready_list, node);
                fprintf(file, "%d,%d,%s,%s\n", cpu_clock, node->p->pid, STATES[STATE_WAITING], STATES[STATE_READY]);

                node = temp;
            } else {
                node = node->next;
            }
        }

    	// Check if any of the items in new queue should be moved to the ready queue
        node = new_list;
        while(node!= NULL) {
            // If the program has arrived change its state and add it to ready queue
            if(node->p->arrival_time == cpu_clock){
                node->p->s = STATE_READY;

                temp = node->next;
                remove_node(&new_list, node);
                ready_list = push_node(ready_list, node);
                fprintf(file,"%d,%d,%s,%s\n", cpu_clock, node->p->pid, STATES[STATE_NEW], STATES[STATE_READY]);
                
                node = temp;
            } else {
                node = node->next;
            }
        } 

        // Make sure the CPU is running a process of the highest priority
        if(running == NULL){
            // If it isn't, check if there is one ready
            if(ready_list!=NULL){
                running = get_highest_priority(ready_list);
                running->p->s = STATE_RUNNING;
                remove_node(&ready_list, running);
                fprintf(file, "%d,%d,%s,%s\n", cpu_clock, running->p->pid, STATES[STATE_READY], STATES[STATE_RUNNING]);
            } else{
                running = NULL; 
            } 
        } 
        else {
            // if it is then remove the time step from remaining time until process completetion and next io event
            running->p->cpu_time_remaining --;
            running->p->io_time_remaining --;
            // if(verbose) printf("%d: PID %d has %dms until completion and %dms until io block\n", cpu_clock,  running->p->pid, running->p->cpu_time_remaining,running->p->io_time_remaining);
            
            if(running->p->cpu_time_remaining == 0){
                // The process is finished running, terminate it
                running->p->s = STATE_TERMINATED;
                terminated = push_node(terminated,running);
                fprintf(file, "%d,%d,%s,%s\n", cpu_clock, running->p->pid, STATES[STATE_RUNNING], STATES[STATE_TERMINATED]);
                
                // Calculate waiting and turnaround time
                running->p->turnaround_time=cpu_clock-running->p->arrival_time;
                running->p->wait_time=running->p->turnaround_time-running->p->total_cpu_time;

                if(ready_list!=NULL){
                running = get_highest_priority(ready_list);
                running->p->s = STATE_RUNNING;
                remove_node(&ready_list, running);
                fprintf(file, "%d,%d,%s,%s\n", cpu_clock, running->p->pid, STATES[STATE_READY], STATES[STATE_RUNNING]);
            	} else{
                	running = NULL; 
            	} 
            } 
            else if(running->p->io_time_remaining == 0){
                // The process is blocked by io, update the timer and set state to waiting
                running->p->io_time_remaining = running->p->io_duration;
                running->p->s = STATE_WAITING;
                waiting_list = push_node(waiting_list,running);
                fprintf(file,"%d,%d,%s,%s\n", cpu_clock, running->p->pid, STATES[STATE_RUNNING], STATES[STATE_WAITING]);
                
                if(ready_list!=NULL){
                running = get_highest_priority(ready_list);
                running->p->s = STATE_RUNNING;
                remove_node(&ready_list, running);
                fprintf(file, "%d,%d,%s,%s\n", cpu_clock, running->p->pid, STATES[STATE_READY], STATES[STATE_RUNNING]);
            	} else{
                	running = NULL; 
            	} 
            }
        }

        // The simulation is completed when all the queues are empty, in otherwords, all programs have run to completion
        simulation_completed = (ready_list == NULL) && (new_list == NULL) && (waiting_list == NULL) && (running == NULL);
    }

    // Close the output file
    fclose(file);

    // Output average time
    node = terminated;
    while(node!= NULL) {
        avg_wt +=node->p->wait_time;
        avg_tt +=node->p->turnaround_time;
        proc_count++;
        node=node->next;
    } 
    printf("Average waiting time: %2.f, Average turnaround time: %2.f\n\n", (avg_wt*1.0)/proc_count, (avg_tt*1.0)/proc_count);

    // The simulation is done, all the nodes are in the terminated list, free them
    clean_up(terminated);
}

/* FUNCTION DESCRIPTION: rr
* Round Robin algorith implimentation
*/
void rr(char *input_file, char *output_file, int time_quantum) 
{
	int cpu_clock, temp_quantum=time_quantum, avg_wt=0, avg_tt=0, proc_count=0;
	bool simulation_completed = false;
	node_t ready_list = NULL, new_list = NULL, waiting_list = NULL, terminated = NULL, temp, node;
    node_t running = NULL;

    // Process meta data should be read from a text file
    printf("RR Scheduler for %s\n", input_file);
    new_list = read_proc_from_file(input_file);

	//open output file and write heading to file
	FILE *file = fopen(output_file, "w");
    fprintf(file,"%s, %s, %s, %s\n","Time of transition","PID","Old State","New State");

    // Simulation loop
    for(cpu_clock=0; simulation_completed==false; cpu_clock++) 
    {
    	if(running !=NULL && running->p->cpu_time_remaining!=0 && running->p->io_time_remaining == 0 && temp_quantum==0) {
    		// The process time quantum has expired, context switch from running to ready
            running->p->s = STATE_READY;
            terminated = push_node(ready_list,running);
            fprintf(file, "%d,%d,%s,%s\n", cpu_clock, running->p->pid, STATES[STATE_RUNNING], STATES[STATE_READY]);
            running=NULL;
    	}

    	// Advance all the io timers for processes in waiting state
        node = waiting_list;
        while(node != NULL){
            if (node ==NULL) break;
            node->p->io_time_remaining --;
            if(node->p->io_time_remaining == 0){
                // This process is ready, it should change states from waiting to ready
                // Update the time of next io event to the frequency of its occurance
                // add it to the ready queue and remove it from waiting list
                node->p->s = STATE_READY;
                node->p->io_time_remaining = node->p->io_frequency;

                temp = node->next;
                remove_node(&waiting_list, node);
                ready_list = push_node(ready_list, node);
                fprintf(file, "%d,%d,%s,%s\n", cpu_clock, node->p->pid, STATES[STATE_WAITING], STATES[STATE_READY]);

                node = temp;
            } else {
                node = node->next;
            }
        }

    	// Check if any of the items in new queue should be moved to the ready queue
        node = new_list;
        while(node!= NULL) {
            // If the program has arrived change its state and add it to ready queue
            if(node->p->arrival_time == cpu_clock){
                node->p->s = STATE_READY;

                temp = node->next;
                remove_node(&new_list, node);
                ready_list = push_node(ready_list, node);
                fprintf(file,"%d,%d,%s,%s\n", cpu_clock, node->p->pid, STATES[STATE_NEW], STATES[STATE_READY]);
                
                node = temp;
            } else {
                node = node->next;
            }
        } 

        // Make sure the CPU is running a process
        if(running == NULL){
            // If it isn't, check if there is one ready
            if(ready_list!=NULL){
                running = ready_list;
                running->p->s = STATE_RUNNING;
                remove_node(&ready_list, running);
                temp_quantum=time_quantum;
                fprintf(file, "%d,%d,%s,%s\n", cpu_clock, running->p->pid, STATES[STATE_READY], STATES[STATE_RUNNING]);
            } else{
                running = NULL; 
            } 
        } 
        else {
            // if it is then remove the time step from remaining time until process completetion and next io event
            running->p->cpu_time_remaining --;
            running->p->io_time_remaining --;
            temp_quantum--;
            // if(verbose) printf("%d: PID %d has %dms until completion and %dms until io block\n", cpu_clock,  running->p->pid, running->p->cpu_time_remaining,running->p->io_time_remaining);
            
            if(running->p->cpu_time_remaining == 0){
                // The process is finished running, terminate it
                running->p->s = STATE_TERMINATED;
                terminated = push_node(terminated,running);
                fprintf(file, "%d,%d,%s,%s\n", cpu_clock, running->p->pid, STATES[STATE_RUNNING], STATES[STATE_TERMINATED]);

                // Calculate waiting and turnaround time
                running->p->turnaround_time=cpu_clock-running->p->arrival_time;
                running->p->wait_time=running->p->turnaround_time-running->p->total_cpu_time;
                
                if(ready_list!=NULL){
                running = ready_list;
                running->p->s = STATE_RUNNING;
                remove_node(&ready_list, running);
                temp_quantum=time_quantum;
                fprintf(file, "%d,%d,%s,%s\n", cpu_clock, running->p->pid, STATES[STATE_READY], STATES[STATE_RUNNING]);
            	} else{
                	running = NULL; 
            	} 
            } 
            else if(running->p->io_time_remaining == 0){
                // The process is blocked by io, update the timer and set state to waiting
                running->p->io_time_remaining = running->p->io_duration;
                running->p->s = STATE_WAITING;
                waiting_list = push_node(waiting_list,running);
                fprintf(file,"%d,%d,%s,%s\n", cpu_clock, running->p->pid, STATES[STATE_RUNNING], STATES[STATE_WAITING]);
                
                if(ready_list!=NULL){
                running = ready_list;
                running->p->s = STATE_RUNNING;
                remove_node(&ready_list, running);
                temp_quantum=time_quantum;
                fprintf(file, "%d,%d,%s,%s\n", cpu_clock, running->p->pid, STATES[STATE_READY], STATES[STATE_RUNNING]);
            	} else{
                	running = NULL; 
            	} 
            }
        }

        // The simulation is completed when all the queues are empty, in otherwords, all programs have run to completion
        simulation_completed = (ready_list == NULL) && (new_list == NULL) && (waiting_list == NULL) && (running == NULL);
    }

    // Close the output file
    fclose(file);

    // Output average time
    node = terminated;
    while(node!= NULL) {
        avg_wt +=node->p->wait_time;
        avg_tt +=node->p->turnaround_time;
        proc_count++;
        node=node->next;
    } 
    printf("Average waiting time: %2.f, Average turnaround time: %2.f\n\n", (avg_wt*1.0)/proc_count, (avg_tt*1.0)/proc_count);

    // The simulation is done, all the nodes are in the terminated list, free them
    clean_up(terminated);
}

/* FUNCTION DESCRIPTION: mm
* Memory management First Fit implimentation
*/
void mm(char *input_file, char *output_file, Partition *memory_partition) 
{
	int cpu_clock, total_memory_used=0, total_partition_memory_used=0, avg_wt=0, avg_tt=0, proc_count=0;
	bool simulation_completed = false;
	node_t ready_list = NULL, new_list = NULL, waiting_list = NULL, terminated = NULL, temp, node;
    node_t running = NULL;

    // Process meta data should be read from a text file
    printf("Memory Partition simulation for %s\n", input_file);
    new_list = read_proc_from_file(input_file);

	//open output file and write heading to file
	FILE *file = fopen(output_file, "w");
    fprintf(file,"%s, %s, %s, %s, %s, %s, %s, %s\n","Time of transition","PID","Old State","New State", "Total Memory Used", "Total Memory Free", "Total Usable Memory", "Partition Size");

    // Simulation loop
    for(cpu_clock=0; simulation_completed==false; cpu_clock++) 
    {
    	// Advance all the io timers for processes in waiting state
        node = waiting_list;
        while(node != NULL){
            if (node ==NULL) break;
            node->p->io_time_remaining --;
            if(node->p->io_time_remaining == 0){
                // This process is ready, it should change states from waiting to ready
                // Update the time of next io event to the frequency of its occurance
                // add it to the ready queue and remove it from waiting list
                node->p->s = STATE_READY;
                node->p->io_time_remaining = node->p->io_frequency;

                temp = node->next;
                remove_node(&waiting_list, node);
                ready_list = push_node(ready_list, node);
                fprintf(file, "%d,%d,%s,%s\n", cpu_clock, node->p->pid, STATES[STATE_WAITING], STATES[STATE_READY]);

                node = temp;
            } else {
                node = node->next;
            }
        }

    	// Check if any of the items in new queue should be moved to the ready queue
        node = new_list;
        while(node!= NULL) {
            // If the program has arrived change its state and add it to ready queue
            if(node->p->arrival_time == cpu_clock){
                node->p->s = STATE_READY;

                temp = node->next;
                remove_node(&new_list, node);
                ready_list = push_node(ready_list, node);
                fprintf(file, "%d,%d,%s,%s\n", cpu_clock, node->p->pid, STATES[STATE_NEW], STATES[STATE_READY]);
                
                node = temp;
            } else {
                node = node->next;
            }
        } 
        
        // Check if process is running within the memory partition
        for(int i=0; i<4; i++) {
            if(memory_partition[i].occupied==false) {
                if(ready_list!=NULL) {
                    running=get_first_fit(ready_list, memory_partition[i].size);

                    // Check of no process can enter block
                    if(running==NULL) continue;
                    
                    running->p->s = STATE_RUNNING;
                    remove_node(&ready_list, running);

                    memory_partition[i].occupied=true;
                    memory_partition[i].node_process=running;
                    total_partition_memory_used+=memory_partition[i].size;
                    total_memory_used+=running->p->memory_required;

                    fprintf(file,"%d,%d,%s,%s,%d,%d,%d,%s%d\n", cpu_clock, running->p->pid, STATES[STATE_READY], STATES[STATE_RUNNING],
                        total_memory_used,1000-total_memory_used,1000-total_partition_memory_used , "Partition ", memory_partition[i].size);
                } else {running=NULL;}
            }
            else{
                running = memory_partition[i].node_process;

                // if it is then remove the time step from remaining time until process completetion and next io event
                running->p->cpu_time_remaining --;
                running->p->io_time_remaining --;

                if(running->p->cpu_time_remaining == 0){
                    // The process is finished running, terminate it
                    running->p->s = STATE_TERMINATED;
                    terminated = push_node(terminated,running);
                    total_partition_memory_used-=memory_partition[i].size;
                    total_memory_used-=running->p->memory_required;
                    memory_partition[i].occupied=false;
                    fprintf(file, "%d,%d,%s,%s\n", cpu_clock, running->p->pid, STATES[STATE_RUNNING], STATES[STATE_TERMINATED]);

                    // Calculate waiting and turnaround time
                    running->p->turnaround_time=cpu_clock-running->p->arrival_time;
                    running->p->wait_time=running->p->turnaround_time-running->p->total_cpu_time;

                    running=NULL;
                } 
                else if(running->p->io_time_remaining == 0){
                    // The process is blocked by io, update the timer and set state to waiting
                    running->p->io_time_remaining = running->p->io_duration;
                    running->p->s = STATE_WAITING;
                    total_partition_memory_used-=memory_partition[i].size;
                    total_memory_used-=running->p->memory_required;
                    memory_partition[i].occupied=false;
                    waiting_list = push_node(waiting_list,running);
                   fprintf(file,"%d,%d,%s,%s\n", cpu_clock, running->p->pid, STATES[STATE_RUNNING], STATES[STATE_WAITING]);
                   running=NULL;
                }
            }
        }

        // The simulation is completed when all the queues are empty, in otherwords, all programs have run to completion
        simulation_completed = (ready_list == NULL) && (new_list == NULL) && (waiting_list == NULL);
        // Check if any process is running in memory
        for(int i=0; i<4; i++) {
            if(memory_partition[i].occupied==true) {simulation_completed=false;}
        }
    }

    // Close the output file
    fclose(file);

     // Output average time
    node = terminated;
    while(node!= NULL) {
        avg_wt +=node->p->wait_time;
        avg_tt +=node->p->turnaround_time;
        proc_count++;
        node=node->next;
    } 
    printf("Average waiting time: %2.f, Average turnaround time: %2.f\n\n", (avg_wt*1.0)/proc_count, (avg_tt*1.0)/proc_count);

    // The simulation is done, all the nodes are in the terminated list, free them
    clean_up(terminated);
}

int main(int argc, char *argv[])
{
	if(argc>3) {
		if(atoi(argv[3])==1) fcfs(argv[1],argv[2]);
		if(atoi(argv[3])==2) priority(argv[1],argv[2]);
		if(atoi(argv[3])==3) rr(argv[1],argv[2], atoi(argv[3]));
		if(atoi(argv[3])==4) {
			Partition memory_partition[4];

			for(int i=0, j=5; i<4; i++, j++) {
				memory_partition[i].size=atoi(argv[j]);
				memory_partition[i].occupied=false;
			}

			if((memory_partition[0].size+memory_partition[1].size+memory_partition[2].size+memory_partition[3].size)>1000) {
				printf("Error! total partition size greter than allocated 1Mb");
				exit(1);
			}

			mm(argv[1],argv[2], memory_partition);
		}
	}

	else if(argc==3) {
		int schedule, time_quantum;
		Partition memory_partition[4];

		printf("1. First Come First Serve\n");
		printf("2. Non-preemptive Priority\n");
		printf("3. Round Robin\n");
		printf("4. Memory management\n");
		printf("Enter simulation option: ");
		scanf("%d", &schedule);
		
		switch(schedule) {
		case 1:
			printf("\nRunning FCFS scheduler for %s\n",argv[1]);
			printf("-------------------\n");
			fcfs(argv[1],argv[2]);
			break;

		case 2:
			printf("\nRunning Non-preemptive Priority scheduler for %s\n",argv[1]);
			printf("-------------------\n");
			priority(argv[1],argv[2]);
			break;

		case 3:
			printf("Enter Time quantum: ");
			scanf("%d", &time_quantum);
			printf("\nRunning Round Robin scheduler for %s\n",argv[1]);
			printf("\nTime quantum: %d\n",time_quantum);
			printf("-------------------\n");
			rr(argv[1],argv[2], time_quantum);
			break;

		case 4:
			printf("\nRunning Memory Management scheduler for %s\n",argv[1]);
			printf("-------------------\n");
			for(int i=0; i<4; i++) {
				printf("Enter the size of partition %d: ",i+1);
				scanf("%d", &memory_partition[i].size);
				memory_partition[i].occupied=false;
			}

			if((memory_partition[0].size+memory_partition[1].size+memory_partition[2].size+memory_partition[3].size)>1000) {
				printf("Error! total partition size greter than allocated 1Mb");
				exit(1);
			}

			mm(argv[1],argv[2], memory_partition);
			break;

		default:
			printf("\nError! Invalid selection");
			exit(1);
		}
	}

    else if(argc<3) {
        printf("Error! Minimum 2 arguements required\n Enter the nae of the file and output file");
        exit(1);
    }
	return 0;
}
