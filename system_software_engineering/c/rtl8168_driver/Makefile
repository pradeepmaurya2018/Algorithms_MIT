obj-m += rtl8168.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)
FIRMWARE := firmware/rtl_nic/rtl8168h-2.fw
FIRMWAREDIR ?= /lib/firmware

# The development environment may expose ccache with a read-only cache directory.
# Disable it by default so a normal `make` remains reproducible; callers can override.
export CCACHE_DISABLE ?= 1

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

install-firmware:
	install -D -m 0644 $(FIRMWARE) $(DESTDIR)$(FIRMWAREDIR)/rtl_nic/rtl8168h-2.fw
