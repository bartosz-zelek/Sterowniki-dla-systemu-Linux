# -*- Makefile -*-

# © 2010 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

# Unittests can be built and run with command:
# make systemc-library TGT=unittest

# NOTE: to build using a different SC kernel than the one provided by the
#       package, set the TEST_LDFLAGS manually on the command line with
#       complete flags. If same SC kernel is used but extra flags are needed
#       set the SYSTEMC_LIBRARY_UNITTEST_EXTRA_LDFLAGS to these extra flags.

# Setup host information depending on build type.
HOST_DIR := $(TARGET_DIR)/..

# Check host type.
ifneq ($(HOST_TYPE),linux64)
    $(error Unittest is only supported on linux64)
endif

# Setup build in Simics tree or Simics project)
UNITTEST_ROOT = $(SYSTEMC_LIBRARY_SRC)/unittest
ifeq ($(_CORE_PROJECT_BUILD),)
    _lookup_file = $(SIMICS_PROJECT)/bin/lookup-file
    QT_DB_INC = $(strip $(dir $(shell $(_lookup_file) -f src/extensions/systemc-quickthreads-db/qt_db.h)))
    $(if $(QT_DB_INC),,$(error cannot find qt_db.h))
    QT_DB_LIB = $(strip $(dir $(shell $(_lookup_file) -f $(HOST_TYPE)/lib/libsystemc-quickthreads-db.so)))
    $(if $(QT_DB_LIB),,$(error cannot find libsystemc-quickthreads-db.so))
    LIB_SYSTEMC_PATH = $(strip $(dir $(wildcard $(SIMICS_PROJECT)/$(HOST_TYPE)/lib/systemc/$(SYSTEMC_KERNEL_VERSION)/libsystemc.a)))
    VTUTILS_PATH = $(strip $(dir $(shell $(_lookup_file) -f $(HOST_TYPE)/bin/libvtutils.so)))
    SIMICS_PATH = $(strip $(dir $(shell $(_lookup_file) -f $(HOST_TYPE)/sys/lib/libstdc++.so.6)))
else
    QT_DB_INC = $(SIMICS_REPO_ROOT)/src/extensions/systemc-quickthreads-db
    QT_DB_LIB = $(SIMICS_PROJECT)/linux64/lib
    LIB_SYSTEMC_PATH = $(SIMICS_PROJECT)/linux64/lib/systemc/$(SYSTEMC_KERNEL_VERSION)
    VTUTILS_PATH = $(SIMICS_REPO_ROOT)/core/linux64/bin
    SIMICS_PATH = $(SIMICS_REPO_ROOT)/core/linux64/sys/lib
endif

ifneq ($(QT_DB_LIB),)
    QT_DB_LDFLAGS = -L$(QT_DB_LIB) -lsystemc-quickthreads-db
endif
ifeq ($(LIB_SYSTEMC_PATH),)
    LIB_SYSTEMC_PATH = $(strip $(dir $(shell $(_lookup_file) -f $(HOST_TYPE)/lib/systemc/$(SYSTEMC_KERNEL_VERSION)/libsystemc.a)))
endif
# Test t1007 uses a systemc kernel not part of the project.
# The kernel is specified by setting the makefile variable SYSTEMC_CORE_LDFLAGS.
ifeq ($(LIB_SYSTEMC_PATH),)
    LIB_SYSTEMC_PATH = $(strip $(dir $(filter %/libsystemc.a, $(SYSTEMC_CORE_LDFLAGS))))
endif
TEST_LDFLAGS ?= $(SYSTEMC_LIBRARY_UNITTEST_EXTRA_LDFLAGS) -L$(LIB_SYSTEMC_PATH) -L$(VTUTILS_PATH) $(QT_DB_LDFLAGS) -B$(BINUTILS)

export LD_LIBRARY_PATH := $(if $(LD_LIBRARY_PATH),$(LD_LIBRARY_PATH):)$(SIMICS_PATH):$(VTUTILS_PATH):$(QT_DB_LIB)

