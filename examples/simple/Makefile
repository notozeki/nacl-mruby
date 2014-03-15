NACL_SDK_ROOT ?= $(abspath $(CURDIR)/../../..)

# same as <body>'s attributes in index.html
NACL_CONFIG = Release
NACL_TOOLCHAIN = newlib
NACL_MODULE = nacl-mruby

PLUGIN_DIR = ../../plugin
NEXE_DIR = $(PLUGIN_DIR)/$(NACL_TOOLCHAIN)/$(NACL_CONFIG)
NEXES = $(NEXE_DIR)/$(NACL_MODULE)_x86_32.nexe \
	$(NEXE_DIR)/$(NACL_MODULE)_x86_64.nexe \
	$(NEXE_DIR)/$(NACL_MODULE)_arm.nexe

.PHONY: all
all: ./$(NACL_TOOLCHAIN)
./$(NACL_TOOLCHAIN): $(NEXES)
	cp -r $(PLUGIN_DIR)/$(NACL_TOOLCHAIN) .
$(NEXES):
	cd $(PLUGIN_DIR); make CONFIG=$(NACL_CONFIG) TOOLCHAIN=$(NACL_TOOLCHAIN)

.PHONY: run
run:
	python $(NACL_SDK_ROOT)/tools/httpd.py --no_dir_check
