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

SERIALIZATION_LIB:=$(TARGET_DIR)/systemc/$(SYSTEMC_KERNEL_VERSION)/libcci_serialization.lib

include $(MODULE_MAKEFILE:module.mk=systemc-find.mk)

UNIQUE:=/Makefile
P1:=external/systemc/foundation/checkpoint/serialization$(UNIQUE)
P3:=src/external/systemc/foundation/checkpoint/serialization$(UNIQUE)
MSG:=Unable to locate: Serialization
$(eval $(call SC_FIND,SERIALIZATION_DIR,P1,,P3,MSG,UNIQUE))

unique:=/systemc-boost.mk
P1:=modules/systemc-library$(unique)
P3:=src/extensions/systemc-library$(unique)
MSG:=Unable to locate: SystemC Library
$(eval $(call SC_FIND,SYSTEMC_LIBRARY_DIR,P1,,P3,MSG,unique))
include $(call _MAKEQUOTE,$(SYSTEMC_LIBRARY_DIR)/systemc-boost.mk)

INC_DIR:=$(SERIALIZATION_DIR)
SRC_DIR:=$(SERIALIZATION_DIR)/libs/serialization/src

# NOTE: Visual Studio (msvc.mk) builds directly from this Makefile unlike GCC
#       (gcc.mk) which builds from serializations's Makefile in the foundation
#       submodule via a submake. That is why EXPECTED_BOOST_VERSION must be set
#       correctly here, and the SRCS file list must be correctly updated below.
CXXFLAGS:=/I$(INC_DIR) \
  /I$(BOOST_INC_PATH) \
  /vmg /vmv /EHsc /Zc:wchar_t /WX \
  /DEXPECTED_BOOST_VERSION=106501 \
  /D_SCL_SECURE_NO_WARNINGS

ifeq ($(D),1)
  CXXFLAGS += /Od /MDd /Zi /FS
else
  CXXFLAGS += /Ox /MD
endif

ifneq ($(SYSTEMC_CXX_STANDARD),)
  CXXFLAGS += /std:c++$(SYSTEMC_CXX_STANDARD)
endif

.PHONY: all
all: $(SERIALIZATION_LIB)

# NOTE: wide-char support has been intentionally left out
SRCS:= \
  archive_exception.cpp \
  assert_boost_version.cpp \
  basic_archive.cpp \
  basic_iarchive.cpp \
  basic_iserializer.cpp \
  basic_oarchive.cpp \
  basic_oserializer.cpp \
  basic_pointer_iserializer.cpp \
  basic_pointer_oserializer.cpp \
  basic_serializer_map.cpp \
  basic_text_iprimitive.cpp \
  basic_text_oprimitive.cpp \
  basic_xml_archive.cpp \
  binary_iarchive.cpp \
  binary_oarchive.cpp \
  codecvt_null.cpp \
  extended_type_info.cpp \
  extended_type_info_no_rtti.cpp \
  extended_type_info_typeid.cpp \
  polymorphic_iarchive.cpp \
  polymorphic_oarchive.cpp \
  singleton.cpp \
  stl_port.cpp \
  text_iarchive.cpp \
  text_oarchive.cpp \
  utf8_codecvt_facet.cpp \
  void_cast.cpp \
  xml_archive_exception.cpp \
  xml_grammar.cpp \
  xml_iarchive.cpp \
  xml_oarchive.cpp

OBJS:=$(SRCS:cpp=obj)

RM_OR_MOVE = $(PYTHON) $(W_SIMICS_BASE)\scripts\build\rm_or_move.py $(subst /,\,$@)
$(SERIALIZATION_LIB): $(OBJS)
	$(info Linking $@)
	$(RM_OR_MOVE)
	$(VCVARSPREFIX) lib /out:$@ $^

marker: $(addprefix $(SRC_DIR)/,$(SRCS))
	$(info Compiling $(notdir $^))
	$(VCVARSPREFIX) cl /MP $(CXXFLAGS) /c $^ /Fo$(CURDIR)/
	type nul > $@

$(OBJS): marker ;