TEST_LIBS = -lrt -lm -pthread -lvtutils -ldl
ifneq ($(INTC_EXT),0)
    ifneq ($(_CORE_PROJECT_BUILD),)
        TEST_LIBS += $(LIB_SYSTEMC_PATH)/libsystemc-checkpoint.a \
                     $(LIB_SYSTEMC_PATH)/libcci_serialization.a
    else
        ifeq ($(USE_SIMICS_CHECKPOINTING),yes)
            TEST_LIBS += $(LIB_SYSTEMC_PATH)/libsystemc-checkpoint.a \
                         $(LIB_SYSTEMC_PATH)/libcci_serialization.a
        endif
    endif
endif

ifeq ($(SYSTEMC_KERNEL_VERSION),2.3.3)
	accellera = /nfs/simics/test-deps/6.4/testfiles/t63_systemc_abi/systemc-2.3.3_6.3.0_CXX14
else
	ifeq ($(SYSTEMC_KERNEL_VERSION),2.3.4)
		accellera = /nfs/simics/test-deps/6.4/testfiles/t63_systemc_abi/systemc-2.3.4_12.1.0
	else
		accellera = /nfs/simics/test-deps/6.4/testfiles/t63_systemc_abi/systemc-3.0.0_12.1.0
	endif
endif


SC_TEST_OBJS = argument_registry.o                              \
               awareness_attributes.o                           \
               awareness_class_attributes.o                     \
               awareness_cci_attribute.o                        \
               awareness_proxy_attribute_name.o                 \
               awareness_proxy_class_name.o                     \
               awareness_proxy_create_traverser.o               \
               awareness_proxy_factory_base.o                   \
               awareness_proxy_name.o                           \
               awareness_proxy.o                                \
               awareness_tlm_multi_handler_registry.o           \
               cci_configuration.o                              \
               connector.o                                      \
               context.o                                        \
               event.o                                          \
               gasket_tag.o                                     \
               injection_attr_dict_parser.o                     \
               instrumentation_tool_controller.o                \
               internal.o                                       \
               internal_processes.o                             \
               kernel_state_comparator.o                        \
               kernel_state_modifier.o                          \
               late_binder.o                                    \
               memory_profiler.o                                \
               module_loaded.o                                  \
               null_simulation.o                                \
               pcie_tlm_extension.o                             \
               process_stack.o                                  \
               report_handler.o                                 \
               sc_signal_access.o                               \
               sc_signal_access_base.o                          \
               sc_signal_access_registry.o                      \
               sim_context_proxy.o                              \
               simics2systemc_gasket_serializable.o             \
               simics2systemc_signal.o                          \
               simics2systemc_signal_serializable.o             \
               simics2tlm_dmi_data_table.o                      \
               simics2tlm_io_memory.o                           \
               simics2tlm_i2c_master_v2.o                       \
               simics2tlm_i2c_slave_v2.o                        \
               simics2tlm_i3c_master.o                          \
               simics2tlm_i3c_slave.o                           \
               simics2tlm_mii_management.o                      \
               simics2tlm_gasket_factory.o                      \
               simics2tlm_pci_express.o                         \
               simics2tlm_pci_device.o                          \
               simics2tlm_pcie_device.o                         \
               simulation.o                                     \
               simulation_deleter.o                             \
               systemc2simics_gasket.o                          \
               systemc2simics_gasket_serializable.o             \
               systemc2simics_signal.o                          \
               systemc2simics_signal_serializable.o             \
               thread_pool.o                                    \
               tlm2simics_dmi_transaction_handler.o             \
               tlm2simics_error_transaction_handler.o           \
               tlm2simics_gasket_dispatcher.o                   \
               tlm2simics_gasket_factory.o                      \
               tlm2simics_i2c_slave_v2.o                        \
               tlm2simics_i3c_master.o                          \
               tlm2simics_i3c_slave.o                           \
               tlm2simics_pci_express.o                         \
               tlm2simics_pcie_map.o                            \
               tlm2simics_serial_device.o                       \
               tlm2simics_transaction_handler.o                 \
               tlm2simics_transaction.o                         \
               trace_monitor.o                                  \
               version.o

