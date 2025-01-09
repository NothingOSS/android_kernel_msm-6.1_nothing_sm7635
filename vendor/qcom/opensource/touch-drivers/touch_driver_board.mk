TOUCH_DLKM_ENABLE := true
ifeq ($(TARGET_KERNEL_DLKM_DISABLE), true)
        ifeq ($(TARGET_KERNEL_DLKM_TOUCH_OVERRIDE), false)
                TOUCH_DLKM_ENABLE := false
        endif
endif

ifeq ($(TOUCH_DLKM_ENABLE),  true)
        ifneq ($(TARGET_BOARD_AUTO),true)
                ifeq ($(call is-board-platform-in-list,$(TARGET_BOARD_PLATFORM)),true)
                       ifeq ($(TARGET_BOARD_PLATFORM), monaco)
                                BOARD_VENDOR_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/pt_ts.ko \
                                        $(KERNEL_MODULES_OUT)/pt_i2c.ko \
                                        $(KERNEL_MODULES_OUT)/pt_device_access.ko \
                                        $(KERNEL_MODULES_OUT)/glink_comm.ko \
                                        $(KERNEL_MODULES_OUT)/raydium_ts.ko
                        else ifeq ($(TARGET_BOARD_PLATFORM), kona)
                                BOARD_VENDOR_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/focaltech_fts.ko
                        else ifeq ($(TARGET_BOARD_PLATFORM), pineapple)
                                BOARD_VENDOR_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/nt36xxx-i2c.ko \
                                        $(KERNEL_MODULES_OUT)/goodix_ts.ko \
                                        $(KERNEL_MODULES_OUT)/atmel_mxt_ts.ko
                        else ifeq ($(TARGET_BOARD_PLATFORM), kalama)
                                BOARD_VENDOR_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/nt36xxx-i2c.ko \
                                        $(KERNEL_MODULES_OUT)/goodix_ts.ko \
                                        $(KERNEL_MODULES_OUT)/atmel_mxt_ts.ko
                        else ifeq ($(TARGET_BOARD_PLATFORM), blair)
                                BOARD_VENDOR_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/focaltech_fts.ko \
                                        $(KERNEL_MODULES_OUT)/nt36xxx-i2c.ko \
                                        $(KERNEL_MODULES_OUT)/synaptics_tcm_ts.ko \
                                        $(KERNEL_MODULES_OUT)/goodix_ts.ko
                        else ifeq ($(TARGET_BOARD_PLATFORM), crow)
                                BOARD_VENDOR_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/goodix_ts.ko
                        else ifeq ($(TARGET_BOARD_PLATFORM), bengal)
                                BOARD_VENDOR_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/synaptics_tcm_ts.ko \
                                        $(KERNEL_MODULES_OUT)/nt36xxx-i2c.ko \
					$(KERNEL_MODULES_OUT)/focaltech_fts.ko
                        else ifeq ($(TARGET_BOARD_PLATFORM), trinket)
                                BOARD_VENDOR_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/synaptics_tcm_ts.ko
                        else ifeq ($(TARGET_BOARD_PLATFORM), pitti)
                                BOARD_VENDOR_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/focaltech_fts.ko \
                                        $(KERNEL_MODULES_OUT)/goodix_ts.ko
                        else ifeq ($(TARGET_BOARD_PLATFORM), volcano)
                                BOARD_VENDOR_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/goodix_ts.ko \
                                        $(KERNEL_MODULES_OUT)/focaltech_fts.ko
                        else
                                BOARD_VENDOR_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/nt36xxx-i2c.ko \
                                        $(KERNEL_MODULES_OUT)/goodix_ts.ko \
                                        $(KERNEL_MODULES_OUT)/atmel_mxt_ts.ko \
                                        $(KERNEL_MODULES_OUT)/synaptics_tcm_ts.ko
                        endif
                endif
        endif
endif
