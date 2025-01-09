# Android makefile for display kernel modules

TOUCH_DLKM_ENABLE := true
ifeq ($(TARGET_KERNEL_DLKM_DISABLE), true)
       ifeq ($(TARGET_KERNEL_DLKM_TOUCH_OVERRIDE), false)
               TOUCH_DLKM_ENABLE := false
       endif
endif

ifeq ($(TOUCH_DLKM_ENABLE),  true)
       TOUCH_SELECT := CONFIG_MSM_TOUCH=m
       BOARD_OPENSOURCE_DIR ?= vendor/qcom/opensource
       BOARD_COMMON_DIR ?= device/qcom/common
       
       LOCAL_PATH := $(call my-dir)
       ifeq ($(TARGET_BOARD_PLATFORM), pineapple)
              LOCAL_MODULE_DDK_BUILD := true
       endif

       ifeq ($(TARGET_BOARD_PLATFORM), blair)
              LOCAL_MODULE_DDK_BUILD := true
       endif

       ifeq ($(TARGET_BOARD_PLATFORM), pitti)
              LOCAL_MODULE_DDK_BUILD := true
       endif

       ifeq ($(TARGET_BOARD_PLATFORM), monaco)
              LOCAL_MODULE_DDK_BUILD := true
       endif

       ifeq ($(TARGET_BOARD_PLATFORM), volcano)
              LOCAL_MODULE_DDK_BUILD := true
       endif

       include $(CLEAR_VARS)

       # This makefile is only for DLKM
       ifneq ($(findstring vendor,$(LOCAL_PATH)),)

       ifneq ($(findstring opensource,$(LOCAL_PATH)),)
               TOUCH_BLD_DIR := $(shell pwd)/$(BOARD_OPENSOURCE_DIR)/touch-drivers
       endif # opensource

       DLKM_DIR := $(TOP)/$(BOARD_COMMON_DIR)/dlkm

       LOCAL_ADDITIONAL_DEPENDENCIES := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)

       # Build
       ###########################################################
       # This is set once per LOCAL_PATH, not per (kernel) module
       KBUILD_OPTIONS := TOUCH_ROOT=$(TOUCH_BLD_DIR)

       KBUILD_OPTIONS += MODNAME=touch_dlkm
       KBUILD_OPTIONS += BOARD_PLATFORM=$(TARGET_BOARD_PLATFORM)
       KBUILD_OPTIONS += $(TOUCH_SELECT)

       ###########################################################

ifeq ($(TARGET_BOARD_PLATFORM), monaco)

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := pt_ts.ko
       LOCAL_MODULE_KBUILD_NAME  := pt_ts.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := pt_i2c.ko
       LOCAL_MODULE_KBUILD_NAME  := pt_i2c.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := pt_device_access.ko
       LOCAL_MODULE_KBUILD_NAME  := pt_device_access.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := glink_comm.ko
       LOCAL_MODULE_KBUILD_NAME  := glink_comm.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := raydium_ts.ko
       LOCAL_MODULE_KBUILD_NAME  := raydium_ts.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

else ifeq ($(TARGET_BOARD_PLATFORM), kona)

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := focaltech_fts.ko
       LOCAL_MODULE_KBUILD_NAME  := focaltech_fts.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

else ifeq ($(TARGET_BOARD_PLATFORM), pineapple)

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := nt36xxx-i2c.ko
       LOCAL_MODULE_KBUILD_NAME  := nt36xxx-i2c.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := goodix_ts.ko
       LOCAL_MODULE_KBUILD_NAME  := goodix_ts.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := atmel_mxt_ts.ko
       LOCAL_MODULE_KBUILD_NAME  := atmel_mxt_ts.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

else ifeq ($(TARGET_BOARD_PLATFORM), kalama)

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := nt36xxx-i2c.ko
       LOCAL_MODULE_KBUILD_NAME  := nt36xxx-i2c.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := goodix_ts.ko
       LOCAL_MODULE_KBUILD_NAME  := goodix_ts.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := atmel_mxt_ts.ko
       LOCAL_MODULE_KBUILD_NAME  := atmel_mxt_ts.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

else ifeq ($(TARGET_BOARD_PLATFORM), blair)

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := focaltech_fts.ko
       LOCAL_MODULE_KBUILD_NAME  := focaltech_fts.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := nt36xxx-i2c.ko
       LOCAL_MODULE_KBUILD_NAME  := nt36xxx-i2c.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := synaptics_tcm_ts.ko
       LOCAL_MODULE_KBUILD_NAME  := synaptics_tcm_ts.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := goodix_ts.ko
       LOCAL_MODULE_KBUILD_NAME  := goodix_ts.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

else ifeq ($(TARGET_BOARD_PLATFORM), crow)

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := goodix_ts.ko
       LOCAL_MODULE_KBUILD_NAME  := goodix_ts.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

else ifeq ($(TARGET_BOARD_PLATFORM), bengal)

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := synaptics_tcm_ts.ko
       LOCAL_MODULE_KBUILD_NAME  := synaptics_tcm_ts.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := nt36xxx-i2c.ko
       LOCAL_MODULE_KBUILD_NAME  := nt36xxx-i2c.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := focaltech_fts.ko
       LOCAL_MODULE_KBUILD_NAME  := focaltech_fts.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

else ifeq ($(TARGET_BOARD_PLATFORM), trinket)

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := synaptics_tcm_ts.ko
       LOCAL_MODULE_KBUILD_NAME  := synaptics_tcm_ts.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

else ifeq ($(TARGET_BOARD_PLATFORM), pitti)

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := focaltech_fts.ko
       LOCAL_MODULE_KBUILD_NAME  := focaltech_fts.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := goodix_ts.ko
       LOCAL_MODULE_KBUILD_NAME  := goodix_ts.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

else ifeq ($(TARGET_BOARD_PLATFORM), volcano)

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := goodix_ts.ko
       LOCAL_MODULE_KBUILD_NAME  := goodix_ts.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := focaltech_fts.ko
       LOCAL_MODULE_KBUILD_NAME  := focaltech_fts.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

else

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := nt36xxx-i2c.ko
       LOCAL_MODULE_KBUILD_NAME  := nt36xxx-i2c.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := goodix_ts.ko
       LOCAL_MODULE_KBUILD_NAME  := goodix_ts.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := atmel_mxt_ts.ko
       LOCAL_MODULE_KBUILD_NAME  := atmel_mxt_ts.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

       ###########################################################
       include $(CLEAR_VARS)
       LOCAL_SRC_FILES   := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
       LOCAL_MODULE              := synaptics_tcm_ts.ko
       LOCAL_MODULE_KBUILD_NAME  := synaptics_tcm_ts.ko
       LOCAL_MODULE_TAGS         := optional
       #LOCAL_MODULE_DEBUG_ENABLE := true
       LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
       include $(DLKM_DIR)/Build_external_kernelmodule.mk
       ###########################################################

endif #kona
       endif # DLKM check
endif
