# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)
obj-m := char_driver.o
# Otherwise we were called directly from the command
# line; invoke the kernel build system.
else
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:  modules createsharedlib linksharedlib

modules:	
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

createsharedlib:
	gcc -shared -fpic -o cpuid_helper.so cpuid_helper.c	

linksharedlib:
	gcc -o WriteCpuManufacturerId ioctl.c -L -lib ./"cpuid_helper.so"
endif



clean:	
	$(MAKE) -C $(KERNELDIR) SUBDIRS=$(PWD) clean

