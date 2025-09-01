# coding: utf-8

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

# Setup lookup script
ifeq ($(findstring win,$(HOST_TYPE)),win)
    _lookup_file=$(SIMICS_PROJECT)\bin\lookup-file.bat
else
    _lookup_file=$(SIMICS_PROJECT)/bin/lookup-file
endif

# Find in project
define FIND_IN_PROJECT
    ifneq ($($(2)),)
        ifneq ($(3),)
            $(1) := $$(realpath $(3)/$($(2)))
            $(1) := $$(wildcard $$($(1)))
        endif
    endif
endef

# Find in packages (and in repo root)
define FIND_IN_PACKAGES
    ifneq ($($(2)),)
        $(1) := $$(shell $(_lookup_file) -s -f $($(2)))
        ifneq ($$($(1)),)
            _sc_shellquote = _shellquote
            ifeq ($$(call _sc_shell_shellquote test),)
                _sc_shellquote = $$(1)
            endif
            $(1) := $$(call _sc_shellquote,$$($(1)))
        else
            $(1) := $$(wildcard $(SIMICS_PROJECT)/$($(2)))
        endif
    endif
endef

# Find in user's cache
define FIND_ANYWHERE
    ifneq ($($(2)),)
        $(1) := $$(realpath $$($(2)))
        $(1) := $$(wildcard $$($(1)))
    endif
endef

# Sets variable in first argument to value of second argument if not empty,
# otherwise to value of third argument etc
define SET_NON_EMPTY_PATH
    ifneq ($$($(2)),)
        $(1) := $$($(2))
    else
        ifneq ($$($(3)),)
            $(1) := $$($(3))
        else
            $(1) := $$($(4))
        endif
    endif
endef

# Removes subdir from dir and handles slashes depending on host
REMOVE_SUBDIR = $(patsubst %$(2),%,$(subst \,/,$(1)))

# Function for finding paths in project, packages or elsewhere
#
# Paths 1, 2 and 3 are prioritized in that order.
#
# Input arguments:
#   ARG1 = Output variable name.
#   ARG2 = Variable containing pattern to search for in project.
#   ARG3 = Variable containing full or relative path to locate (i.e. in cache)
#   ARG4 = Variable containing pattern to search for in packages.
#   ARG5 = Error message if not found. If not set no error will be activated.
#   ARG6 = Pattern to remove with consideration to host slashes.
#
# Output arguments:
#   ARG1 = Variable where result is stored.
define SC_FIND
    PATH_PRIO_1 :=
    PATH_PRIO_2 :=
    PATH_PRIO_3 :=

    $(call FIND_IN_PROJECT,PATH_PRIO_1,$(2),$(SIMICS_PROJECT))
    $(call FIND_ANYWHERE,PATH_PRIO_2,$(3))
    $(call FIND_IN_PACKAGES,PATH_PRIO_3,$(4))
    $(call SET_NON_EMPTY_PATH,$(1),PATH_PRIO_1,PATH_PRIO_2,PATH_PRIO_3)

    ifeq ($$($(1)),)
        ifneq ($($(5)),)
            $$(error $($(5)))
        endif
    else
        ifneq ($$($(6)),)
            $(1) := $$(call REMOVE_SUBDIR,$$($(1)),$$($(6)))
        endif
    endif
endef

# Try to be backwards compatible. This worked on SystemC ISCTLM 6.0.6 from
# build 6041 (Base 6.0.11)
define SC_FIND_NEW
    $(call SC_FIND,$(1),$(2),$(3),$(4),$(6),$(7))
endef
