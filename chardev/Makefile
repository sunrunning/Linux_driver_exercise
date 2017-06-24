CONFIG_MODULE_SIG=n
CONFIG_MODULE_SIG_ALL=n
obj-m += chardev.o

CURRENT_PATH:=$(shell pwd)

LINUX_KERNEL:=$(shell uname -r)

LINUX_KERNEL_PATH:=/usr/src/linux-headers-$(LINUX_KERNEL)

all:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules

demo:
	gcc -o demo main.c

mknod:
	sudo mknod /dev/chardev c 260 0

clean:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) clean
	rm -rf demo
