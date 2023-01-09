To run the simulation with the shell script ensure test files are in the same directory as main.exe and file names are not altered.

Output from run_test.sh was moved to run_test_output; which the shell scripts created when run:
10 fcfs, 10 priority, 10 rr, 20 main memory test case (of the two diferent memory partitions) 

To run the simulation from the command line to have an interactive menu; only pass the name of the input file 
and the name of the output file you want, no additoinal arguements.

However, the simulation takes a total of 8 arguements:
"input file" "output file" "scheduler selection" "time quantum" "partition 1 size" "partition 2 size" "partition 3 size" "partition 4 size"