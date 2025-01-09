ifeq ($(CONFIG_ARCH_PINEAPPLE),y)
dtbo-y += pineapple-ese-mtp.dtbo
dtbo-y += pineapple-ese-cdp.dtbo
dtbo-y += pineapple-ese-qrd.dtbo
dtbo-y += pineapple-v2-ese-mtp.dtbo
dtbo-y += pineapple-v2-ese-cdp.dtbo
dtbo-y += pineapple-v2-ese-qrd.dtbo
endif

ifeq ($(CONFIG_ARCH_CLIFFS),y)
dtbo-y += cliffs-ese-mtp.dtbo
dtbo-y += cliffs-ese-cdp.dtbo
dtbo-y += cliffs-ese-qrd.dtbo
endif

ifeq ($(CONFIG_ARCH_SUN),y)
dtbo-y += sun-ese-mtp.dtbo
dtbo-y += sun-ese-cdp.dtbo
dtbo-y += sun-ese-qrd.dtbo
dtbo-y += sun-v2-ese-mtp.dtbo
dtbo-y += sun-v2-ese-cdp.dtbo
dtbo-y += sun-v2-ese-qrd.dtbo
endif

ifeq ($(CONFIG_ARCH_VOLCANO),y)
dtbo-y += volcano-ese.dtbo
endif

always-y	:= $(dtb-y) $(dtbo-y)
subdir-y	:= $(dts-dirs)
clean-files	:= *.dtb *.dtbo
