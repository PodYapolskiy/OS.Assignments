#!/bin/bash

# clear script that helped while doing this task

start_index=$1
end_index=$2

# unmount and delete loop_devices
for ((i = start_index; i <= end_index; i++)); do
    echo "Index $i"
    sudo umount ./lofsdisk
    sudo losetup -d /dev/loop$i
done

# delete LOFS directory
sudo rm -r ./lofsdisk

# delete genereted file
sudo rm -f ./ex1.txt

# delete executable
sudo rm -f ./ex1

# delete created file
sudo rm -f lofs.img
