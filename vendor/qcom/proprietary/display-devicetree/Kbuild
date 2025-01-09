ifeq ($(CONFIG_TARGET), msm.neo_la)
dtbo-$(CONFIG_ARCH_NEO) += display/neo-sde.dtbo \
		display/neo_la-sde-no-display-overlay.dtbo \
		display/neo_luna-v2-sde-display-idp-overlay.dtbo \
		display/neo_luna-v2-sde-display-sg-idp-overlay.dtbo
endif

ifneq ($(CONFIG_ARCH_QTI_VM), y)
dtbo-$(CONFIG_ARCH_PINEAPPLE) += display/pineapple-sde.dtbo \
		display/pineapple-sde-display-rumi-overlay.dtbo \
		display/pineapple-sde-display-cdp-overlay.dtbo \
		display/pineapple-sde-display-rcm-overlay.dtbo \
		display/pineapple-sde-display-mtp-overlay.dtbo \
		display/pineapple-sde-display-qrd-overlay.dtbo \
		display/pineapple-sde-display-hdk-overlay.dtbo \
		display/pineapple-sde-display-aim500-overlay.dtbo \
		display/pineapple-sde-display-cdp-nfc-overlay.dtbo \
		display/pineapple-sde-display-mtp-nfc-overlay.dtbo \
		display/pineapple-sde-display-atp-overlay.dtbo
else
dtbo-$(CONFIG_ARCH_PINEAPPLE) += display/trustedvm-pineapple-sde-display-mtp-overlay.dtbo \
		  display/trustedvm-pineapple-sde-display-cdp-overlay.dtbo \
		  display/trustedvm-pineapple-sde-display-rumi-overlay.dtbo \
		  display/trustedvm-pineapple-sde-display-qrd-overlay.dtbo \
		  display/trustedvm-pineapple-sde-display-atp-overlay.dtbo
endif

ifneq ($(CONFIG_ARCH_QTI_VM), y)
dtbo-$(CONFIG_ARCH_KALAMA) += display/kalama-sde.dtbo \
		display/kalama-sde-display-rumi-overlay.dtbo \
		display/kalama-sde-display-cdp-overlay.dtbo \
		display/kalama-sde-display-cdp-wsa883x-overlay.dtbo \
		display/kalama-sde-display-mtp-overlay.dtbo \
		display/kalama-sde-display-qrd-overlay.dtbo \
		display/kalama-sde-display-hdk-overlay.dtbo \
		display/kalama-sde-display-hhg-overlay.dtbo \
		display/kalama-sde-display-mtp-nfc-overlay.dtbo \
		display/kalama-sde-display-rcm-overlay.dtbo \
		display/kalama-sde-display-cdp-nfc-overlay.dtbo \
		display/kalama-sde-display-atp-overlay.dtbo
else
dtbo-$(CONFIG_ARCH_KALAMA) += display/trustedvm-kalama-sde-display-mtp-overlay.dtbo \
                  display/trustedvm-kalama-sde-display-mtp-nfc-overlay.dtbo \
                  display/trustedvm-kalama-sde-display-cdp-nfc-overlay.dtbo \
		  display/trustedvm-kalama-sde-display-cdp-overlay.dtbo \
		  display/trustedvm-kalama-sde-display-cdp-wsa883x-overlay.dtbo \
		  display/trustedvm-kalama-sde-display-rumi-overlay.dtbo \
		  display/trustedvm-kalama-sde-display-qrd-overlay.dtbo \
		  display/trustedvm-kalama-sde-display-atp-overlay.dtbo
endif

ifneq ($(CONFIG_ARCH_QTI_VM), y)
dtbo-$(CONFIG_ARCH_CLIFFS) += display/cliffs-sde.dtbo \
		display/cliffs-sde-display-rumi-overlay.dtbo \
		display/cliffs-sde-display-cdp-overlay.dtbo \
		display/cliffs-sde-display-rcm-overlay.dtbo \
		display/cliffs-sde-display-mtp-overlay.dtbo \
		display/cliffs-sde-display-mtp-wcd9395-overlay.dtbo \
		display/cliffs-sde-display-qrd-overlay.dtbo \
		display/cliffs-sde-display-atp-overlay.dtbo
