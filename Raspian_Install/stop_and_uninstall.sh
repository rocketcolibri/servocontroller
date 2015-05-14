#!/bin/sh

# ServoBlaster
cd ../ServoBlaster
make uninstall

# RTSP Server
cd ../RtspServer
make uninstall

cd ..
make uninstall

