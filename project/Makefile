PATCHLEVEL = 1

obj-m += key-value-lkm.o

PWD := $(CURDIR)

all:
	make -C /lib/modules/5.16.0-PAPAYAPOG/build M=$(PWD) modules

clean:
	make -C /lib/modules/5.16.0-PAPAYAPOG/build M=$(PWD) clean
