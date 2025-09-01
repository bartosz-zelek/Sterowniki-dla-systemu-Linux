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


from simics import SIM_object_iterator_for_interface
from simmod.systemc_interfaces.common import fmt_time

def format_events(events):
    return sorted([[kind, time] for (time, name, kind) in events])

def format_state(state):
    if state == 0:
        return 'Normal'
    return ','.join([e[1] for e in [[1, 'Disabled'], [2, 'Ready'],
                                    [4, 'Suspended'],
                                    [8, 'Zombie']] if state & e[0]])

def format_file(fname, line):
    if fname is None:
        return "Use WAIT* macros to enable wait tracking"

    return "%s: %d" % (fname, line)

def profiling_enabled(process):
    '''Return if process profiling is enabled on the adapter the process
    belongs to'''
    for obj in SIM_object_iterator_for_interface(
            ["sc_process_profiler_control"]):
        if process.name.startswith(obj.name + '.'):
            return obj.iface.sc_process_profiler_control.is_enabled()
    return False

def format_profiler_status(obj):
    if profiling_enabled(obj):
        status = "enabled"
    else:
        status = "disabled"
    iface = obj.iface.sc_process_profiler
    if iface.number_of_calls() > 0:
        profiling_data = [("Min time", fmt_time(iface.min_time())),
                          ("Max time", fmt_time(iface.max_time())),
                          ("Total time", fmt_time(iface.total_time())),
                          ("Calls", iface.number_of_calls())]
    else:
        profiling_data = "no data available"
    return [("Execution", [("Profiling status", status),
                           ("Profiling data", profiling_data)])]

def is_memory_profiling_available(processes):
    if not processes:
        return False
    if not hasattr(processes[0].iface, 'sc_memory_profiler'):
        return False  # memory profiling disabled at compile-time
    return True

def get_allocation_total(allocations):
    return sum([size for (_, size, _) in allocations])

def summarize_allocations(allocations):
    return '%s bytes, %s allocations' % (get_allocation_total(allocations),
                                         len(allocations))

def memory_use_process(process):
    if not is_memory_profiling_available([process]):
        return None

    allocations = process.iface.sc_memory_profiler.allocations()
    return ("Memory use",
            [('Total (bytes)', get_allocation_total(allocations)),
             ('Number of allocations', len(allocations))])

def process_state(iface):
    return ("State", format_state(iface.state()))
def process_id(iface):
    return ("Process ID", iface.process_id())
def process_events(iface):
    return ("Events", format_events(iface.events()))
def waiting_at(obj, iface):
    if 'thread' in obj.iface.sc_object.sc_kind():
        at = format_file(iface.file(), iface.line())
    else:
        at = "<not applicable for methods>"
    return ("Waiting at", at)
def sc_process_status(obj, iface):
    # sc_process and sc_process_profiler are always registered by the process
    # proxy factory
    status = [(None, [process_state(iface),
                      process_id(iface),
                      waiting_at(obj, iface)]),
              process_events(iface)]
    memory_use = memory_use_process(obj)
    if memory_use:
        status.append(memory_use)
    status += format_profiler_status(obj)
    return status

# NOTE: the status command is registered at the end, after info commands
# --- end helper methods for process status commands
