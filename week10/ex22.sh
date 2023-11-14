#!/bin/bash

# complete first paragraph
stat -c "i-node = %i, nblocks = %b, block_size = %B, total_size = %s, permissions = %A" ./ex1

# make a copy of ./ex1
cp ./ex1 ./ex2

# output the number of hard links on ./ex2 (1)
stat -c "%h" ./ex2

# JUSTIFICATION: the number appeared to be 1.
# That's right because we COPIED a file, not made a HARD LINK.
# The i-node number of ./ex2 differs from ./ex1


# identify the files who have 3 links in the path /etc
stat -c "%h - %n" /etc/* | grep ^3

# JUSTIFICATION: that means that these files have exactly 3 hard links accross
# this operating system. There are 3 files in the system consisted of the same data
# for each of them
