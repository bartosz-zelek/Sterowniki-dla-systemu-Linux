DEV_MODULE_VERSION = 1.0
DEV_MODULE_SITE = $(BR2_EXTERNAL_DEV_MOD_PATH)/package/dev_module
DEV_MODULE_SITE_METHOD = local

define KERNEL_MODULE_BUILD_CMDS
        $(MAKE) -C '$(@D)' LINUX_DIR='$(LINUX_DIR)' CC='$(TARGET_CC)' LD='$(TARGET_LD)' modules
endef

$(eval $(kernel-module))
$(eval $(generic-package))