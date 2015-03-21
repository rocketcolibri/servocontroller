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

PKG_MAINTAINER := Tod Kurt <tod@thingm.com>

include $(INCLUDE_DIR)/package.mk

define Package/servocontroller
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+json-c
  TITLE:=servocontroller
  URL:=http://rocketcolibri.org
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ../* $(PKG_BUILD_DIR)/
	mkdir -p $(PKG_BUILD_DIR)/base
	$(CP) ../base/* $(PKG_BUILD_DIR)/base/
	
endef

define Build/Compile
	 $(MAKE) USBLIB_TYPE=HIDDATA OS="wrt" servocontroller -C $(PKG_BUILD_DIR) \
                $(TARGET_CONFIGURE_OPTS) \
                COPT_FLAGS="$(TARGET_CFLAGS)"  \
                LDOPT_FLAGS="$(TARGET_LDFLAGS) "
endef

define Package/servocontroller/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/servocontroller $(1)/usr/bin/

endef

$(eval $(call BuildPackage,servocontroller))