UNITTEST_MOCK_AWARENESS_OBJS = unittest_mock_awareness_mock_attribute.o       \

UNITTEST_AWARENESS_OBJS = unittest_awareness_test_attributes.o                \
                          unittest_awareness_test_class_attributes.o          \
                          unittest_awareness_test_proxy_attribute_name.o      \
                          unittest_awareness_test_proxy_class_name.o          \
                          unittest_awareness_test_proxy_class_registry.o      \
                          unittest_awareness_test_proxy_create_traverser.o    \
                          unittest_awareness_test_proxy_name.o                \
                          unittest_awareness_test_tlm_spy_factory.o           \
                          unittest_awareness_test_tlm_spy_factory_registry.o  \
                          unittest_awareness_test_proxy.o

UNITTEST_IFACE_OBJS = unittest_iface_test_ethernet_common_extension.o          \
                      unittest_iface_test_extension.o                          \
                      unittest_iface_test_extension_dispatcher.o               \
                      unittest_iface_test_i2c_master_v2_extension.o            \
                      unittest_iface_test_pci_bus_extension.o                  \
                      unittest_iface_test_pci_device_extension.o               \
                      unittest_iface_test_pci_express_extension.o              \
                      unittest_iface_test_pci_upstream_operation.o             \
                      unittest_iface_test_pcie_device_extension.o              \
                      unittest_iface_test_serial_device_extension.o            \
                      unittest_iface_test_transaction_pool.o

UNITTEST_INJECTION_OBJS = unittest_injection_test_b_transport_invoker.o

UNITTEST_TYPES_OBJS = unittest_types_test_any_type.o

UNITTEST_INSTRUMENTATION_OBJS = \
                     unittest_instrumentation_test_bank_instrumentation_subscribe_simics_adapter.o \
                     unittest_instrumentation_test_tool_controller.o

UNITTEST_QUICKTHREADS_OBJS = unittest_quickthreads_test_qt_db.o

UNITTEST_ROOT_OBJS = unittest_environment.o                  \
                     unittest_main.o                         \
                     unittest_simcontext_wrapper.o           \
                     unittest_stubs.o                        \
                     unittest_test_allocator.o               \
                     unittest_test_argument_registry.o       \
                     unittest_test_cc_api.o                  \
                     unittest_test_connector.o               \
                     unittest_test_device.o                  \
                     unittest_test_interface_provider.o      \
                     unittest_test_kernel.o                  \
                     unittest_test_kernel_state_comparator.o \
                     unittest_test_kernel_state_modifier.o   \
                     unittest_test_late_binder.o             \
                     unittest_test_memory_profiler.o         \
                     unittest_test_multi_traverser.o         \
                     unittest_test_null_simulation.o         \
                     unittest_test_pcie_tlm_extension.o      \
                     unittest_test_process_stack.o           \
                     unittest_test_reference_counter.o       \
                     unittest_test_registry.o                \
                     unittest_test_report_handler.o          \
                     unittest_test_simulation.o              \
                     unittest_test_thread_pool.o             \
                     unittest_test_trace_monitor.o           \
                     unittest_test_version.o

UNITTEST_SIMICS2SYSTEMC_OBJS = unittest_simics2systemc_test_signal.o

UNITTEST_SIMICS2TLM_OBJS = unittest_simics2tlm_test_dmi_data_table.o          \
                           unittest_simics2tlm_test_gasket.o                  \
                           unittest_simics2tlm_test_gasket_with_pool.o        \
                           unittest_simics2tlm_test_gasket_factory.o          \
                           unittest_simics2tlm_test_gasket_owner.o            \
                           unittest_simics2tlm_test_io_memory.o               \
                           unittest_simics2tlm_test_i2c_master_v2.o           \
                           unittest_simics2tlm_test_i2c_slave_v2.o            \
                           unittest_simics2tlm_test_i3c_master.o              \
                           unittest_simics2tlm_test_i3c_slave.o               \
                           unittest_simics2tlm_test_mii_management.o          \
                           unittest_simics2tlm_test_multi_gasket_owner.o      \
                           unittest_simics2tlm_test_null_gasket.o             \
                           unittest_simics2tlm_test_pci_express.o             \
                           unittest_simics2tlm_test_pci_device.o              \
                           unittest_simics2tlm_test_pcie_device.o             \
                           unittest_simics2tlm_test_register_view.o           \
                           unittest_simics2tlm_test_serial_device.o

