#!/bin/bash

####################################################################
# The file ex1.txt here is ex10.txt not to overwrite file from ex1 #
####################################################################

# clear files before new actions on them
rm -f ex10.txt
rm -f ex11.txt
rm -f ex12.txt
rm -f ex13.txt
rm -f /tmp/ex13.txt
rm -f output.txt

# create initial file
touch ex10.txt

# make 2 hard links
ln ex10.txt ex11.txt
ln ex10.txt ex12.txt

# generate random numbers for "ex10.txt"
bash gen.sh 10 ex10.txt

# try to find difference between files
diff ex10.txt ex11.txt
diff ex10.txt ex12.txt
diff ex11.txt ex12.txt

# JUSTIFICATION: there is no difference because 
# all these files have same i-node, consequently 
# all of them will have same content


# save i-node numbers of all 3 files
ls -i ex10.txt >> output.txt
ls -i ex11.txt >> output.txt
ls -i ex12.txt >> output.txt

# JUSTIFICATION: they are still same i-node 
# that's why the results are the same


# check the disk usage 
du ex10.txt

# create another hard link and move it to temp
ln ex10.txt ex13.txt
mv ex13.txt /tmp/

# search in current path 
printf "\nSearch in the current path\n"
find . -samefile ex10.txt

# search in root path, use "2> /dev/null" to redirect all errors to null (errors with permissions)
printf "\nSearch in the root path\n"
find / -samefile ex10.txt 2> /dev/null

# JUSTIFICATION: the difference is predictable, whiel searching from root 
# we found the i-node that we moved some steps ago that is unreachable from
# "." (current path)


# print number of hard links
printf "\nNumber of hard links: "
stat -c "%h" ex10.txt

# remove all links to "ex10.txt"
find . -samefile ex10.txt -exec rm {} \;
printf "\nSuccessfully unlinked\n"
