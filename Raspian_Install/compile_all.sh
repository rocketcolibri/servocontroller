#!/bin/sh

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
