#!/bin/bash

# create the file with 50MB of zeros
sudo dd if=/dev/zero of=lofs.img bs=1M count=50

# find first available loop device
loop_device_number=$(sudo losetup --find | sed 's/\/dev\/loop//')

# path to available loop device
LOOP_DEVICE=/dev/loop$loop_device_number

# setup loop device 
sudo losetup $LOOP_DEVICE lofs.img

# create a Loop File System ex4 on the created file (lofs.img is already a device)
sudo mkfs.ext4 $LOOP_DEVICE

# create a new empty directory
sudo mkdir ./lofsdisk

# mount the file system in a device to a specified folder
sudo mount -t auto $LOOP_DEVICE ./lofsdisk

# enter the filesystem
cd ./lofsdisk

# adding 2 files
echo "Anatoly" | sudo tee ./file1
echo "Soldatov" | sudo tee ./file2

# function that returns paths to all shared libraries of a binary
get_libs() {
    ldd "$1" | grep -o '/.* '
}

# before coping libs to LOFS make dirs to copy to
sudo mkdir ./bin ./lib ./lib/x86_64-linux-gnu ./lib64

# copy the bash binary
bash_path=/bin/bash
sudo cp $bash_path ./bin

# get and copy shared libraries of bash command
for i in $(get_libs $bash_path); do
    sudo cp $i ./lib/x86_64-linux-gnu/;
    sudo cp $i ./lib64/;
done

# copy the cat binary
cat_path=/bin/cat
sudo cp $cat_path ./bin

# get and copy shared libraries of cat command
for i in $(get_libs $cat_path); do 
    sudo cp $i ./lib/x86_64-linux-gnu/;
    sudo cp $i ./lib64/;
done

# copy the echo binary
echo_path=/bin/echo
sudo cp $echo_path ./bin

# get and copy shared libraries of echo command
for i in $(get_libs $echo_path); do
    sudo cp $i ./lib/x86_64-linux-gnu/;
    sudo cp $i ./lib64/;
done

# copy the ls binary
ls_path=/bin/ls
sudo cp $ls_path ./bin

# get and copy shared libraries of ls command
for i in $(get_libs $ls_path); do
    sudo cp $i ./lib/x86_64-linux-gnu/;
    sudo cp $i ./lib64/;
done

# return back to week11 dir
cd ..

# compile program of iterating over root's "/" directory
gcc ex1.c -o ex1

# copy the program to LOFS
sudo cp ./ex1 ./lofsdisk

# сhange the root directory of the process to the mount point
# and execute the ex1 command
# and save results to ex1.txt
printf "LOFS \"/\":\n" >> ex1.txt
sudo chroot ./lofsdisk ./ex1 >> ex1.txt

# check root again
printf "\nNormal \"/\":\n" >> ex1.txt
./ex1 >> ex1.txt

# add findings to the end of the ex1.txt file
printf "\nThe findings:\nWe see different results because after chroot the only accessible directories were inside .lofsdisk. It's like a jail to a program when it sees that root the most upper one and can not access any upper. That's why in the 1st case we got only components of .lofsdisk as root for changed root and in the 2nd case we got the normal expected results of / directory\n" >> ex1.txt