UNITTEST_SYSTEMC2SIMICS_OBJS = unittest_systemc2simics_test_signal.o

UNITTEST_TLM2SIMICS_OBJS = unittest_tlm2simics_test_dmi_transaction_handler.o \
                           unittest_tlm2simics_test_gasket.o                  \
                           unittest_tlm2simics_test_gasket_factory.o          \
                           unittest_tlm2simics_test_gasket_owner.o            \
                           unittest_tlm2simics_test_i2c_slave_v2.o            \
                           unittest_tlm2simics_test_i3c_master.o              \
                           unittest_tlm2simics_test_i3c_slave.o               \
                           unittest_tlm2simics_test_null_gasket.o             \
                           unittest_tlm2simics_test_pci_express.o             \
                           unittest_tlm2simics_test_pcie_map.o                \
                           unittest_tlm2simics_test_serial_device.o           \
                           unittest_tlm2simics_test_transaction_handler.o     \
                           unittest_tlm2simics_test_transaction.o

UNITTEST_COMPOSITE_OBJS = unittest_composite_test_pcie_mapping_interconnect.o

UNITTEST_ACCELLERA_OBJS = unittest_test_accellera.o

# This section includes tests which require access to the Simics core.
# If this is not available, use the stubs and don't run the tests
ifneq ($(_CORE_PROJECT_BUILD),)
    UNITTEST_ATTR_VALUE_OBJS = attr-val.o attr-val-stubs.o attr-type.o
    UNITTEST_CC_API_OBJS = conf-class.o conf-object.o
    ifneq ($(USE_SIMICS_CCI),no)
        UNITTEST_ROOT_OBJS += unittest_test_cci_attribute.o
        UNITTEST_ROOT_OBJS += unittest_test_cci_configuration.o
    endif
    UNITTEST_ROOT_OBJS += unittest_test_gasket_tag.o
    UNITTEST_ROOT_OBJS += unittest_test_sc_signal_access.o
    UNITTEST_IFACE_OBJS += unittest_iface_test_sc_tlm_bw_transport_simics_adapter.o
    UNITTEST_IFACE_OBJS += unittest_iface_test_sc_tlm_fw_transport_simics_adapter.o
    UNITTEST_INJECTION_OBJS += unittest_injection_test_attr_dict_parser.o
    UNITTEST_INJECTION_OBJS += unittest_injection_test_attr_dmi.o
    UNITTEST_INJECTION_OBJS += unittest_injection_test_inject_registry.o
    UNITTEST_INJECTION_OBJS += unittest_injection_test_inject_gp.o
    UNITTEST_AWARENESS_OBJS += unittest_awareness_test_sc_attribute.o
else
    UNITTEST_ROOT_OBJS += unittest_attr_value_stubs.o
endif

CXXFLAGS := $(CXXFLAGS) $(BLD_CXXFLAGS) $(SC_CFLAGS) -I$(UNITTEST_ROOT) \
            -I$(BOOST_INC_PATH) -Wno-sign-compare -Wno-pmf-conversions \
            -Wno-maybe-uninitialized \
            -B$(BINUTILS)

_COMPILE=$(CXX) $(CXXFLAGS) -c $< -o $@

$(UNITTEST_MOCK_AWARENESS_OBJS): unittest_mock_awareness_%.o: $(SYSTEMC_LIBRARY_SRC)/unittest/mock/awareness/%.cc $(SIMICS_REPO_ROOT)/core/src/include/simics/build-id-6.h
	$(info Compiling $@)
	$(_COMPILE)

