# © 2023 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

simics_add_module(systemc-interfaces
  INTERFACES sc_trace sc_object sc_module sc_process sc_initiator_gasket sc_export sc_port sc_process_profiler sc_process_profiler_control sc_memory_profiler sc_memory_profiler_control sc_signal_read sc_signal_write sc_tlm_fw_transport sc_tlm_bw_transport sc_vector sc_event sc_register_access sc_memory_access sc_simcontext sc_version sc_gasket_info sc_provider_controller sc_tool_connection
  SOURCES common.py module_load.py profiling_commands.py systemc-interfaces.h systemc-provider-interfaces.h
  SIMICS_API latest
)