else
dtbo-$(CONFIG_ARCH_CLIFFS) += display/trustedvm-cliffs-sde-display-mtp-overlay.dtbo \
		  display/trustedvm-cliffs-sde-display-cdp-overlay.dtbo \
		  display/trustedvm-cliffs-sde-display-rumi-overlay.dtbo \
		  display/trustedvm-cliffs-sde-display-qrd-overlay.dtbo \
		  display/trustedvm-cliffs-sde-display-rcm-overlay.dtbo \
		  display/trustedvm-cliffs-sde-display-atp-overlay.dtbo
endif

ifneq ($(CONFIG_ARCH_QTI_VM), y)
dtbo-$(CONFIG_ARCH_VOLCANO) += display/volcano-sde.dtbo \
		display/volcano-sde-fp1.dtbo \
		display/volcano-sde-fp2.dtbo \
		display/volcano-sde-fp3.dtbo \
		display/volcano-sde-fp4.dtbo \
		display/volcano-sde-display-atp-overlay.dtbo \
		display/volcano-sde-display-idp-overlay.dtbo \
		display/volcano-sde-display-idp-wcd9395-overlay.dtbo \
		display/volcano-sde-display-mtp-overlay.dtbo \
		display/volcano-sde-display-mtp-wcd9395-overlay.dtbo \
		display/volcano-sde-display-qrd-overlay.dtbo \
		display/volcano-sde-display-rumi-overlay.dtbo \
		display/volcano-sde-display-iot-idp-wcd9395-overlay.dtbo \
		display/volcano-sde-display-iot-mtp-overlay.dtbo \
		display/volcano-sde-display-iot-mtp-wcd9395-overlay.dtbo \
		display/volcano-sde-display-iot-mtp-wcd9395-pm7550ba-overlay.dtbo \
		display/volcano-sde-display-iot-mtp-wingmate-overlay.dtbo
else
dtbo-$(CONFIG_ARCH_VOLCANO) += display/trustedvm-volcano-sde-display-atp-overlay.dtbo \
		  display/trustedvm-volcano-sde-display-idp-overlay.dtbo \
		  display/trustedvm-volcano-sde-display-mtp-overlay.dtbo \
		  display/trustedvm-volcano-sde-display-qrd-overlay.dtbo \
		  display/trustedvm-volcano-sde-display-rumi-overlay.dtbo \
		  display/trustedvm-volcano-sde-display-iot-idp-overlay.dtbo
endif

ifneq ($(CONFIG_ARCH_QTI_VM), y)
dtbo-$(CONFIG_ARCH_PITTI) += display/pitti-sde.dtbo \
		display/pitti-sde-display-atp-overlay.dtbo \
		display/pitti-sde-display-idp-overlay.dtbo \
		display/pitti-sde-display-idp-3gb-overlay.dtbo \
		display/pitti-sde-display-idp-amoled-overlay.dtbo \
		display/pitti-sde-display-idp-90fps-overlay.dtbo \
		display/pitti-sde-display-idp-pmiv0104-overlay.dtbo \
		display/pitti-sde-display-idp-pmiv0104-90fps-overlay.dtbo \
		display/pitti-sde-display-idp-pmiv0104-3gb-overlay.dtbo \
		display/pitti-sde-display-idp-pmiv0104-amoled-overlay.dtbo \
		display/pitti-sde-display-qrd-overlay.dtbo \
		display/pitti-sde-display-qrd-3gb-overlay.dtbo \
		display/pitti-sde-display-qrd-90fps-overlay.dtbo \
		display/pitti-sde-display-rumi-overlay.dtbo
endif

ifeq ($(CONFIG_ARCH_SA8155), y)
dtbo-y += display/sa8155-adp-star-display.dtbo
endif

always-y    := $(dtb-y) $(dtbo-y)
subdir-y    := $(dts-dirs)
clean-files    := *.dtb *.dtbo