$(UNITTEST_AWARENESS_OBJS): unittest_awareness_%.o: $(SYSTEMC_LIBRARY_SRC)/unittest/awareness/%.cc $(SIMICS_REPO_ROOT)/core/src/include/simics/build-id-6.h
	$(info Compiling $@)
	$(_COMPILE)

$(UNITTEST_IFACE_OBJS): unittest_iface_%.o: $(SYSTEMC_LIBRARY_SRC)/unittest/iface/%.cc $(SIMICS_REPO_ROOT)/core/src/include/simics/build-id-6.h
	$(info Compiling $@)
	$(_COMPILE)

$(UNITTEST_INJECTION_OBJS): unittest_injection_%.o: $(SYSTEMC_LIBRARY_SRC)/unittest/injection/%.cc $(SIMICS_REPO_ROOT)/core/src/include/simics/build-id-6.h
	$(info Compiling $@)
	$(_COMPILE)

$(UNITTEST_INSTRUMENTATION_OBJS): unittest_instrumentation_%.o: $(SYSTEMC_LIBRARY_SRC)/unittest/instrumentation/%.cc $(SIMICS_REPO_ROOT)/core/src/include/simics/build-id-6.h
	$(info Compiling $@)
	$(_COMPILE)

$(UNITTEST_TYPES_OBJS): unittest_types_%.o: $(SYSTEMC_LIBRARY_SRC)/unittest/types/%.cc $(SIMICS_REPO_ROOT)/core/src/include/simics/build-id-6.h
	$(info Compiling $@)
	$(_COMPILE)

$(UNITTEST_QUICKTHREADS_OBJS): unittest_quickthreads_%.o: $(SYSTEMC_LIBRARY_SRC)/unittest/quickthreads/%.cc $(SIMICS_REPO_ROOT)/core/src/include/simics/build-id-6.h
	$(info Compiling $@)
	$(_COMPILE) -I$(QT_DB_INC)

