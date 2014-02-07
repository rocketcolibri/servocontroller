#!/bin/sh

sudo su

# basics 
apt-get install gcc git

# access point
# http://elinux.org/RPI-Wireless-Hotspot
apt-get install hostapd udhcpd

# video streaming
# add v4l repository
# http://www.linux-projects.org/modules/sections/index.php?op=viewarticle&artid=14
if [ grep -q uv4l_repo /etc/apt/sources.list ]
then
        echo "uv4l_repo allready in sources.list"
        exit 1
else
	echo "deb http://www.linux-projects.org/listing/uv4l_repo/raspbian/ wheezy main" >> /etc/apt/sources.list
        echo "uv4l_repo added to sources.list"

fi
apt-get update
apt-get install uv4l uv4l-raspicam libv4l-dev liblivemedia-dev uv4l-raspicam-extras






