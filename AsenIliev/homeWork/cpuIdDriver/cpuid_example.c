# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)
obj-m := test_proc1.o
# Otherwise we were called directly from the command
# line; invoke the kernel build system.
else
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:  modules

modules:	
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

endif


clean:	
	$(MAKE) -C $(KERNELDIR) SUBDIRS=$(PWD) clean
	

