NACL_SDK_ROOT ?= $(abspath $(CURDIR)/../../..)

# same as <body>'s attributes in index.html
NACL_CONFIG = Debug
NACL_TOOLCHAIN = newlib
NACL_MODULE = nacl-mruby

PLUGIN_DIR = ../../plugin
NEXE_DIR = $(PLUGIN_DIR)/$(NACL_TOOLCHAIN)/$(NACL_CONFIG)
NEXES = $(NEXE_DIR)/$(NACL_MODULE)_x86_32.nexe \
	$(NEXE_DIR)/$(NACL_MODULE)_x86_64.nexe \
	$(NEXE_DIR)/$(NACL_MODULE)_arm.nexe

JQCONSOLE = jqconsole.min.js

.PHONY: all
all: ./$(NACL_TOOLCHAIN) $(JQCONSOLE)
./$(NACL_TOOLCHAIN): $(NEXES)
	cp -r $(PLUGIN_DIR)/$(NACL_TOOLCHAIN) .
$(NEXES):
	cd $(PLUGIN_DIR); make CONFIG=$(NACL_CONFIG) TOOLCHAIN=$(NACL_TOOLCHAIN)

$(JQCONSOLE):
	curl -O https://raw.githubusercontent.com/replit/jq-console/master/jqconsole.min.js

.PHONY: run
run:
	python $(NACL_SDK_ROOT)/tools/httpd.py --no_dir_check
