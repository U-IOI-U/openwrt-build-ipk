include $(TOPDIR)/rules.mk

PKG_NAME:=loopshell
PKG_VERSION:=1
PKG_RELEASE:=1

PKG_MAINTAINER:=U-IOI-U
PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=LICENSE



PKG_SOURCE_SUBDIR:=$(PKG_NAME)

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/loopshell
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=LoopShell -- loop to run shell
	DEPENDS:=+libopenssl +libjson-c +libcurl
endef

define Package/loopshell/description
	It's my first package demo.
endef

define Build/Prepare
	echo "Here is Package/Prepare"
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/loopshell/install
	echo "Here is Package/install"
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/loopshell $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ddns $(1)/usr/bin/
	ln -s loopshell $(1)/usr/bin/base64
	ln -s loopshell $(1)/usr/bin/parse-ssr-v2ray
	$(CP) ./root/* $(1)/
endef

$(eval $(call BuildPackage,loopshell))
