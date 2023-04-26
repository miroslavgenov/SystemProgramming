# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)
obj-m := driver_ex3.o
# Otherwise we were called directly from the command
# line; invoke the kernel build system.
else
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:  modules createsharedlib linksharedlib

modules:	
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

createsharedlib:
	gcc -shared -fpic -o cpuid_example.so cpuid_example.c	

linksharedlib:
	gcc -o main ioctl.c -L -lib ./"cpuid_example.so"
endif



clean:	
	$(MAKE) -C $(KERNELDIR) SUBDIRS=$(PWD) clean

