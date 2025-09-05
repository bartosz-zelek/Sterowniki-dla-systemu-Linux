# © 2019 Intel Corporation
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
# objects that get linked into the SystemC library
#

SC_LIBRARY_OBJS= \
	$(SIMICS2TLM_OBJS) \
	$(TLM2SIMICS_OBJS) \
	$(TOOLS_OBJS) \
	$(SYSTEMC2SIMICS_OBJS) \
	$(SIMICS2SYSTEMC_OBJS) \
	$(AWARENESS_OBJS) \
	$(INJECTION_OBJS) \
	$(INSTRUMENTATION_OBJS) \
	$(COMMON_OBJS) \
	$(VERSION_OBJS)

SIMICS2TLM_PREF = simics2tlm_
SIMICS2TLM_OBJS = \
	$(SIMICS2TLM_PREF)dmi_data_table.$(OBJEXT) \
	$(SIMICS2TLM_PREF)ethernet_common.$(OBJEXT) \
	$(SIMICS2TLM_PREF)gasket_factory.$(OBJEXT) \
	$(SIMICS2TLM_PREF)i2c_master_v2.$(OBJEXT) \
	$(SIMICS2TLM_PREF)i2c_slave_v2.$(OBJEXT) \
	$(SIMICS2TLM_PREF)i3c_master.$(OBJEXT) \
	$(SIMICS2TLM_PREF)i3c_slave.$(OBJEXT) \
	$(SIMICS2TLM_PREF)io_memory.$(OBJEXT) \
	$(SIMICS2TLM_PREF)mii_management.$(OBJEXT) \
	$(SIMICS2TLM_PREF)packet.$(OBJEXT) \
	$(SIMICS2TLM_PREF)pci_device.$(OBJEXT) \
	$(SIMICS2TLM_PREF)pci_express.$(OBJEXT) \
	$(SIMICS2TLM_PREF)pcie_device.$(OBJEXT) \
	$(SIMICS2TLM_PREF)pcie_transaction.$(OBJEXT) \
	$(SIMICS2TLM_PREF)serial_device.$(OBJEXT) \
	$(SIMICS2TLM_PREF)spi_master.$(OBJEXT) \
	$(SIMICS2TLM_PREF)transaction.$(OBJEXT)

TLM2SIMICS_PREF = tlm2simics_
TLM2SIMICS_OBJS = \
	$(TLM2SIMICS_PREF)dmi_transaction_handler.$(OBJEXT) \
	$(TLM2SIMICS_PREF)error_transaction_handler.$(OBJEXT) \
	$(TLM2SIMICS_PREF)ethernet_common.$(OBJEXT) \
	$(TLM2SIMICS_PREF)gasket_dispatcher.$(OBJEXT) \
	$(TLM2SIMICS_PREF)gasket_factory.$(OBJEXT) \
	$(TLM2SIMICS_PREF)i2c_master_v2.$(OBJEXT) \
	$(TLM2SIMICS_PREF)i2c_slave_v2.$(OBJEXT) \
	$(TLM2SIMICS_PREF)i3c_master.$(OBJEXT) \
	$(TLM2SIMICS_PREF)i3c_slave.$(OBJEXT) \
	$(TLM2SIMICS_PREF)memory_space.$(OBJEXT) \
	$(TLM2SIMICS_PREF)packet.$(OBJEXT) \
	$(TLM2SIMICS_PREF)pci_bus.$(OBJEXT) \
	$(TLM2SIMICS_PREF)pci_express.$(OBJEXT) \
    $(TLM2SIMICS_PREF)pcie_map.$(OBJEXT) \
    $(TLM2SIMICS_PREF)pcie_transaction.$(OBJEXT) \
	$(TLM2SIMICS_PREF)serial_device.$(OBJEXT) \
	$(TLM2SIMICS_PREF)spi_slave.$(OBJEXT) \
	$(TLM2SIMICS_PREF)transaction.$(OBJEXT) \
	$(TLM2SIMICS_PREF)transaction_handler.$(OBJEXT)

TOOLS_PREF = tools_
TOOLS_OBJS = \
	$(TOOLS_PREF)sc_break_tool.$(OBJEXT) \
	$(TOOLS_PREF)sc_protocol_checker_tool.$(OBJEXT) \
	$(TOOLS_PREF)sc_tool.$(OBJEXT) \
	$(TOOLS_PREF)sc_trace_tool.$(OBJEXT) \
	$(TOOLS_PREF)sc_transaction_tracker_extension.$(OBJEXT) \
	$(TOOLS_PREF)sc_transaction_tracker_tool.$(OBJEXT) \
	$(TOOLS_PREF)sc_vcd_trace_tool.$(OBJEXT) \
	$(TOOLS_PREF)systemc_tools.$(OBJEXT)

SYSTEMC2SIMICS_PREF = systemc2simics_
SYSTEMC2SIMICS_OBJS = \
	$(SYSTEMC2SIMICS_PREF)gasket.$(OBJEXT) \
	$(SYSTEMC2SIMICS_PREF)gasket_serializable.$(OBJEXT) \
	$(SYSTEMC2SIMICS_PREF)signal.$(OBJEXT) \
	$(SYSTEMC2SIMICS_PREF)signal_serializable.$(OBJEXT) \
	$(SYSTEMC2SIMICS_PREF)signal_class.$(OBJEXT) \
	$(SYSTEMC2SIMICS_PREF)signal_class_serializable.$(OBJEXT)

