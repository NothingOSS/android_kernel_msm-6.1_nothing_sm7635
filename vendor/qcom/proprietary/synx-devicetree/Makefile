
SYNX_DEVICETREE_ROOT=$(KERNEL_SRC)/$(M)
SYNX_KERNEL_ROOT=$(SYNX_DEVICETREE_ROOT)/../../opensource/synx-kernel

KBUILD_OPTIONS += SYNX_DEVICETREE_ROOT=$(KERNEL_SRC)/$(M)
KBUILD_OPTIONS += KBUILD_DTC_INCLUDE=$(SYNX_KERNEL_ROOT)
KBUILD_OPTIONS += KBUILD_EXTMOD_DTS=.
KBUILD_OPTIONS += KERNEL_ROOT=$(ROOT_DIR)/$(KERNEL_DIR)
KBUILD_OPTIONS += MODNAME=synx-devicetree

all: dtbs

dtbs:
	$(MAKE) -C $(KERNEL_SRC) M=$(M) dtbs $(KBUILD_OPTIONS)

modules_install:
	$(MAKE) M=$(M) -C $(KERNEL_SRC) modules_install

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(M) clean
