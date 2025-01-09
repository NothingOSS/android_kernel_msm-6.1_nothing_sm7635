ifeq ($(CONFIG_ARCH_KALAMA),y)
dtbo-y += kalama-ipa.dtbo
endif

ifeq ($(CONFIG_ARCH_PINEAPPLE),y)
dtbo-y += pineapple-ipa.dtbo
endif

ifeq ($(CONFIG_ARCH_BLAIR),y)
dtbo-y += blair-ipa.dtbo
endif

ifeq ($(CONFIG_ARCH_HOLI),y)
dtbo-y += holi-ipa.dtbo
endif

ifeq ($(CONFIG_ARCH_CLIFFS),y)
dtbo-y += cliffs-ipa.dtbo
endif

ifeq ($(CONFIG_ARCH_PITTI),y)
dtbo-y += pitti-ipa.dtbo
endif

ifeq ($(CONFIG_ARCH_VOLCANO),y)
dtbo-y += volcano-ipa.dtbo
endif

always-y	:= $(dtb-y) $(dtbo-y)
subdir-y	:= $(dts-dirs)
clean-files	:= *.dtb *.dtbo
