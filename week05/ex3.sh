#!/bin/bash
gcc ex3.c -o ex3 -pthread

# clear the file
> ex3_res.txt

n=10000000
for m in 1 2 4 10 100
do
    echo "Running with n=$n and m=$m"
    time ./ex3 $n $m >> ex3_res.txt
done

rm ex3
