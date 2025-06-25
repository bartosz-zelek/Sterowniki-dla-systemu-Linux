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


import stest
import os

iface_dir = os.path.join(os.pardir, 'simics', 'systemc', 'iface')
iface_header_file = os.path.join(iface_dir, 'iface.h')

def parse_header_file(header):
    includes = set()
    with open(header,'r') as f:
        for l in f:
            (before, sep, rem) = l.partition('#include <simics/systemc/iface/')
            if rem:
                includes.add(rem.replace('>','').rstrip())
    return includes

all_headers = set([f for f in os.listdir(iface_dir) if '.h' in f])
all_includes = parse_header_file(iface_header_file)

excludes = {'concurrency_group_interface.h',
            'concurrency_group_simics_adapter.h',
            'concurrency_mode_interface.h',
            'concurrency_mode_simics_adapter.h',
            'execute_control_interface.h',
            'execute_control_simics_adapter.h',
            'extension_sender_interface.h',
            'iface.h',
            'pci_bus_interface.h',
            'pci_device_query_interface.h',
            'pci_upstream_operation_extension.h',
            'pci_upstream_operation_interface.h',
            'pcie_device_query_interface.h',
            'receiver_interface.h',
            'sc_event_interface.h',
            'sc_event_simics_adapter.h',
            'sc_export_interface.h',
            'sc_export_simics_adapter.h',
            'sc_gasket_info_interface.h',
            'sc_gasket_info_simics_adapter.h',
            'sc_initiator_gasket_interface.h',
            'sc_initiator_gasket_simics_adapter.h',
            'sc_memory_access_interface.h',
            'sc_memory_access_simics_adapter.h',
            'sc_memory_profiler_control_interface.h',
            'sc_memory_profiler_control_simics_adapter.h',
            'sc_memory_profiler_interface.h',
            'sc_memory_profiler_simics_adapter.h',
            'sc_object_interface.h',
            'sc_object_simics_adapter.h',
            'sc_port_interface.h',
            'sc_port_simics_adapter.h',
            'sc_process_interface.h',
            'sc_process_profiler_control_interface.h',
            'sc_process_profiler_control_simics_adapter.h',
            'sc_process_profiler_interface.h',
            'sc_process_profiler_simics_adapter.h',
            'sc_process_simics_adapter.h',
            'sc_register_access_interface.h',
            'sc_register_access_simics_adapter.h',
            'sc_signal_read_interface.h',
            'sc_signal_read_simics_adapter.h',
            'sc_signal_write_interface.h',
            'sc_signal_write_simics_adapter.h',
            'sc_vector_interface.h',
            'sc_vector_simics_adapter.h',
            'sc_version_interface.h',
            'sc_version_simics_adapter.h',
            'simcontext_interface.h',
            'simcontext_simics_adapter.h',
            'simics_adapter.h',
            'simics_adapter_interface.h',
            'temporal_state_interface.h',
            'temporal_state_simics_adapter.h'}
missing = sorted(all_headers - all_includes - excludes)
stest.expect_false(missing, "Missing include(s) %s in %s. " % (missing, iface_header_file))
