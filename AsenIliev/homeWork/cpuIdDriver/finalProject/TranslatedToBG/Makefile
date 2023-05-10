# Ако KERNELRELEASE е дефиниран, ще бъдем извикани от ядрото и ще
# може да използваме неговият език

# KERNELRELEASE намира директорията източник на ядрото(the kernel build tree)
ifneq ($(KERNELRELEASE),)

# obj-m : = test_char.o заявява, че има модул, който трябва да бъде изграден 
# от обектния файл test_char.o.
# Полученият модул е с име test_char.ko.

#obj-m символ в makefile използван от системата за изграждане на ядрото с което  се определя
# кой модъл трябва да бъде изграден в текущата директория
obj-m := test_char.o

# В протеивен случай се обръщаме директно в командния ред 
#извикай системата за изграждане на ядрото
else
# If you are not actually running the kernel that you are building for
# , you can supply a KERNELDIR= option on the
# command line, set the KERNELDIR environment variable, or rewrite the line that sets
# KERNELDIR in the makefile.
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)


# След като дървото на източника на ядрото бъде намерено, makefile
# извиква етикетът default, който стартира втора make команда в случаят $(MAKE) 
# за да извика изграждащата система на ядрото.
# По време на второто четене makefile задава obj-m и makefiles на ядрото се
# грижи за изграждане на модълът 
default:  modules createsharedlib linksharedlib


modules:	
# този 'make' изисква да се изгради модул
# '-C' променя текущата директория с предоставената изходна директория на ядрото
# M= кара този makefile да се върне обратно при директорията на модулът преди да се
# опита да изгради целевият модул 
#Тази цел се отнася към списък с модули намерени в obj-m променливата, която е test_char.o 
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

# етикет с който ще е създаде споделена библиотека
createsharedlib:
# fpic генерира позиционно независим код който е подходящ за споделена библиотека
# shared Създава споделен обект който може да бъде свъзан с друг обект за да сформира изпълним файл
	gcc -shared -fpic -o cpuid_helper.so cpuid_helper.c	

# етикет с който ще свъзрваже споделената библиотека с ioctl.c ще създаваде изпълним файл
linksharedlib:
	gcc -o WriteCpuManufacturerId ioctl.c -L -lib ./"cpuid_helper.so"
endif


# етикет за почистване
clean:	
	$(MAKE) -C $(KERNELDIR) SUBDIRS=$(PWD) clean
