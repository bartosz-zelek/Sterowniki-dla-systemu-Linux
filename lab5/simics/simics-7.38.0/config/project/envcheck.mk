# -*- Makefile ; coding: utf-8 -*-

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

#
# User project environment checks.
#

ENV_DEPS:=config.mk compiler.mk GNUmakefile
ENV_DEPS+=$(wildcard config-user.mk) $(wildcard Config-user.mk)
ENV_DEPS+=$(call makequote,$(wildcard $(call makequote,$(RAW_SIMICS_PACKAGE_LIST)))) \
	  $(wildcard .project-properties/*)

$(HOST_TYPE)/.environment-check/all: $(ENV_DEPS)
	@echo "=== Environment Check ==="
	bin/project-setup --check-project-version	\
	        $(CHECK_PROJECT_VERSION_VERBOSE)                                \
		|| (echo '*''** Project not up-to-date, bailing out.'		\
			 'Set ENVCHECK=disable to skip this check.' && false)
	$(PYTHON) $(SIMICS_MODEL_BUILDER)/scripts/build/cctype.py $(CC) -v -o $@
