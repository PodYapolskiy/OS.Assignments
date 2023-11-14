#!/bin/bash

gcc monitor.c -o monitor
gcc ex1.c -o ex1

rm -f myfile1.txt
rm -f path/myfile11.txt
rm -f path/myfile12.txt
rm -f path/myfile13.txt

# gnome-terminal -- ./monitor path &
./ex1 /home/podyapolskiy/Documents/os/week10/path
