obj-m += dev_module.o

.PHONY: all clean

all:
	$(MAKE) -C /lib/modules/`uname -r`/build M='$(PWD)' modules

clean:
	$(MAKE) -C /lib/modules/`uname -r`/build M='$(PWD)' clean
