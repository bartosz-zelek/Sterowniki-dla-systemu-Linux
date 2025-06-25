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


from os.path import basename
from .common import fmt_time, get_processes
from operator import itemgetter
from simics import conf_object_t, SimExc_General, SIM_get_object
import cli

# See original format_state() in src/extensions/systemc-commands:
def format_state(state):
    if state == 0:
        return 'Normal'
    return ','.join([e[1] for e in [[1, 'Disabled'], [2, 'Ready'],
                                    [4, 'Suspended'],
                                    [8, 'Zombie']] if state & e[0]])

def list_processes_command(adapter, obj):
    processes = get_processes(obj if obj else adapter)
    indent = 17
    lengthOfTab = 8
    indent2 = ((indent + lengthOfTab) // lengthOfTab) * lengthOfTab
    print('Type'.ljust(indent)+'Process')
    print('-' * 3*indent)
    for obj in processes:
        events = obj.iface.sc_process.events()
        if "sc_method_process" in obj.classname:
            if events:
                s = ''
                for event in events:
                    (time, objectname, kind) = event
                    if kind == "dynamic method":
                        s = "Method (dynamic)"
                    elif kind == "static method":
                        s = "Method (static)"
                    else:
                        s = str(kind)
            else:
                s = "Method"
        elif "sc_thread_process" in obj.classname:
            if events:
                for event in events:
                    (time, objectname, kind) = event
                    if kind == "dynamic thread":
                        s = "Thread (dynamic)"
                    elif kind == "static thread":
                        s = "Thread (static)"
                    else:
                        s = str(kind)
            else:
                s = "Thread"
        else:
            s = '%s' % obj.classname
        s = s.ljust(indent)
        s += '%s' % obj.name
        print(s)
        s = 'state: %s' % format_state(obj.iface.sc_process.state())
        print(' '.ljust(indent2) + s)
        filename = obj.iface.sc_process.file()
        if filename:
            # Only threads can do wait(), not methods
            s = 'wait: %s' % basename(filename)
            s += ':%s' % str(obj.iface.sc_process.line())
            print(' '.ljust(indent2) + s)
        if events:
            # See ProxyProcess::events() how the events list is arranged:
            for event in events:
                (time, _, _) = event
                s = 'time: %s' % str(time)
                print(' '.ljust(indent2) + s)

def print_top_limit_results(results, limit, topic):
    '''Print the top limit number result from results with head info.'''
    print_results = None
    if not results:
        pass
    elif limit == 0:
        print_results = results[:10]
    else:
        print_results = results[:limit]

    if print_results:
        length = len(print_results)
        if length < len(results):
            topic += (" (limited to top %d process%s)" %
                      (length, "es" if length > 1 else ""))
        cli.print_info([(topic, print_results)])

def process_parent_expander(string, obj):
    '''Expand a string with the list of objects which have processes
    available as its child objects (recursively)'''
    objs_has_processes = []
    for process in get_processes(obj):
        prefix = process.component.name.split(".")
        for level in range(len(prefix)):
            objs_has_processes.append(".".join(prefix[:level + 1]))
    return cli.get_completions(string, list(set(objs_has_processes)))

class MemoryProfilingSupport:
    def __init__(self, *new_command_args):
        (self.obj,) = new_command_args
        self.profiling_control = self.obj.iface.sc_memory_profiler_control

    def what(self):
        return 'Memory profiling support for SystemC processes'

    def is_enabled(self):
        return self.profiling_control.is_enabled()

    def set_enabled(self, enable):
        processes = get_processes(self.obj)
        if not processes:
            raise cli.CliError('There are no child processes to profile')
        else:
            self.profiling_control.set_enabled(enable)

    def results(self, limit, obj):
        def total_bytes_allocated(allocations):
            return sum([size for (_, size, _) in allocations])

        def summarize_allocations(allocations):
            return '%s bytes, %s allocations' %\
                (total_bytes_allocated(allocations),
                 len(allocations))

        processes = get_processes(obj if obj else self.obj)
        process_allocations = [
            (p.iface.sc_object.sc_name(),
             summarize_allocations(p.iface.sc_memory_profiler.allocations()),
             total_bytes_allocated(p.iface.sc_memory_profiler.allocations()))
            for p in processes]
        memory_use_status = [(name, summary) for (name, summary, allocs)
                             in sorted(process_allocations,
                                       key=itemgetter(2),
                                       reverse=True) if allocs != 0]
        print_top_limit_results(memory_use_status, limit, "Memory usage")

class ProcessProfilerSupport:
    def __init__(self, *new_command_args):
        (self.obj,) = new_command_args
        assert isinstance(self.obj, conf_object_t)

    def what(self):
        return "Process profiler"

    def iface(self):
        return self.obj.iface.sc_process_profiler_control

    def is_enabled(self):
        return self.iface().is_enabled()

    def set_enabled(self, enable):
        self.iface().set_enabled(enable)

    def clear_data(self):
        self.iface().clear_data()

    def results(self, limit, obj):
        if not self.is_enabled():
            print("%s is disabled." % self.what())
            return

        processes = get_processes(obj if obj else self.obj)

        methods_number_of_calls = 0
        threads_number_of_calls = 0
        process_consumption = []
        total_time = 0
        total_call = 0
        for process in processes:
            pp = process.iface.sc_process_profiler
            total_time += pp.total_time()
            total_call += pp.number_of_calls()
            if "sc_method_process" in process.classname:
                methods_number_of_calls += pp.number_of_calls()
                process_consumption.append(("%s (method)" % process.name,
                                            pp.total_time(),
                                            pp.number_of_calls()))
            elif "sc_thread_process" in process.classname:
                threads_number_of_calls += pp.number_of_calls()
                process_consumption.append(("%s (thread)" % process.name,
                                            pp.total_time(),
                                            pp.number_of_calls()))
            else:
                # Unsupported classes
                assert False

        # If we do not have a obj argument, we use the global profiling information
        # that include spawned process data.
        if obj is None:
            total_time = self.iface().total_time()
            total_call = self.iface().total_number_of_calls()
        dynamic_calls = (total_call
                         - (methods_number_of_calls + threads_number_of_calls))

        print("In total %d calls, %s consumed" % (total_call, fmt_time(total_time)))
        msg = ("(method: %d, thread: %d, spawned: "
               % (methods_number_of_calls, threads_number_of_calls))
        msg += "-)" if obj else "%d)" % dynamic_calls
        print(msg)

        # List the top time consumed processes
        process_call_status = [
            (name, (fmt_time(consumed_time), calls)) for\
            (name, consumed_time, calls)
            in sorted(process_consumption,
                      key=itemgetter(1),
                      reverse=True) if calls != 0]
        print_top_limit_results(process_call_status, limit,
                                "Process profiler result")

def add_commands():
    cli.new_command('list-processes', list_processes_command,
                    [cli.arg(cli.obj_t('object'), 'object', "?",
                             expander = process_parent_expander)],
                    iface = "sc_simcontext",
                    type = [ 'Profiling', 'Debugging', 'SystemC' ],
                    short = 'lists methods and threads',
                    doc = """
    Lists methods and threads implementing the sc_process interface.
    If scheduled on the time axis, the time will appear.
    If the device source code use the debug facility WAIT*() instead of wait(),
    or sets the file and line information accordingly,
    the source code line of the passive wait() or WAIT*() call will appear.
    <arg>object</arg> limits the scope of listed processes to only those
    under its hierarchy.
    """)

    cli.new_command('enable-memory-profiler',
                    cli.enable_cmd(MemoryProfilingSupport), [],
                    iface = "sc_memory_profiler_control",
                    type = [ 'Profiling', 'SystemC' ],
                    short = 'enables memory profiling',
                    doc = """
    Enables memory profiling for SystemC processes. This allows inspection of the
    number and size of allocations performed in a SystemC thread or method.
    """)

    cli.new_command('disable-memory-profiler',
                    cli.disable_cmd(MemoryProfilingSupport), [],
                    iface = "sc_memory_profiler_control",
                    type = [ 'Profiling', 'SystemC' ],
                    short = 'disables memory profiling',
                    doc = """
    Disables memory profiling for SystemC processes. This stops recording of
    allocations made by SystemC threads and methods.
    """)

    cli.new_command("memory-profiler-results",
                    lambda o, limit, obj: MemoryProfilingSupport(
                        o).results(limit, obj),
                    [cli.arg(cli.uint_t, "limit", "?", 0),
                     cli.arg(cli.obj_t('object'), 'object', "?",
                             expander = process_parent_expander)],
                    iface = "sc_memory_profiler_control",
                    type = [ 'Profiling', 'SystemC' ],
                    short = "show memory profiler results",
                    doc = """
    Lists memory usage of method and thread processes, as well as the number
    of allocations.
    <arg>limit</arg> specifies how many processes are listed in the result.
    <arg>object</arg> limits the scope of listed processes to only those
    under its hierarchy.
    """)

    cli.new_command("enable-process-profiler",
                    cli.enable_cmd(ProcessProfilerSupport), [],
                    iface = "sc_process_profiler_control",
                    type = [ 'Profiling', 'SystemC' ],
                    short = "enable use of process profiler",
                    see_also = [
                        "<sc_process_profiler_control>.disable-process-profiler" ],
                    doc = """
    Enables use of the process-profiler. Can be disabled with the
    <cmd class="sc_process_profiler_control">disable-process-profiler</cmd>
    command.""")

    cli.new_command("disable-process-profiler",
                    cli.disable_cmd(ProcessProfilerSupport), [],
                    iface = "sc_process_profiler_control",
                    type = [ 'Profiling', 'SystemC' ],
                    short = "disable use of process profiler",
                    see_also = [
                        "<sc_process_profiler_control>.enable-process-profiler" ],
                    doc = """
    Disables use of the process-profiler. Can be enabled with the
    <cmd class="sc_process_profiler_control">enable-process-profiler</cmd>
    command.""")

    cli.new_command("clear-process-profiler-results",
                    lambda o: ProcessProfilerSupport(
                        o).clear_data(), [],
                    iface = "sc_process_profiler_control",
                    type = [ 'Profiling', 'SystemC' ],
                    short = "clear process profiler results",
                    doc = """
    Clears accumulated profiling results. All process profiler data including
    the aggregated execution time is cleared.""")

    cli.new_command("process-profiler-results",
                    lambda o, limit, obj: ProcessProfilerSupport(
                        o).results(limit, obj),
                    [cli.arg(cli.uint_t, "limit", "?", 0),
                     cli.arg(cli.obj_t('object'), 'object', "?",
                             expander = process_parent_expander)],
                    iface = "sc_process_profiler_control",
                    type = [ 'Profiling', 'SystemC' ],
                    short = "show process profiler results",
                    doc = """
    Lists information of the total number of calls of method and thread
    processes, as well as the aggregated executiontime of those.
    <arg>limit</arg> specifies how many processes are listed in the result.
    <arg>object</arg> limits the scope of listed processes to only those
    under its hierarchy.""")
