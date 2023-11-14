#!/bin/bash

# check filetypes in /dev and /etc
ls -l /dev
ls -l /dev

# count number of directories in /etc 
printf "Number of directories in /etc: "
ls -l /etc | grep ^d | wc -l

rm -f ./ex1
gcc ex21.1.c -o ex1
file --mime-encoding ex21.1.c

gcc ex21.2.c -o ex1
file --mime-encoding ex21.2.c

# JUSTIFICATINO: the difference is that ex21.1.c was encoded 
# in ascii meanwhile ex21.2.c got UTF-8 encoding.
# This happened because it was impossible to encode kirilic symbols
# into ascii.