#!/bin/bash
gcc ex3.c -o ex3 -pthread

> ex3_res.txt # clear the file

n=10000000
for m in 1 2 4 10 100
do
    echo "Running with n=$n and m=$m"
    { time ./ex3 $n $m; } 2>> ex3_res.txt # redirect standard error output 
done

rm ex3
