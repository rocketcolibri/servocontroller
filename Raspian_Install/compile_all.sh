#!/bin/sh

# copy a patched version of the minunit header to include folder
sudo cp minunit.h /usr/local/include

# ServoBlaster
cd ../ServoBlaster
make clean all

# RTSP Server
cd ../RtspServer
make clean all

# ServoController
cd ../Debug_Rpi
make clean all
cd ../Raspian_Install

