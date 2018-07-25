.PHONY: all
obj-m := rootkit.o
KERNEL_DIR = /lib/modules/$(shell uname -r)/build
PWD = $(shell pwd)
all: rootkit client
rootkit:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(PWD)
client:
	gcc -o nuk3gh0st nuk3gh0st.c --std=gnu99 -Wall -Wextra -pedantic
clean:
	rm -rf *.o *.ko *.symvers *.mod.* *.order nuk3gh0st
