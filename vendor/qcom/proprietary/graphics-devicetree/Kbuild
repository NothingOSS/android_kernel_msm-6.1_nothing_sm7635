ifeq ($(CONFIG_ARCH_WAIPIO), y)
dtbo-y += gpu/waipio-gpu.dtbo \
		gpu/waipio-v2-gpu.dtbo
endif

ifeq ($(CONFIG_ARCH_KALAMA), y)
dtbo-y += gpu/kalama-gpu.dtbo \
		gpu/kalama-v2-gpu.dtbo \
		gpu/kalama-iot-gpu.dtbo \
		gpu/kalamap-hhg-gpu.dtbo
endif

ifeq ($(CONFIG_ARCH_PINEAPPLE), y)
dtbo-y += gpu/pineapple-gpu.dtbo \
		gpu/pineapple-v2-gpu.dtbo
endif

ifeq ($(CONFIG_ARCH_SA8155), y)
dtbo-y += gpu/sa8155-v2-gpu.dtbo
endif

ifeq ($(CONFIG_ARCH_KHAJE), y)
dtbo-y += gpu/khaje-gpu.dtbo \
		gpu/khajep-gpu.dtbo \
		gpu/khajeq-gpu.dtbo \
		gpu/khajeg-gpu.dtbo
endif

ifeq ($(CONFIG_ARCH_SA8195), y)
dtbo-y += gpu/sa8195p-gpu.dtbo
endif

ifeq ($(CONFIG_ARCH_SA6155), y)
dtbo-y += gpu/sa6155p-gpu.dtbo
endif

ifeq ($(CONFIG_ARCH_MONACO), y)
dtbo-y += gpu/monaco-gpu.dtbo
endif

ifeq ($(CONFIG_ARCH_LEMANS), y)
dtbo-y += gpu/lemans-gpu.dtbo \
		gpu/lemans-gpu-ivi-adas-star.dtbo
endif

ifeq ($(CONFIG_ARCH_KONA), y)
dtbo-y += gpu/kona-gpu.dtbo \
		gpu/kona-v2-gpu.dtbo \
		gpu/kona-v2.1-gpu.dtbo
endif

ifeq ($(CONFIG_ARCH_BLAIR), y)
dtbo-y += gpu/blair-gpu.dtbo
endif

ifeq ($(CONFIG_ARCH_TRINKET), y)
dtbo-y += gpu/trinket-gpu.dtbo \
		gpu/trinketp-gpu.dtbo
endif

ifeq ($(CONFIG_ARCH_HOLI), y)
dtbo-y += gpu/holi-gpu.dtbo
endif

ifeq ($(CONFIG_ARCH_QCS405), y)
dtbo-y += gpu/qcs405-gpu.dtbo
endif

ifeq ($(CONFIG_ARCH_PITTI), y)
dtbo-y += gpu/pitti-gpu.dtbo
endif

ifeq ($(CONFIG_ARCH_CLIFFS), y)
dtbo-y += gpu/cliffs-gpu.dtbo \
		gpu/cliffs7-gpu.dtbo
endif

ifeq ($(CONFIG_ARCH_VOLCANO), y)
dtbo-y += gpu/volcano-gpu.dtbo
dtbo-y += gpu/volcano6-gpu.dtbo
dtbo-y += gpu/volcano6p-gpu.dtbo
dtbo-y += gpu/volcano6i-gpu.dtbo
dtbo-y += gpu/volcano6ip-gpu.dtbo
endif

always-y    := $(dtb-y) $(dtbo-y)
subdir-y    := $(dts-dirs)
clean-files    := *.dtb *.dtbo
