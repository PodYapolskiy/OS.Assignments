#!/bin/bash

# compile pager and mmu
gcc pager.c -o pager
gcc mmu.c -o mmu

# make folder if it doesn't exist
mkdir -p /tmp/ex2

# set the number of pages and frames
NUM_PAGES=4
NUM_FRAMES=2

# run the pager in the background
./pager $NUM_PAGES $NUM_FRAMES &
PAGER_PID=$! # get the most recently executed background pipilene process's id

# to give time to pager process
sleep 1

# run the MMU process
./mmu $NUM_PAGES "R0 R1 W1 R0 R2 W2 R0 R3 W2" $PAGER_PID
# gnome-terminal -- 

# wait for the pager process to finish
wait $PAGER_PID

# clean up - remove temporary files if necessary
rm ./pager
rm ./mmu

echo "Simulation completed."
