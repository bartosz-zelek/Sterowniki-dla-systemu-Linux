# © 2015 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

# Removing flags that interferes with autoconf.
LDFLAGS :=
LIBS :=

ifneq ($(SYSTEMC_CXX),)
CXX = $(SYSTEMC_CXX)
# TODO(ah): assume CXX means GCC for now
CC = $(subst g++,gcc,$(SYSTEMC_CXX))
endif

# Extra CXX flags
intc_cflags := $(foreach define,$(SYSTEMC_KERNEL_INTC_DEFINES),-D$(define))
EXTRA_CXXFLAGS := $(intc_cflags) -g $(SYSTEMC_VALGRIND_CFLAGS)
ifeq ($(findstring win,$(HOST_TYPE)),win)
  ifeq ($(findstring win32,$(HOST_TYPE)),win32)
    AC_TARGET := --target=mingw32
  else
    AC_TARGET := --target=x86_64-pc-mingw32 --host=x86_64-pc-mingw32
  endif
else
  # PIC not needed for MinGW, produces a warning
  EXTRA_CXXFLAGS += -fPIC
  # Run with a more strict set of flags to catch some more errors
  GCC_WARN_FLAGS = -Wall $(WARNING_AS_ERRORS) -Wwrite-strings -Wpointer-arith
  GCC_WARN_FLAGS += -Wformat -Wformat-nonliteral
  # As of GCC 5.2 we need to disable the following checks:
  GCC_WARN_FLAGS += -Wno-unused-variable
  EXTRA_CXXFLAGS += $(GCC_WARN_FLAGS)
endif
ifeq ($(D),1)
  EXTRA_CXXFLAGS += -O0
endif
ifneq ($(SYSTEMC_CXX_STANDARD),)
    EXTRA_CXXFLAGS += -std=c++$(SYSTEMC_CXX_STANDARD)
endif

# Built library in build dir.
kernel_build_lib := $(CURDIR)/src/.libs/libsystemc.a

.PHONY: build_and_copy
build_and_copy: $(kernel_build_lib) | $(install_dir)
	cp -p $(kernel_build_lib) $(install_dir)/.

# Real taget is made phony here to force a rebuild, as this is the only way for
# our Makefile to trigger the dependency tracking created by the GNU Autotools
# Makefiles
.PHONY: $(kernel_build_lib)
.NOTPARALLEL:  # Invoke make install once for all libs (though we only have one lib)
$(kernel_build_lib): $(CURDIR)/config.status
	$(MAKE) -$(MAKEFLAGS)

$(CURDIR)/config.status: $(kernel_src_dir)/configure
	$(info Configure systemc-kernel)
	mkdir -p $(CURDIR)/installation
	$(kernel_src_dir)/configure \
            --disable-maintainer-mode \
            --prefix=$(CURDIR)/installation $(AC_TARGET) \
            CC="$(CC)" \
            CXX="$(CXX)" \
            CXXFLAGS="$(EXTRA_CXXFLAGS)" \
            AR="$(AR)" \
            RANLIB="$(RANLIB)"

# Rule for local development, running systemc-regressions suite
.PHONY: install
install: $(kernel_build_lib)
	$(MAKE) install

$(install_dir):
	mkdir -p $@
