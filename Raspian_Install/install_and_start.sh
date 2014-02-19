#!/bin/sh

# ServoBlaster
cd ../ServoBlaster
make install

# RTSP Server
cd ../RtspServer
make install

# ServoController
cd ../Raspian_Install

cp -f ../Debug_Rpi/ServoController /usr/local/sbin
cp -f init-script.servocontroller /etc/init.d/servocontroller
	chmod 755 /etc/init.d/servocontroller
	update-rc.d servocontroller defaults 99 08
	/etc/init.d/servocontroller start

