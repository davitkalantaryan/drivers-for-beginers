#
# File Makefile
# File created : 26 Sep 2019
# Created by : Davit Kalantaryan (davit.kalantaryan@desy.de)
# This Makefile can be used to create hello world LINUX test driver
#

MODULE_NAME		:= init_clean_module_test

mkfile_path		:= $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir		:= $(dir $(mkfile_path))
#SRC_DIR			:= $(mkfile_dir)../../../src
SRC_DIR			:= ../../../src

KVERSION		:= $(shell uname -r)
LINUX_CODE		:= $(shell lsb_release -c | cut -f 2)
KO_FILES		:= $(mkfile_dir)../../../../sys/driver/$(LINUX_CODE)_$(KVERSION)


OBJECT_FILES	:=		\
	$(SRC_DIR)/tests/drventry_init_clean_module_test.o


$(MODULE_NAME)-objs := $(OBJECT_FILES)

obj-m			:= $(MODULE_NAME).o

default: compile  

echos:
	@echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!mkfile_dir="$(mkfile_dir)
	@echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!mkfile_path="$(mkfile_path)


compile:
	#cp $(KO_FILES)/Module.symvers.upciedev Module.symvers
	cp $(mkfile_path) $(mkfile_dir)Makefile
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
	mkdir -p $(KO_FILES)
	cp Module.symvers $(KO_FILES)/Module.symvers.$(MODULE_NAME)
	mv $(MODULE_NAME).ko $(KO_FILES)/.

clean:
	cp $(mkfile_path) $(mkfile_dir)Makefile
	rm -f $(OBJECT_FILES)
	rm -f $(SRC_DIR)/tests/.*.cmd
	rm -f $(KO_FILES)/$(MODULE_NAME).ko
	rm -f $(KO_FILES)/Module.symvers.$(MODULE_NAME)
	test ! -d /lib/modules/$(KVERSION) || make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
	rm -rf $(mkfile_dir)Makefile
	
	
EXTRA_CFLAGS	+= -DUSE_SEMAPHORE -I$(PWD)/../../../src/driver/include -I$(PWD)/../../../include/mtsys
