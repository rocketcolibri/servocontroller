#
# Ipkg Makefile for building servocontroller-tool on OpenWrt / Arduino Yun
#
# Symlink this directory into your openwrt 'package' directory,
# with name 'servocontroller', then add it as a package, e.g. 
# % cd ~/projects/linino/trunk/package
# % ln -s ~/projects/servocontroller/openwrt servocontroller
# % cd ~/projects/linino/trunk
# % ./scripts/feeds install servocontroller
# % make menuconfig  (select 'servocontroller' in Utilities, then save new config)
# % make V=s package/servocontroller/compile
# % scp bin/ar71xx/packages/servocontroller_007-1_ar71xx.ipk root@arduino.local:
# # opkg install servocontroller_007_ar71xx.ipk (on yun or equiv)
#
#
include $(TOPDIR)/rules.mk

PKG_NAME:=servocontroller
PKG_VERSION:=001
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

#PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
#PKG_SOURCE_URL:=http://todbot.com/servocontroller/tst
#PKG_MD5SUM:=a76c271ebeaa3eebbea793b71a9175d8
#PKG_BUILD_PARALLEL:=1
#PKG_INSTALL:=1

PKG_MAINTAINER := Lorenz Schelling lorenz.schelling@gmx.net

include $(INCLUDE_DIR)/package.mk

define Package/servocontroller
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=servocontroller
  URL:=http://rocketcolibri.org
endef

define Build/Prepare
	echo $(BUILD_DIR)
	echo $(INCLUDE_DIR)
	echo $(TOPDIR)
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ../* $(PKG_BUILD_DIR)/
	mkdir -p $(PKG_BUILD_DIR)/json-c
	$(CP) ../json-c/* $(PKG_BUILD_DIR)/json-c/
	mkdir -p $(PKG_BUILD_DIR)/base
	$(CP) ../base/* $(PKG_BUILD_DIR)/base/
	find -name "*.o" -type f -delete
endef
  
define Build/Compile Build/Prepare
	$(MAKE) -C $(PKG_BUILD_DIR) clean
	$(MAKE) -C $(PKG_BUILD_DIR) CC="$(TARGET_CROSS)gcc" NO_BACKTRACE CFLAGS="-g" LIBS="lm" all 
	cd ..
endef

define Package/servocontroller/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/servocontroller $(1)/usr/bin/
	$(INSTALL_BIN) ./files/servocontroller.init $(1)/etc/init.d/servocontroller
endef

define Package/servocontroller/postinst
#!/bin/sh
# check if we are on real system
if [ -z "$${IPKG_INSTROOT}" ]; then
        echo "Enabling rc.d symlink for servocontroller"
        /etc/init.d/servocontroller enable
fi
exit 0
endef

define Package/servocontroller/prerm
#!/bin/sh
# check if we are on real system
if [ -z "$${IPKG_INSTROOT}" ]; then
        echo "Removing rc.d symlink for servocontroller"
        /etc/init.d/servocontroller disable
fi
exit 0
endef

$(eval $(call BuildPackage,servocontroller))
