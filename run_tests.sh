gcc -o main.exe main.c 

echo ""> average_time.txt

# FCFS test case
for i in {1..10} 
do
	echo "running fcfs test $i"
    ./main.exe test_case_$i.csv fcfs_test_$i.csv 1 >> average_time.txt
done

# Priority test case
for i in {1..10} 
do
    echo "running priority test $i"
    ./main.exe test_case_$i.csv priority_test_$i.csv 2 >> average_time.txt
done

# RR test case
for i in {1..10} 
do
    echo "running round robin test $i"
    ./main.exe test_case_$i.csv rr_test_$i.csv 3 10 >> average_time.txt
done

# MM test case
for i in {1..10} 
do
    echo "running memory management test $i"
    ./main.exe test_case_$i.csv mm_test_$i.csv 4 0 500 250 150 100 >> average_time.txt
    ./main.exe test_case_$i.csv mm_repeated_test_$i.csv 4 0 300 300 350 50 >> average_time.txt
done