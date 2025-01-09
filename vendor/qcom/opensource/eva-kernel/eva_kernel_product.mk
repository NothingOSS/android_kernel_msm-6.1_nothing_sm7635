ENABLE_EVA_KERNEL := true
ifeq ($(TARGET_USES_QMAA), true)
ifneq ($(TARGET_USES_QMAA_OVERRIDE_CVP), true)
ENABLE_EVA_KERNEL := false
endif
endif
ifeq ($(TARGET_BOARD_PLATFORM),volcano)
ENABLE_EVA_KERNEL := false
endif
ifeq ($(ENABLE_EVA_KERNEL), true)
PRODUCT_PACKAGES += msm-eva.ko
endif
