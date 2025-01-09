ifeq ($(CONFIG_ARCH_PINEAPPLE), y)
dtbo-y += pineapple-fingerprint.dtbo
endif

ifeq ($(CONFIG_ARCH_SUN), y)
dtbo-y += sun-fingerprint.dtbo
endif

ifeq ($(CONFIG_ARCH_CLIFFS), y)
dtbo-y += cliffs-fingerprint.dtbo
endif

always-y	:= $(dtb-y) $(dtbo-y)
subdir-y	:= $(dts-dirs)
clean-files	:= *.dtb *.dtbo
