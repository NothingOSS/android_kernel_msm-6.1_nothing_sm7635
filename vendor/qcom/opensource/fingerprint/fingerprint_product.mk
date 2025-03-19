ifeq ($(call is-board-platform-in-list,volcano), true)
PRODUCT_PACKAGES += goodix_fp.ko
endif
