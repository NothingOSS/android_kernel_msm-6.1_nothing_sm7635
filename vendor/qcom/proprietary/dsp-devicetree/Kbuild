ifeq ($(CONFIG_ARCH_PINEAPPLE), y)
ifneq ($(CONFIG_ARCH_QTI_VM), y)
dtbo-y += pineapple-dsp.dtbo
endif
endif

ifeq ($(CONFIG_ARCH_QTI_VM), y)
dtbo-y += pineapple-dsp-trustedvm.dtbo
endif

ifeq ($(CONFIG_ARCH_BLAIR), y)
dtbo-y += blair-dsp.dtbo
endif

ifeq ($(CONFIG_ARCH_HOLI), y)
dtbo-y += holi-dsp.dtbo
endif

ifeq ($(CONFIG_ARCH_CLIFFS), y)
dtbo-y += cliffs-dsp.dtbo
endif

ifeq ($(CONFIG_ARCH_VOLCANO), y)
dtbo-y += volcano-dsp.dtbo
endif

ifeq ($(CONFIG_ARCH_PITTI), y)
dtbo-y += pitti-dsp.dtbo
endif

ifeq ($(CONFIG_ARCH_NEO), y)
dtbo-y += neo-dsp.dtbo
endif

always-y	:= $(dtb-y) $(dtbo-y)
subdir-y	:= $(dts-dirs)
clean-files	:= *.dtb *.dtbo
