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


from simics import SIM_create_object, SIM_get_all_modules, VT_get_interfaces
from instrumentation import make_tool_commands
import cli
from . import tracing_commands as tc

def _socket_requirements(obj):
    if not hasattr(obj.iface, 'sc_provider_controller'):
        return False
    if not hasattr(obj.iface, 'sc_object'):
        return False
    return obj.iface.sc_object.sc_kind() in ("tlm_target_socket",
                                             "tlm_initiator_socket",)

def _functions_arg():
    '''Generate a tuple of three elements (args, fn, doc) passing as
    <arg>connect_extra_args</arg> for make_tool_commands.'''
    def pre_connect(obj, provider, functions):
        if provider == None and functions:
            raise cli.CliError("Functions argument given without a provider")
        if provider and _socket_requirements(provider):
            return (functions, "functions = %s" % str(functions))

    def functions_expander(str, obj, prev_args):
        return cli.get_completions(str, tc.functions)

    func_text = """The optional <arg>functions</arg> argument decides which
    TLM socket functions the tool will listen to. If no function is provided,
    the tool will listen to all possible functions. It has no effects on
    providers other than socket providers."""
    return ([cli.arg(cli.string_set_t(tc.functions), "functions", "*",
                     expander = functions_expander)],
            pre_connect, func_text)

def _create_logger(tool_class, name, filename):
    return SIM_create_object(tool_class, name, filepath=filename)

def _create_tool(tool_class, name, internal):
    return SIM_create_object(tool_class, name, internal=internal)

_common_text = """when following actions happen: an event is notified,
    a process instance is triggered or resumed, a signal's value is
    changed and a method in a TLM socket is called."""
_help_text = {
    "sc_trace_tool": """Enables tracing on the SystemC object. A log message
        is printed every time %s Setting the flag <tt>-i</tt> additionally
        traces internal processes.""" % _common_text,
    "sc_break_tool": """Enables breaking simulation on the SystemC object. A
        log message is printed and simulation is stopped every time
        %s Setting the flag <tt>-i</tt> additionally stops the simulation for
        internal processes.""" % _common_text,
    "sc_vcd_trace_tool": """Enables VCD tracing on the SystemC object. A new
        entry is created in the VCD log <arg>file</arg> %s""" % _common_text,
    "sc_protocol_checker_tool": """Enables protocol verification on TLM
        sockets.""",
    "sc_transaction_tracker_tool": """Enables transactions tracking on TLM
        sockets.""",
}

def _make_sc_tool_command(module_name, tool, class_names):
    kwargs = {
        'simics_class' : module_name + "_" + tool,
        'object_prefix' : module_name + "_" + tool + "_",
        'new_cmd_doc' : _help_text[tool],
        'new_cmd_name' : "new-" + tool.replace('_', '-')
    }
    if tool in ("sc_protocol_checker_tool", "sc_transaction_tracker_tool",):
        kwargs['provider_requirements'] = _socket_requirements
    else:
        kwargs['provider_requirements'] = "sc_provider_controller"
    if tool in ("sc_trace_tool", "sc_break_tool",):
        kwargs['connect_extra_args'] = _functions_arg()
        kwargs['new_cmd_extra_args'] = ([cli.arg(cli.flag_t, '-i')],
                                        _create_tool)
    if tool == "sc_vcd_trace_tool":
        kwargs['new_cmd_extra_args'] = ([cli.arg(cli.filename_t(), 'file',
                                                 '?', 'trace.vcd')],
                                        _create_logger)
    for class_name in class_names:
        kwargs['tied_to_class'] = class_name
        make_tool_commands(**kwargs)

def _get_device_info(obj):
    return []

def _get_device_status(obj):
    return []

def sc_tool_make_tool_commands(module_name):
    tc.add_commands()
    class_names = []
    for mod in SIM_get_all_modules():
        if mod[0] == module_name:
            for class_name in mod[7]:
                # Some class may not ready yet
                try:
                    if "sc_simcontext" in VT_get_interfaces(class_name):
                        class_names.append(class_name)
                except:
                    continue
            break
    underscore_name = module_name.replace("-", "_")
    for tool in _help_text:
        _make_sc_tool_command(underscore_name, tool, class_names)
    cli.new_info_command(underscore_name + '_tool_connection', _get_device_info)
    cli.new_status_command(underscore_name + '_tool_connection',
                           _get_device_status)
