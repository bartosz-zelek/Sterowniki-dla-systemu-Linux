# coding: utf-8

# © 2020 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

DOCS_COMMON=$(DODOC_PKG)/src/docs/dodoc
DOC_OUTPUT := $(HOST_TYPE)/doc/html
DODOC=$(DODOC_PKG)/$(HOST_TYPE)/bin/dodoc

# Find simple documents by location and having a toc.json file.
DOC_SRC_DIRS?=doc-src
DOC_DIRS+=$(patsubst %/toc.json,%,$(wildcard $(addsuffix /*/toc.json,$(DOC_SRC_DIRS))))

# Sort to remove duplicates
# Other parts of the system can provide more DOC_DIRS which may overlap with
# the set of simple documents
doc_dirs_set:=$(sort $(DOC_DIRS))

DOC_TARGETS:=$(foreach x,$(doc_dirs_set),doc/$(notdir $(x)))

DODOC_FLAGS?=--unstyled

# This can be set to variables that documents can use.
# Each variables is <name>=<value>
DODOC_VARIABLES+= simics-major=$(SIMICS_MAJOR_VERSION) simics-user-build-id=$(USER_BUILD_ID)

.PHONY: $(DOC_TARGETS)
$(DOC_TARGETS): doc/%: dodoc | $(DOC_OUTPUT)/%
	$(DODOC) $(DODOC_FLAGS) $(DOC_BASE) $(DOCS_COMMON) \
	$(DOC_EXTRA) -o $(DOC_OUTPUT)/$* --variables $(DODOC_VARIABLES)
$(foreach d,$(doc_dirs_set),$(eval doc/$(notdir $(d)): DOC_BASE:=$(d)))

doc/all: $(DOC_TARGETS)

doc/list:
	$(foreach d,$(sort $(DOC_TARGETS)), $(info $()  $d))

$(DOC_OUTPUT)/%:
	$(MKDIRS) $@

# Doc dirs that contain an extra.mk include that file.
# That file gets the variable $(DOC_EXTRA) set.
# It should place all its output in that directory.
# It should also set DOC_EXTRA_TARGETS to the top-level targets it defines.
# The main doc/doc-name target will depend on all these targets.
doc_dirs_with_makefiles:=$(patsubst %/extra.mk,%,$(wildcard $(addsuffix /extra.mk,$(doc_dirs_set))))
define MK_DOC_DIR
DOC_SRC_DIR:=$(2)
DOC_EXTRA:=$(HOST_TYPE)/obj/docs/$(1)
include $(2)/extra.mk
doc/$(1): DOC_EXTRA:=$$(DOC_EXTRA)
doc/$(1): $$(DOC_EXTRA_TARGETS)
$$(DOC_EXTRA_TARGETS): $(2)/extra.mk | $$(DOC_EXTRA)
undefine DOC_EXTRA
undefine DOC_EXTRA_TARGETS
undefine DOC_SRC_DIR
endef
$(foreach d,$(doc_dirs_with_makefiles),$(eval $(call MK_DOC_DIR,$(notdir $(d)),$(d))))
$(HOST_TYPE)/obj/docs/%:
	$(MKDIRS) $@

# In projects we have no need to build and tool. This allows us depend
# on building the tool internally, while doing nothing in user projects.
dodoc:
