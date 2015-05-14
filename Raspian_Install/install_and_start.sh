#!/bin/sh

# ServoBlaster
cd ../ServoBlaster
make install

# RTSP Server
#cd ../RtspServer
#make install

# ServoController
cd ../Raspian_Install

cp -f ../servocontroller /usr/local/sbin
cp -f init-script.servocontroller /etc/init.d/servocontroller
	chmod 755 /etc/init.d/servocontroller
	update-rc.d servocontroller defaults 92 08
	/etc/init.d/servocontroller start

