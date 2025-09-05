# -*- Makefile -*-

# © 2016 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

# Re-use unittest rules, like the setup of CXXFLAGS and Boost include paths etc
include $(SYSTEMC_LIBRARY_SRC)/unittest/Makefile

IFACE_OBJS = iface_test_ethernet_common_extension.o \
             iface_test_i2c_master_v2_extension.o   \
             iface_test_i2c_slave_v2_extension.o    \
	     iface_test_i3c_master_extension.o      \
             iface_test_i3c_slave_extension.o       \
             iface_test_mii_management_extension.o  \
             iface_test_map_info_extension.o        \
             iface_test_pci_bus_extension.o         \
             iface_test_pci_device_extension.o      \
             iface_test_pci_express_extension.o     \
             iface_test_pci_upstream_operation_extension.o \
             iface_test_serial_device_extension.o

UNITTEST_OBJS = unittest_simcontext_wrapper.o

FRAMEWORK_OBJS = integration_main.o

# for standalone testing, exclude Simics paths and copy headers to isolated path
IFACE_CXXFLAGS := $(patsubst %/src/include,,$(CXXFLAGS))
IFACE_CXXFLAGS := $(patsubst %/linux64/config,,$(IFACE_CXXFLAGS))
IFACE_CXXFLAGS := $(patsubst %/src/extensions/systemc-interfaces,,$(IFACE_CXXFLAGS))
IFACE_CXXFLAGS := $(patsubst %/src/extensions/systemc-library,,$(IFACE_CXXFLAGS)) -I$(TARGET_DIR)/../obj/modules/systemc-library/inc

_COMPILE=$(CXX) $(IFACE_CXXFLAGS) -c $< -o $@

$(IFACE_OBJS): iface_%.o: $(SYSTEMC_LIBRARY_SRC)/integration/iface/%.cc
	$(info Compiling $@)
	$(_COMPILE)

$(FRAMEWORK_OBJS): integration_%.o: $(SYSTEMC_LIBRARY_SRC)/integration/%.cc
	$(info Compiling $@)
	$(_COMPILE)

INTEGRATION_OBJS = $(IFACE_OBJS) $(UNITTEST_OBJS) $(FRAMEWORK_OBJS)
$(INTEGRATION_OBJS): copy-headers

# Dependency headers currently distributed as part of SystemC Library that we must
# distribute stand-alone if the SystemC parts are to be used outside of Simics.
# The point of doing this is to identify and remove the use of Simics types and
# other constructs not ported to SystemC via the SystemC Library.
.PHONY : copy-headers
copy-headers: inc/simics/types inc/simics/systemc/iface inc/simics/systemc/utility.h
	$(info Interface, type and utility headers successfully copied to build directory)

inc/simics:
	mkdir -p $@

inc/simics/types: $(SYSTEMC_LIBRARY_SRC)/simics/types | inc/simics
	cp -rn $< $@

inc/simics/systemc: | inc/simics
	mkdir $@

inc/simics/systemc/iface: $(SYSTEMC_LIBRARY_SRC)/simics/systemc/iface | inc/simics/systemc
	cp -rn $< $@

inc/simics/systemc/utility.h: $(SYSTEMC_LIBRARY_SRC)/simics/systemc/utility.h | inc/simics/systemc
	cp -n $< $@

integration: systemc-library-integration
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./systemc-library-integration
	touch $@

.PHONY : integration-clean
integration-clean:
	rm -f $(INTEGRATION_OBJS)
	rm -f integration

systemc-library-integration: $(INTEGRATION_OBJS) $(LIB_SYSTEMC_PATH)/libsystemc.a
	$(info Linking $@)
	$(CXX) -o $@ $^ $(TEST_LDFLAGS) $(TEST_LIBS)
