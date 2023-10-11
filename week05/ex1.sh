#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <number_of_subscribers>"
    exit 1
fi

num_subscribers=$1

gcc publisher.c -o publisher
gcc subscriber.c -o subscriber


# Start the publisher in the background
gnome-terminal -- ./publisher $num_subscribers

# Start the subscribers in separate terminal windows
for ((i = 1; i <= num_subscribers; i++)); do
    gnome-terminal -- ./subscriber $i
done
