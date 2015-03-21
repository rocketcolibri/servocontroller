Compiling servocontroller for OpenWrt/linino
============================================

WARNING:
------
** The Arduino-YUN / Liniono-ONE port of the servocontroller application is work in progress. I hope I'll have a working package soon!

Linino-ONE is a small SBC with a AR9331 processor running OpenWrt and an Atmel AtMega32u4 microcontroller for the external interfaces 
(GPIO, PWM, servo outputs ...)

The small form factor ant the onborad Wifi makes the Linino-ONE to the perfect platform for the RocketColibri remote control system.

This Makfile compiles for the target devices Linino-ONE and Arduino-YUN.

* Install needed tools
```
     % apt-get install subversion build-essential git-core libncurses5-dev zlib1g-dev gawk asciidoc bash bc binutils bzip2 fastjar flex g++ gcc util-linux zlib1g-dev libncurses5-dev libssl-dev perl-modules python2.6-dev rsync ruby sdcc unzip wget gettext xsltproc zlib1g-dev libxml-parser-perl
```

* Download version of OpenWrt used on the Yun:
```
    % git clone https://github.com/arduino/linino.git linino
```

* Build the cross-compiling toolchain and host tools:
(note this step can take up to 1 hour and uses 10GB of disk space)
```
    % cd ~/projects/linino/trunk
    % make tools/install
    % make toolchain/install
```

* Stick the servocontroller OpenWrt directory in the "package" directory where packages-to-be-built live:
```
    % cd ~/projects/linino/trunk
    % cd package
    % ln ~/projects/servocontroller/openwrt servocontroller
```

* Build the servocontroller and create ipkg file:
```
    % make V=s package/servocontroller/compile
```

* Grab resulting .ipkg file and copy to Yun/Linino system:
```
    % scp ~/projects/linino/trunk/bin/ar71xx/packages/servocontroller.ipk root@arduino.local:
```

* Log onto Yun/OpenWrt system and install the package
```
    % ssh root@arduino.local
    # opkg install servocontroller-1ar71xx.ipk
```

* Test the servocontroller with:
```
    # servocontroller 
```


-------------------

References:
-----------
- http://www.rocketcolibri.org
- http://www.linino.org/
- http://wiki.openwrt.org/doc/howtobuild/single.package
- http://fibasile.github.io/arduino-yun-custom-buildroot.html
- http://hanez.org/openwrt-building-software-packages.html


* Configuring other feeds
```
cd  linino/trunk    
./scripts/feeds update -a
./scripts/feeds install -a
make menuconfig
```