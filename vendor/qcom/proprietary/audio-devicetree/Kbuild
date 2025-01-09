ifeq ($(CONFIG_ARCH_KALAMA), y)
dtbo-y += kalama-audio.dtbo \
                 kalama-audio-cdp.dtbo \
                 kalama-audio-cdp-nfc.dtbo \
                 kalama-audio-wsa883x-cdp.dtbo \
                 kalama-audio-mtp.dtbo \
                 kalama-audio-mtp-nfc.dtbo \
                 kalama-audio-qrd.dtbo \
                 kalama-audio-atp.dtbo \
                 kalama-audio-rcm.dtbo \
                 kalama-audio-rumi.dtbo \
                 kalama-audio-hdk.dtbo \
                 kalama-sg-audio-hhg.dtbo
endif

ifeq ($(CONFIG_ARCH_PINEAPPLE), y)
dtbo-y += pineapple-audio.dtbo \
                 pineapple-audio-cdp.dtbo \
                 pineapple-audio-wsa883x-cdp.dtbo \
                 pineapple-audio-cdp-nfc.dtbo \
                 pineapple-audio-mtp.dtbo \
                 pineapple-audio-mtp-nfc.dtbo \
                 pineapple-audio-qrd.dtbo \
                 pineapple-audio-atp.dtbo \
                 pineapple-audio-rumi.dtbo \
                 pineapple-audio-rcm.dtbo \
                 pineapple-audio-qrd-sku2.dtbo \
                 pineapplep-audio-hdk.dtbo \
                 pineapplep-audio-aim500.dtbo \
                 pineapplep-audio-aim500-v2.dtbo \
                 cliffs-audio.dtbo \
                 cliffs-audio-mtp.dtbo \
                 cliffs7-audio-mtp.dtbo \
                 cliffs-audio-qrd.dtbo \
                 cliffs7-audio-qrd.dtbo \
                 cliffs-audio-cdp.dtbo \
                 cliffs7-audio-cdp.dtbo \
                 cliffs-audio-rcm.dtbo \
                 cliffs7-audio-rcm.dtbo \
                 cliffs-audio-mtp-wcd9395.dtbo \
                 cliffs7-audio-mtp-wcd9395.dtbo \
                 cliffs-audio-atp.dtbo \
                 cliffs7-audio-atp.dtbo
endif

ifeq ($(CONFIG_ARCH_VOLCANO), y)
dtbo-y += volcano-audio.dtbo \
		volcano-audio-cdp.dtbo \
		volcano-audio-mtp.dtbo \
		volcano-audio-qrd.dtbo \
		volcano-audio-atp.dtbo \
		volcano-audio-mtp-wcd9395.dtbo \
		volcano-audio-cdp-wcd9395.dtbo \
		volcano6i-audio-idp-wcd9395-ganges.dtbo \
		volcano6i-audio-idp-wcd9395-brahma.dtbo \
		volcano6i-audio-mtp-wcd9395-ganges.dtbo \
		volcano6i-audio-mtp-ganges.dtbo \
		volcano6i-audio-mtp-wcd9395-ganges-hac2019.dtbo \
		volcano6i-audio-mtp-wcd9395-brahma.dtbo \
		volcano6i-audio-mtp-brahma.dtbo \
		volcano6i-audio-mtp-wcd9395-moselle.dtbo \
		volcano6i-audio-mtp-moselle-wingmate.dtbo \
		volcano6i-audio-qrd-moselle.dtbo
endif

ifeq ($(CONFIG_ARCH_PITTI), y)
dtbo-y += pitti-audio.dtbo \
                 pitti-audio-idp.dtbo \
                 pitti-audio-qrd.dtbo \
                 pitti-audio-atp.dtbo
endif

ifeq ($(CONFIG_ARCH_SA8155), y)
dtbo-y +=  sa8155-audio.dtbo
endif

ifeq ($(CONFIG_ARCH_SA6155), y)
dtbo-y +=  sa6155-audio.dtbo
endif

ifeq ($(CONFIG_ARCH_MONACO_AUTO), y)
ifneq ($(CONFIG_ARCH_QTI_VM), y)
dtbo-y +=  monaco-audio.dtbo
endif
endif

ifeq ($(CONFIG_QTI_QUIN_GVM), y)
dtbo-y +=  direwolf-vm-la-audio.dtbo
dtbo-y +=  direwolf-vm-lv-audio.dtbo
dtbo-y +=  lemans-vm-lv-audio.dtbo
dtbo-y +=  lemans-vm-la-audio.dtbo
dtbo-y +=  monaco-vm-la-audio.dtbo
dtbo-y +=  monaco-vm-lv-audio.dtbo
dtbo-y +=  sa8155-vm-audio.dtbo
dtbo-y +=  sa6155-vm-audio.dtbo
dtbo-y +=  sa8195-vm-audio.dtbo
endif

ifeq ($(CONFIG_ARCH_KHAJE), y)
dtbo-y += khaje-audio.dtbo \
		khaje-audio-idp.dtbo \
		khaje-nowcd.dtbo
endif

ifeq ($(CONFIG_ARCH_BLAIR), y)
dtbo-y += blair-audio.dtbo \
		blair-audio-mtp.dtbo \
		blair-audio-mtp-usbc.dtbo \
		blair-audio-qrd.dtbo \
		blair-audio-cdp.dtbo
endif

 always-y    := $(dtb-y) $(dtbo-y)
 subdir-y    := $(dts-dirs)
 clean-files    := *.dtb *.dtbo
