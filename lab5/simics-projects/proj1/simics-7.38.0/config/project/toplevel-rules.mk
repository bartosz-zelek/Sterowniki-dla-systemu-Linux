# coding: utf-8

# © 2018 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

ifeq ($(TARGET_DIR),)
    REL_TARGET_DIR=$(HOST_TYPE)/lib
    TARGET_DIR=$(SIMICS_PROJECT)/$(HOST_TYPE)/lib
else
    ifneq ($(words $(TARGET_DIR)),1)
        $(error TARGET_DIR must be an absolute path without spaces, not $(TARGET_DIR))
    endif
    REL_TARGET_DIR=$(TARGET_DIR)
endif
export TARGET_DIR

MODULE_MAKEFILE := $(_M_SIMICS_BASE)/config/project/module.mk
export MODULE_MAKEFILE

_:=$(if $(wildcard $(REL_TARGET_DIR)),,$(shell $(MKDIRS) $(REL_TARGET_DIR)))

OBJ_MODULES=$(HOST_TYPE)/obj/modules

ifeq ($(MODULE_DIRS),)
ifeq (,$(PROJECT_SRC_DIR))
PROJECT_SRC_DIR=modules
endif

MODULE_DIRS:=$(PROJECT_SRC_DIR)
endif

_MODULES_QUALIFIED:=$(patsubst %/Makefile,%,$(foreach d,$(subst ;, ,$(MODULE_DIRS)),$(wildcard $(d)/*/Makefile)))
# The set of module targets that can be built.
MODULES:=$(sort $(notdir $(_MODULES_QUALIFIED)))

ifeq ($(ENABLE_CMAKE),1)
  ifneq ($(PORTED_TO_CMAKE),)
    MODULES:=$(filter-out  $(PORTED_TO_CMAKE),$(MODULES))
  endif
endif

_say=$(info $(1))

.PHONY: all help
all: $(MODULES)

help:
	$(call _say,Available modules: $(MODULES))
	$(call _say,Show build commands: make V=1)

ifeq ($(strip $(TEST_RUNNER_JOBS)),auto)
_EXTRA_TEST_OPTS += -j
else
ifneq ($(strip $(TEST_RUNNER_JOBS)),)
_EXTRA_TEST_OPTS += -j$(TEST_RUNNER_JOBS)
endif
endif

ifeq ($(USING_CMAKE),)
# module-deps.d contains all inter-module dependencies. It is
# generated based on MODULEDEPS files in modules.
_MODULE_DEPS:=$(wildcard $(addsuffix /MODULEDEPS,$(_MODULES_QUALIFIED)))
_MODULE_DEPS_D:=$(OBJ_MODULES)/module-deps.d
$(_MODULE_DEPS_D): $(_MODULE_DEPS)
	$(PYTHON) $(SIMICS_BASE)/scripts/build/make_module_deps.py $@ $(sort $(dir $(_MODULE_DEPS:/MODULEDEPS=)))
-include $(_MODULE_DEPS_D)
# without this line, an error in the _MODULE_DEPS_D recipe becomes
# non-fatal, because it's merely triggered by an -include
$(MODULES): $(_MODULE_DEPS_D)
endif

_suiteinfos = $(foreach path,$(wildcard $1/*),      \
	$(if $(filter SUITEINFO,$(notdir $(path))), \
	$(path),$(call _suiteinfos,$(path))))

_TSUITEINFOS = $(sort $(foreach d,$(subst ;, ,$(MODULE_DIRS)) targets test,$(call _suiteinfos,$(d))))
_TSUITES = $(_TSUITEINFOS:/SUITEINFO=)

# Avoid searching recursively in case there are links pointing upwards the file
# tree or to a richly populated directory. Such links together with the "test"
# make goal are currently not supported (SIMICS-9802).
ifeq ($(filter test,$(MAKECMDGOALS)),test)
.PHONY: test
test: $(_TSUITES)

.PHONY: $(_TSUITES)
$(_TSUITES): $(MODULES)
	$(PYTHON) $(SIMICS_BASE)/test/scripts/test_single_suite.py \
	$(SIMICS_BASE) $(SIMICS_PROJECT)/logs/test/$(HOST_TYPE)/$@ $(SIMICS_PROJECT)/$@ \
	--project=$(SIMICS_PROJECT) $(_EXTRA_TEST_OPTS)
endif

_TEST_RUNNER_ARGS =
ifneq ($(MODULE_DIRS),)
	_TEST_RUNNER_ARGS += --moduledirs $(call _shellquote,$(MODULE_DIRS))
endif
ifneq ($(PROJECT_SRC_DIR),)
	_TEST_RUNNER_ARGS += --basedir $(PROJECT_SRC_DIR)
endif

# test-runner substring matches its arguments against the paths to the
# test suites using native path separators. We include path separators
# before and after the module name so that is cannot spuriously
# substring match longer module names.
_DIRSEP = $(if $(_IS_WINDOWS),\,/)
test-%: %
	$(_TEST_RUNNER) $(_EXTRA_TEST_OPTS) $(_TEST_RUNNER_ARGS) $(_DIRSEP)$*$(_DIRSEP)

-include build-hook.mk
-include $(_M_DODOC_PKG)/config/project/doc.mk

export MODULE_DIRS

_submakeflags = -f $(SHELL_SIMICS_PROJECT)/$1/Makefile \
    MOD_MAKEFILE=$(SIMICS_PROJECT)/$1/Makefile \
    HOST_TYPE=$(HOST_TYPE) \
    SRC_BASE=$(SIMICS_PROJECT)/$(patsubst %/,%,$(dir $1)) \
    TARGET=$(notdir $1)
_oneword = $(if $(word 2,$1),$(warning Duplicate module name $(notdir $(word 1,$1)), found as: $1))$(word 1,$1)

ifneq ($(findstring $() ,$(MODULE_DIRS)),)
# In the future, we may extend MODULE_DIRS to support space
$(error MODULE_DIRS contains space. It is ;-separated, spaces are not yet allowed: "$(MODULE_DIRS)")
endif

.PHONY: $(MODULES)
ifeq ($(CLOBBER),1)
$(MODULES): % : $(ENVCHECK_FLAG) | clobber-%
else
$(MODULES): % : $(ENVCHECK_FLAG)
endif
	$(call _say,=== Building module $@ ===)
	$(if $(wildcard $(OBJ_MODULES)/$@),,$(MKDIRS) $(OBJ_MODULES)/$@)
	$(MAKE) -r -R \
	    -C $(OBJ_MODULES)/$@ \
            $(call _submakeflags,$(call _oneword,$(filter %/$@,$(_MODULES_QUALIFIED))))

get_source_dir:
	$(call _say,$(PROJECT_SRC_DIR))

MODULE_CLEAN   := $(addprefix clean-,$(MODULES))
MODULE_CLOBBER := $(addprefix clobber-,$(MODULES))

.PHONY: $(MODULE_CLEAN) $(MODULE_CLOBBER) clean clobber

$(MODULE_CLEAN): CLEAN_TARGET=$(patsubst clean-%,%,$@)
$(MODULE_CLEAN):
	$(call _say,Clean of module: $(CLEAN_TARGET))
	$(call _rm_r,$(HOST_TYPE)/obj/modules/$(CLEAN_TARGET))

$(MODULE_CLOBBER): CLOBBER_TARGET=$(patsubst clobber-%,%,$@)
$(MODULE_CLOBBER): MODULE_PYNAME=$(subst -,_,$(CLOBBER_TARGET))
$(MODULE_CLOBBER):
	$(call _say,Removing module: $(CLOBBER_TARGET))
	$(call _rm_r,$(HOST_TYPE)/obj/modules/$(CLOBBER_TARGET))
	$(call _rm,$(HOST_TYPE)/lib/$(CLOBBER_TARGET).$(SO_SFX))
	$(call _rm_r,$(HOST_TYPE)/lib/python/simmod/$(MODULE_PYNAME))
	$(call _rm_r,$(HOST_TYPE)/lib/python-py3/simmod/$(MODULE_PYNAME))
	$(call _rm,$(HOST_TYPE)/lib/$(MODULE_PYNAME).pyc)
	$(call _rm,$(HOST_TYPE)/lib/$(MODULE_PYNAME).pymod)

clean:
	$(call _say,Clean of all modules)
	$(call _rm_r,$(HOST_TYPE)/obj/*)
	$(call _rm_r,$(HOST_TYPE)/.environment-check)

clobber: clean
	$(call _say,Removing all modules)
	$(call _rm_r,$(HOST_TYPE)/lib)

current: $(patsubst $(HOST_TYPE)/obj/modules/%/,%,$(wildcard $(HOST_TYPE)/obj/modules/*/))

ifeq ($(HOST_TYPE),linux64)
dep-clean:
	echo "Removing all dependency files."
	find $(HOST_TYPE) -type f -name '*.d' -exec rm -f {} \;
	echo "Done."
endif
