#!/bin/sh

sudo su


# configure and compile json-c 
# https://github.com/json-c/json-c
cd ../apps/json-c
sh autogen.sh
./configure
make
make install

# ServoBlaster
cd ../ServoBlaster
make clean all install
cd ../../..

# RTSP Server
cd ../RtspServer
make clean all install

