
KBUILD_OPTIONS += KBUILD_EXTMOD_DTS=.

all: dtbs

dtbs:
	$(MAKE) -C $(KERNEL_SRC) M=$(M) dtbs $(KBUILD_OPTIONS)

modules_install:
	$(MAKE) M=$(M) -C $(KERNEL_SRC) modules_install

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(M) clean
