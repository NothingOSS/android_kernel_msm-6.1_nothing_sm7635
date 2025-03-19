# Android makefile for Fingerprint kernel modules
FINGERPRINT_DLKM_ENABLED := true
ifeq ($(TARGET_KERNEL_DLKM_DISABLE), true)
       ifeq ($(TARGET_KERNEL_DLKM_FINGERPRINT_OVERRIDE), false)
             FINGERPRINT_DLKM_ENABLED := false;
       endif
endif

ifeq ($(FINGERPRINT_DLKM_ENABLED), true)

ifeq ($(call is-board-platform-in-list,volcano), true)
ifneq (,$(filter arm aarch64 arm64, $(TARGET_ARCH)))

LOCAL_PATH := $(call my-dir)
LOCAL_MODULE_DDK_BUILD := true

DLKM_DIR   := device/qcom/common/dlkm

include $(CLEAR_VARS)
LOCAL_SRC_FILES := $(LOCAL_PATH)/goodix/gf_spi.c \
                   $(LOCAL_PATH)/goodix/netlink.c \
                   $(LOCAL_PATH)/goodix/platform.c
LOCAL_MODULE := goodix_fp.ko
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(KERNEL_MODULES_OUT)
include $(DLKM_DIR)/Build_external_kernelmodule.mk

endif
endif
endif
