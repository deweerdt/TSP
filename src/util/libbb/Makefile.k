MODULE_NAME = bb
${MODULE_NAME}-objs := bb_core.o bb_alias.o bb_core_k.o bb_module.o bb_simple.o

ifneq ($(KERNELRELEASE),)

obj-m   := $(MODULE_NAME).o

else

KDIR	?= /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

endif


