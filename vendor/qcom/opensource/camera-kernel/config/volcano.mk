# Settings for compiling volcano camera architecture

# Localized KCONFIG settings
CONFIG_SPECTRA_ISP := y
CONFIG_SPECTRA_ICP := y
CONFIG_SPECTRA_TFE := y
CONFIG_SPECTRA_CRE := y
CONFIG_SPECTRA_SENSOR := y
CONFIG_CSF_2_5_SECURE_CAMERA := y

# Flags to pass into C preprocessor
ccflags-y += -DCONFIG_SPECTRA_ISP=1
ccflags-y += -DCONFIG_SPECTRA_ICP=1
ccflags-y += -DCONFIG_SPECTRA_TFE=1
ccflags-y += -DCONFIG_SPECTRA_CRE=1
ccflags-y += -DCONFIG_SPECTRA_SENSOR=1
ccflags-y += -DCONFIG_CSF_2_5_SECURE_CAMERA=1
