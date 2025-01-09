
ifeq ($(CONFIG_ARCH_PINEAPPLE),y)
dtbo-y += nxp/pineapple-nfc.dtbo \
          nxp/pineapple-nfc-atp.dtbo \
          nxp/pineapple-nfc-cdp.dtbo \
          nxp/pineapple-nfc-mtp.dtbo \
          nxp/pineapple-nfc-qrd.dtbo \
          nxp/pineapple-nfc-rcm.dtbo

dtbo-y += st/pineapple-nfc.dtbo \
          st/pineapple-nfc-mtp.dtbo \
          st/pineapple-nfc-cdp.dtbo
endif

ifeq ($(CONFIG_ARCH_CLIFFS),y)
dtbo-y += nxp/cliffs-nfc.dtbo \
          nxp/cliffs-nfc-atp.dtbo \
          nxp/cliffs-nfc-cdp.dtbo \
          nxp/cliffs-nfc-rcm.dtbo \
          nxp/cliffs-nfc-mtp.dtbo \
          nxp/cliffs-nfc-qrd.dtbo

dtbo-y += st/cliffs-nfc.dtbo \
          st/cliffs-nfc-mtp.dtbo
endif

ifeq ($(CONFIG_ARCH_BLAIR),y)
dtbo-y += nxp/blair-nfc.dtbo \
          nxp/blair-nfc-atp.dtbo \
          nxp/blair-nfc-cdp.dtbo \
          nxp/blair-nfc-mtp.dtbo \
          nxp/blair-nfc-qrd.dtbo

endif

ifeq ($(CONFIG_ARCH_HOLI),y)
dtbo-y += nxp/holi-nfc.dtbo \
          nxp/holi-pm6125-nfc.dtbo \
          nxp/holi-pm6125-nopmi-nfc.dtbo
endif

ifeq ($(CONFIG_ARCH_PITTI),y)
dtbo-y += nxp/pitti-nfc.dtbo \
          nxp/pitti-nfc-atp.dtbo \
          nxp/pitti-nfc-idp.dtbo \
          nxp/pitti-nfc-qrd.dtbo
endif

ifeq ($(CONFIG_ARCH_VOLCANO), y)
dtbo-y += nxp/volcano-nfc.dtbo \
          nxp/volcano6i-mtp-moselle-wingmate-overlay-nfc.dtbo \
          nxp/volcano6i-mtp-wcd9395-moselle-overlay-nfc.dtbo \
          nxp/volcano6i-qrd-moselle-overlay-nfc.dtbo

dtbo-y += st/volcano6i-idp-wcd9395-brahma-overlay-nfc.dtbo \
          st/volcano6i-idp-wcd9395-ganges-overlay-nfc.dtbo \
          st/volcano6i-mtp-brahma-overlay-nfc.dtbo \
          st/volcano6i-mtp-ganges-overlay-nfc.dtbo \
          st/volcano6i-mtp-wcd9395-brahma-overlay-nfc.dtbo \
          st/volcano6i-mtp-wcd9395-ganges-hac2019-overlay-nfc.dtbo \
          st/volcano6i-mtp-wcd9395-ganges-overlay-nfc.dtbo
endif

always-y	:= $(dtb-y) $(dtbo-y)
subdir-y	:= $(dts-dirs)
clean-files	:= *.dtb *.dtbo
