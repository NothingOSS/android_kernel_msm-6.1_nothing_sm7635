ifeq ($(CONFIG_ARCH_KALAMA),y)
dtbo-y += kalama-bt.dtbo
endif

ifeq ($(CONFIG_ARCH_PINEAPPLE),y)
dtbo-y += pineapple-kiwi-bt.dtbo
dtbo-y += pineapple-hdk-kiwi-bt.dtbo
dtbo-y += pineapple-aim500-kiwi-bt.dtbo
endif

ifeq ($(CONFIG_ARCH_WAIPIO),y)
dtbo-y += waipio-bt.dtbo
dtbo-y += waipio-kiwi-bt.dtbo
endif

ifeq ($(CONFIG_ARCH_BLAIR),y)
dtbo-y += blair-bt.dtbo
endif

ifeq ($(CONFIG_ARCH_TRINKET), y)
dtbo-y += trinket-bt-v1.dtbo
endif

ifeq ($(CONFIG_ARCH_HOLI),y)
dtbo-y += holi-mtp-pm6125-nopmi-overlay-bt.dtbo
dtbo-y += holi-qrd-pm6125-nopmi-overlay-bt.dtbo
dtbo-y += holi-bt.dtbo
dtbo-y += holi-pm6125-bt.dtbo
endif

ifeq ($(CONFIG_ARCH_CLIFFS), y)
dtbo-y += cliffs-kiwi-bt.dtbo
dtbo-y += cliffs-qca6750-bt.dtbo
endif

ifeq ($(CONFIG_ARCH_PITTI), y)
dtbo-y += pitti-bt.dtbo
endif

ifeq ($(CONFIG_ARCH_NIOBE), y)
dtbo-y += niobe-bt.dtbo
endif

ifeq ($(CONFIG_ARCH_VOLCANO), y)
dtbo-y += volcano-bt.dtbo
endif

ifeq ($(CONFIG_ARCH_ANORAK),y)
dtbo-y += anorak61-kiwi-bt.dtbo
dtbo-y += anorak61-hsp-bt.dtbo
endif

always-y        := $(dtb-y) $(dtbo-y)
subdir-y        := $(dts-dirs)
clean-files     := *.dtb *.dtbo
