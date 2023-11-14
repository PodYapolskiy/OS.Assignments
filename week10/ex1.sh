#!/bin/bash

# compile .c files
gcc monitor.c -o monitor
gcc ex1.c -o ex1

# clear before the next iteration
rm -f myfile1.txt
rm -f path/myfile11.txt
rm -f path/myfile12.txt
rm -f path/myfile13.txt

# execture monitor and ex1 in separate terminals
gnome-terminal -- ./monitor /home/podyapolskiy/Documents/os/week10/path
./ex1 /home/podyapolskiy/Documents/os/week10/path
