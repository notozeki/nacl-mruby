# Copyright (c) 2013 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# GNU Makefile based on shared rules provided by the Native Client SDK.
# See README.Makefiles for more details.

VALID_TOOLCHAINS := newlib glibc pnacl

NACL_SDK_ROOT ?= $(abspath $(CURDIR)/../../..)
include $(NACL_SDK_ROOT)/tools/common.mk

NACL_MRUBY_ROOT = ..

MRUBY = $(NACL_MRUBY_ROOT)/mruby
MRUBY_CONFIG = $(NACL_MRUBY_ROOT)/plugin/build_config.rb
MRBGEMS = $(wildcard $(NACL_MRUBY_ROOT)/mrbgems/*)
MRBGEMS_SRCS = $(foreach gem,$(MRBGEMS),$(wildcard $(gem)/src/*.c))
MRBGEMS_SRCS += $(foreach gem,$(MRBGEMS),$(wildcard $(gem)/src/*.h))
MRBGEMS_MRBLIBS = $(foreach gem,$(MRBGEMS),$(wildcard $(gem)/mrblib/*.rb))
MRBGEMS_TESTS = $(foreach gem,$(MRBGEMS),$(wildcard $(gem)/test/*.rb))
MRBGEMS_TESTS += $(foreach gem,$(MRBGEMS),$(wildcard $(gem)/test/*.c))

ifeq ($(CONFIG),Debug)
DEBUG = -g
endif
CFLAGS = $(DEBUG) -Wall -I$(MRUBY)/include -I$(NACL_MRUBY_ROOT)/include
X86_32_LDFLAGS = -L$(MRUBY)/build/nacl_i686/lib
X86_64_LDFLAGS = -L$(MRUBY)/build/nacl_x86_64/lib
ARM_LDFLAGS    = -L$(MRUBY)/build/nacl_arm/lib

TARGET = nacl-mruby
DEPS = mruby
LIBS = $(DEPS) ppapi ppapi_cpp pthread pcre
SOURCES = plugin.cc remote_file_loader.cc $(wildcard $(NACL_MRUBY_ROOT)/src/*.c)


# Build mruby libraries
$(STAMPDIR)/mruby.stamp: $(MRBGEMS_SRCS) $(MRBGEMS_MRBLIBS) $(MRBGEMS_TESTS) $(MRUBY_CONFIG)
	@echo Building mruby...
	@$(MAKE) -C $(MRUBY) MRUBY_CONFIG=$(MRUBY_CONFIG)
	@$(MKDIR) -p $(STAMPDIR)
	@echo 'BUILDED' > $(STAMPDIR)/mruby.stamp

all: $(STAMPDIR)/mruby.stamp


# Build rules generated by macros from common.mk:

$(foreach src,$(SOURCES),$(eval $(call COMPILE_RULE,$(src),$(CFLAGS))))

ifeq ($(CONFIG),Release)
$(eval $(call LINK_RULE,$(TARGET)_unstripped,$(SOURCES),$(LIBS),$(DEPS)))
$(eval $(call STRIP_RULE,$(TARGET),$(TARGET)_unstripped))
else
$(eval $(call LINK_RULE,$(TARGET),$(SOURCES),$(LIBS),$(DEPS)))
endif

$(eval $(call NMF_RULE,$(TARGET),))
