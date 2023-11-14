#!/bin/bash

# make an empty file
touch ex5.txt

# remove write permission for everybody
chmod a=r-x ex5.txt
ls -l ex5.txt

# grant all permissions to owner and others
chmod u=rwx ex5.txt
chmod o=rwx ex5.txt
ls -l ex5.txt

# make permissions equal to user permissions
chmod a=u ex5.txt
ls -l ex5.txt

# JUSTIFICATION:
# 660 = |110|110|000|, give to user and group read and write permissions
# 775 = |111|111|101|, give all permissions to all and take away write permission from others
# 777 = |111|111|111|, give all permissions to all


# clear
rm -f ex5.txt
