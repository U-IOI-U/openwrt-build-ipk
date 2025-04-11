#
# Copyright (C) 2015-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v3.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=clash
PKG_VERSION:=v1
PKG_RELEASE:=1


PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=net
	CATEGORY:=Network
	TITLE:=Clash is A rule-based tunnel in Go
	DEPENDS:=+ca-certificates 
	URL:=https://github.com/Dreamacro/clash
endef

define Package/$(PKG_NAME)/config

endef

define Package/$(PKG_NAME)/description
Clash is A rule-based tunnel in Go
endef

define Build/Prepare

endef

define Build/Configure

endef

define Build/Compile

endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
