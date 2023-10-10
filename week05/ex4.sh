#!/bin/bash
gcc -o ex4 ex4.c -pthread

# clear the file
> ex4_res.txt

n=10000000
for m in 1 2 4 10 100
do
    echo "Running with n=$n and m=$m"
    time ./ex4 $n $m >> ex4_res.txt
done

rm ./ex4