SIMICS2SYSTEMC_PREF = simics2systemc_
SIMICS2SYSTEMC_OBJS = \
	$(SIMICS2SYSTEMC_PREF)gasket_serializable.$(OBJEXT) \
	$(SIMICS2SYSTEMC_PREF)signal.$(OBJEXT) \
	$(SIMICS2SYSTEMC_PREF)signal_serializable.$(OBJEXT) \
	$(SIMICS2SYSTEMC_PREF)signal_class.$(OBJEXT) \
	$(SIMICS2SYSTEMC_PREF)signal_class_serializable.$(OBJEXT)

AWARENESS_PREF = awareness_
AWARENESS_OBJS = \
	$(AWARENESS_PREF)attributes.$(OBJEXT) \
	$(AWARENESS_PREF)cci_attribute.$(OBJEXT) \
	$(AWARENESS_PREF)class_attributes.$(OBJEXT) \
	$(AWARENESS_PREF)connection_end_point.$(OBJEXT) \
	$(AWARENESS_PREF)proxy_create_traverser.$(OBJEXT) \
	$(AWARENESS_PREF)proxy.$(OBJEXT) \
	$(AWARENESS_PREF)proxy_attribute_name.$(OBJEXT) \
	$(AWARENESS_PREF)proxy_builder.$(OBJEXT) \
	$(AWARENESS_PREF)proxy_class_name.$(OBJEXT) \
	$(AWARENESS_PREF)proxy_event.$(OBJEXT) \
	$(AWARENESS_PREF)proxy_export.$(OBJEXT) \
	$(AWARENESS_PREF)proxy_factory_base.$(OBJEXT) \
	$(AWARENESS_PREF)proxy_name.$(OBJEXT) \
	$(AWARENESS_PREF)proxy_port.$(OBJEXT) \
	$(AWARENESS_PREF)proxy_process.$(OBJEXT) \
	$(AWARENESS_PREF)proxy_signal.$(OBJEXT) \
	$(AWARENESS_PREF)proxy_signal_port.$(OBJEXT) \
	$(AWARENESS_PREF)proxy_vector.$(OBJEXT) \
	$(AWARENESS_PREF)sc_export_connection.$(OBJEXT) \
	$(AWARENESS_PREF)sc_port_connection.$(OBJEXT) \
	$(AWARENESS_PREF)signal_reader.$(OBJEXT) \
	$(AWARENESS_PREF)signal_writer.$(OBJEXT) \
	$(AWARENESS_PREF)tlm2_base_protocol_checker.$(OBJEXT) \
	$(AWARENESS_PREF)tlm_multi_handler_registry.$(OBJEXT)

INJECTION_PREF = injection_
INJECTION_OBJS = \
	$(INJECTION_PREF)attr_dict_parser.$(OBJEXT)

INSTRUMENTATION_PREF = instrumentation_
INSTRUMENTATION_OBJS = \
	$(INSTRUMENTATION_PREF)tool_connection.$(OBJEXT) \
	$(INSTRUMENTATION_PREF)tool_controller.$(OBJEXT)

COMMON_OBJS = \
	adapter.$(OBJEXT) \
	adapter_checkpointable.$(OBJEXT) \
	argument_registry.$(OBJEXT) \
	cci_configuration.$(OBJEXT) \
	checkpoint.$(OBJEXT) \
	connector.$(OBJEXT) \
	context.$(OBJEXT) \
	event.$(OBJEXT) \
	gasket_info.$(OBJEXT) \
	gasket_tag.$(OBJEXT) \
	init_gasket_classes.$(OBJEXT) \
	init_gasket_classes_checkpointable.$(OBJEXT) \
	internal.$(OBJEXT) \
	internal_processes.$(OBJEXT) \
	kernel_state_comparator.$(OBJEXT) \
	kernel_state_modifier.$(OBJEXT) \
	late_binder.$(OBJEXT) \
	memory_profiler.$(OBJEXT) \
	memory_profiling.$(OBJEXT) \
	memory_profiling_utils.$(OBJEXT) \
	module_loaded.$(OBJEXT) \
	null_simulation.$(OBJEXT) \
	pcie_tlm_extension.$(OBJEXT) \
	process_profiler_control.$(OBJEXT) \
	process_stack.$(OBJEXT) \
	report_handler.$(OBJEXT) \
	sc_memory_profiler_control.$(OBJEXT) \
	sc_signal_access.$(OBJEXT) \
	sc_signal_access_base.$(OBJEXT) \
	sc_signal_access_registry.$(OBJEXT) \
	sim_context_proxy.$(OBJEXT) \
	simcontext.$(OBJEXT) \
	simulation.$(OBJEXT) \
	simulation_deleter.$(OBJEXT) \
	thread_pool.$(OBJEXT) \
	trace_monitor.$(OBJEXT) \
	trace_event_all_dynamic.$(OBJEXT) \
	trace_process_all_dynamic.$(OBJEXT) \
	version.$(OBJEXT)
