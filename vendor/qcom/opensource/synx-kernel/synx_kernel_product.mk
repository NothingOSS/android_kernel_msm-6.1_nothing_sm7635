TARGET_SYNX_ENABLE := false
ifeq ($(TARGET_KERNEL_DLKM_DISABLE),true)
	ifeq ($(TARGET_KERNEL_DLKM_SYNX_OVERRIDE),true)
		TARGET_SYNX_ENABLE := true
	endif
else
TARGET_SYNX_ENABLE := true
endif

ifneq (,$(call is-board-platform-in-list2,volcano))
TARGET_SYNX_ENABLE := false
endif
ifneq (,$(call is-board-platform-in-list2,pitti))
TARGET_SYNX_ENABLE := false
endif
ifeq ($(TARGET_SYNX_ENABLE), true)
PRODUCT_PACKAGES += synx-driver.ko
endif