#!/bin/bash
gcc ex3.c -o ex3

# n = 3
./ex3 3 &
printf "./ex3 3 (0s)\n"
pstree
sleep 5

printf "\n./ex3 3 (5s)\n"
pstree
sleep 5

printf "\n./ex3 3 (10s)\n"
pstree
sleep 5

# n = 5
./ex3 5 &
printf "\n\n./ex3 5 (0s)\n"
pstree
sleep 10

printf "\n./ex3 5 (10s)\n"
pstree
sleep 10

printf "\n./ex3 5 (20s)\n"
pstree

rm ex3
