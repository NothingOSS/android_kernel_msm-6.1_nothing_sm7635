
ifeq ($(CONFIG_ARCH_PINEAPPLE),y)
dtbo-y += ubwcp-devicetree/pineapple-ubwcp.dtbo
endif

always-y	:= $(dtb-y) $(dtbo-y)
subdir-y	:= $(dts-dirs)
clean-files	:= *.dtb *.dtbo
