#!/bin/bash
gcc agent.c -o agent
gcc controller.c -o controller

sudo gnome-terminal -- ./agent
sudo gnome-terminal -- ./controller
