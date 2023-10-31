#!/bin/bash

gcc ex1.c -o ex1
./ex1 &

# get the pid from /tmp/ex1.pid
pid=$(cat /tmp/ex1.pid)
# echo "pid: $pid"

# get heap info from process's maps
heap_info=$(grep '\[heap\]' /proc/$pid/maps)

# separate the string by '-' to put start and end addresses of the heap
IFS="-" read heap_start heap_end <<< $(echo "$heap_info" | awk '{print $1}')

# print and check for correctness
# echo $(echo "$heap_info" | awk '{print $1}') # first argument
# echo $heap_start
# echo $heap_end

start="0x$heap_start"
end="0x$heap_end"

# sudo xxd -s $start -l $(($end - $start)) /proc/$pid/mem | less 
memory_address=$(sudo xxd -s $start -l $(($end - $start)) /proc/$pid/mem | less | grep "pass:.\{0,8\}" | cut -d ':' -f 1)
password=$(sudo xxd -s $start -l $(($end - $start)) /proc/$pid/mem | less | grep -o "pass:.\{0,8\}")

echo "Memory address: $memory_address"
echo "Password: $password"

kill -9 $pid # send SIGKILL to the ex1.c program