$(UNITTEST_ROOT_OBJS): unittest_%.o: $(SYSTEMC_LIBRARY_SRC)/unittest/%.cc       \
                       $(wildcard $(SYSTEMC_LIBRARY_SRC)/unittest/*.h) \
                       $(SIMICS_REPO_ROOT)/core/src/include/simics/build-id-6.h
	$(info Compiling $@)
	$(_COMPILE)

$(UNITTEST_SIMICS2SYSTEMC_OBJS): unittest_simics2systemc_%.o: $(SYSTEMC_LIBRARY_SRC)/unittest/simics2systemc/%.cc $(SIMICS_REPO_ROOT)/core/src/include/simics/build-id-6.h
	$(info Compiling $@)
	$(_COMPILE)

$(UNITTEST_SIMICS2TLM_OBJS): unittest_simics2tlm_%.o: $(SYSTEMC_LIBRARY_SRC)/unittest/simics2tlm/%.cc $(SYSTEMC_LIBRARY_SRC)/simics/systemc/simics2tlm/gasket.cc $(SIMICS_REPO_ROOT)/core/src/include/simics/build-id-6.h
	$(info Compiling $@)
	$(_COMPILE)

$(UNITTEST_SYSTEMC2SIMICS_OBJS): unittest_systemc2simics_%.o: $(SYSTEMC_LIBRARY_SRC)/unittest/systemc2simics/%.cc $(SIMICS_REPO_ROOT)/core/src/include/simics/build-id-6.h
	$(info Compiling $@)
	$(_COMPILE)

$(UNITTEST_TLM2SIMICS_OBJS): unittest_tlm2simics_%.o: $(SYSTEMC_LIBRARY_SRC)/unittest/tlm2simics/%.cc $(SIMICS_REPO_ROOT)/core/src/include/simics/build-id-6.h
	$(info Compiling $@)
	$(_COMPILE)

$(UNITTEST_COMPOSITE_OBJS): unittest_composite_%.o: $(SYSTEMC_LIBRARY_SRC)/unittest/composite/%.cc $(SIMICS_REPO_ROOT)/core/src/include/simics/build-id-6.h
	$(info Compiling $@)
	$(_COMPILE)

$(UNITTEST_ACCELLERA_OBJS): unittest_%.o: $(SYSTEMC_LIBRARY_SRC)/unittest/%.cc       \
                            $(wildcard $(SYSTEMC_LIBRARY_SRC)/unittest/*.h)
	$(info Compiling (with Accellera kernel) $@)
	$(CXX) -I$(UNITTEST_ROOT) -I$(BOOST_INC_PATH) -O2 -g -fPIC -D_LARGEFILE64_SOURCE -D_REENTRANT -I$(accellera)/include -fvisibility=hidden -o $@ -c $<

$(UNITTEST_ATTR_VALUE_OBJS): %.o: $(SIMICS_REPO_ROOT)/core/src/core/conf/%.c
	$(info Compiling (from Simics core) $@)
	$(CC) -I$(SIMICS_REPO_ROOT)/core/src/include/ -std=c99 -g -DSIMICS_CORE -o $@ -c $<

$(UNITTEST_CC_API_OBJS): %.o: $(SIMICS_REPO_ROOT)/src/devices/c++-api/%.cc
	$(info Compiling (from Simics C++ API) $@)
	$(_COMPILE)

UNITTEST_OBJS = $(UNITTEST_MOCK_AWARENESS_OBJS) \
                $(UNITTEST_ATTR_VALUE_OBJS)     \
                $(UNITTEST_AWARENESS_OBJS)      \
                $(UNITTEST_CC_API_OBJS)         \
                $(UNITTEST_IFACE_OBJS)          \
                $(UNITTEST_INJECTION_OBJS)      \
                $(UNITTEST_INSTRUMENTATION_OBJS)\
                $(UNITTEST_MOCK_OBJS)           \
                $(UNITTEST_TYPES_OBJS)          \
                $(UNITTEST_ROOT_OBJS)           \
                $(UNITTEST_SIMICS2SYSTEMC_OBJS) \
                $(UNITTEST_SIMICS2TLM_OBJS)     \
                $(UNITTEST_SYSTEMC2SIMICS_OBJS) \
                $(UNITTEST_TLM2SIMICS_OBJS)     \
                $(UNITTEST_COMPOSITE_OBJS)

# In project, we may not always have the QT module depending on which SystemC
# package was installed
ifneq ($(QT_DB_LDFLAGS),)
    UNITTEST_OBJS := $(UNITTEST_QUICKTHREADS_OBJS) $(UNITTEST_OBJS)
endif

# We can only run this internally as we depend on files on NFS
ifneq ($(_CORE_PROJECT_BUILD),)
    UNITTEST_OBJS += $(UNITTEST_ACCELLERA_OBJS)
endif

unittest: systemc-library-unittest
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./systemc-library-unittest
	touch $@

.PHONY : unittest-clean
unittest-clean:
	rm -f $(UNITTEST_OBJS)
	rm -f unittest

UNITTEST_DEPS = $(SC_TEST_OBJS) $(UNITTEST_OBJS)
# The order matters. libcciapi has to be before libsystemc.a for linking
ifneq ($(USE_SIMICS_CCI),no)
    UNITTEST_DEPS += $(LIB_SYSTEMC_PATH)/libcciapi.a
endif
UNITTEST_DEPS += $(LIB_SYSTEMC_PATH)/libsystemc.a

systemc-library-unittest: $(UNITTEST_DEPS)
	$(info Linking $@)
	$(CXX) -o $@ $^ $(TEST_LDFLAGS) $(TEST_LIBS) -Wl,--dynamic-list=$(SYSTEMC_LIBRARY_SRC)/unittest/gdb_dynamic_symbol_table

# Don't fail because of bugs in Boost (GCC 4.9).
%.o: CXXFLAGS += $(CFLAGS_NO_UNUSED_LOCAL_TYPEDEFS)
