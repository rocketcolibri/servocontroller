#!/bin/sh

# ServoBlaster
cd ../ServoBlaster
make uninstall

# RTSP Server
cd ../RtspServer
make uninstall

if [ -e /etc/init.d/servocontroller ]; then
  /etc/init.d/servocontroller stop 
  update-rc.d servocontroller remove
  rm -f /usr/local/sbin/ServoController
  rm -f /etc/init.d/servocontroller
fi
