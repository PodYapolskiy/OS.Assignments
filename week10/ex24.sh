#!/bin/bash

# delete if exists
rm -d -r ./tmp
rm ./tmp1

# create a symbolic link for ./tmp
ln -s ./tmp ./tmp1
ls -li

# create ./tmp
mkdir ./tmp
ls -li

# JUSTIFICATION: the difference is that before creation ./tmp
# we can see that ./tmp1 is red highlighted (in some terminals)
# that exactly points on fact that this link points 
# to nonexistent directory


# generate file (the name was changed not to overwrite existent file)
bash gen.sh 10 ex24.txt

# add generated file to ./tmp
mv ex24.txt ./tmp
printf "\nCheck ./tmp1:\n"
ls -a ./tmp1

# create symbolic link to itself through source
source=$(realpath -s ./tmp)
ln -s $source ./tmp1/tmp2

# create another file
bash gen.sh 10 ex24.txt
mv ex24.txt ./tmp1/tmp2

# check the content of the sub-folders. 
printf "\nCheck ./tmp1/tmp2:\n"
ls -a ./tmp1/tmp2

# try to access the sub-folders ./tmp1/tmp2/tmp2/tmp2/.... 
printf "\nCheck ./tmp1/tmp2/tmp2:\n"
ls -a ./tmp1/tmp2/tmp2/

printf "\nCheck ./tmp1/tmp2/tmp2/tmp2:\n"
ls -a ./tmp1/tmp2/tmp2/tmp2/

# JUSTIFICATION: we made an infinite loop of self recursive links


# delte ./tmp and check link again
rm -r ./tmp
printf "\nCheck ./tmp1:\n"
ls ./tmp1

# delete all symbolic links
rm ./tmp1
