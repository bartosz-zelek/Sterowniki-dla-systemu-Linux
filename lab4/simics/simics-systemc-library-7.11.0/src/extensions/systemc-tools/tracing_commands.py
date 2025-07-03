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


from simics import SIM_create_object, SIM_get_object, SimExc_General
from simics import SIM_object_iterator_for_interface
import cli

functions = ["nb_transport_fw_pre",
             "nb_transport_fw_post",
             "b_transport_pre",
             "b_transport_post",
             "transport_dbg_pre",
             "transport_dbg_post",
             "nb_transport_bw_pre",
             "nb_transport_bw_post",
             "get_direct_mem_ptr_pre",
             "get_direct_mem_ptr_post",
             "invalidate_direct_mem_ptr_pre",
             "invalidate_direct_mem_ptr_post"]

def sc_objects_of_kinds(adapter, kinds):
    return [obj
            for obj in SIM_object_iterator_for_interface(
                    ["sc_provider_controller"])
            if obj.name.startswith(adapter.name + '.')
            and hasattr(obj.iface, "sc_object")
            and obj.iface.sc_object.sc_kind() in kinds]

def sc_objects_of_class(adapter, classname):
    return [obj
            for obj in SIM_object_iterator_for_interface(
                    ["sc_provider_controller"])
            if obj.name.startswith(adapter.name + '.')
            and obj.classname == classname]

def has_connection(toolname, controller):
    return [obj
            for obj in SIM_object_iterator_for_interface(["sc_tool_connection"])
            if obj.iface.sc_tool_connection.controller() == controller
            and obj.iface.sc_tool_connection.tool().name == toolname]

def get_adapter_obj(o):
    if hasattr(o.iface, "sc_simcontext"):
        return o

    pos = o.name.rfind('.')
    assert pos > 1

    return get_adapter_obj(SIM_get_object(o.name[:pos]))

def get_object(name):
    try:
        return SIM_get_object(name)
    except SimExc_General:
        return None

def op_objects(op, objs, extra='', internal=None):
    assert objs
    def check_op(op):
        if op.startswith("disable"):
            return (True, op.replace("disable_", ""))
        return (False, op)
    (disable, op) = check_op(op)
    adapter = get_adapter_obj(objs[0]).name
    if not get_object('%s.internal' % adapter):
        SIM_create_object("namespace", "%s.internal" % adapter)
    tool = '%s.internal.sc_%s_tool' % (adapter, op)
    tool_obj = get_object(tool)
    cmd = None
    if tool_obj:
        if disable:
            obj_names = ' '.join([o.name for o in objs
                                  if has_connection(tool, o)])
            cmd = '%s.remove-instrumentation providers = %s' % (
                tool, obj_names)
        else:
            obj_names = ' '.join([o.name for o in objs
                                  if not has_connection(tool, o)])
            if obj_names:
                cmd = '%s.add-instrumentation providers = %s %s' % (
                    tool, obj_names, extra)
    else:
        if not disable:
            obj_names = ' '.join((o.name for o in objs))
            cmd = '%s.new-sc-%s-tool name = %s providers = %s %s' % (
                adapter, op.replace('_', '-'), tool, obj_names, extra)

    if cmd:
        cli.run_command(cmd)
        tool_obj = get_object(tool)

    if tool_obj and internal is not None:
        tool_obj.internal = internal

def enable_trace(o, internal):
    op_objects('trace', [o], internal=internal)

def disable_trace(o):
    op_objects('disable_trace', [o])

def enable_break(o, internal):
    op_objects('break', [o], internal=internal)

def disable_break(o):
    op_objects('disable_break', [o])

def enable_transaction_tracker(o):
    op_objects('transaction_tracker', [o])

def disable_transaction_tracker(o):
    op_objects('disable_transaction_tracker', [o])

def check_objs(objs):
    if not objs:
        raise cli.CliError("No objects found that matches the selected type")
    return objs

# Used by add_commands to register the -all commands for all kinds
def func_builder(action, kind):
    def f(adapter, internal=None):
        if kind == "event":
            objs = sc_objects_of_kinds(adapter, ['ScEventObject'])
            objs += sc_objects_of_class(adapter, 'ScEventObjectAllDynamic')
            op_objects(action, check_objs(objs))
        elif kind == "process":
            objs = sc_objects_of_kinds(adapter, ['sc_thread_process',
                                                 'sc_method_process'])
            objs += sc_objects_of_class(adapter,
                                        'sc_method_process_all_dynamic')
            op_objects(action, check_objs(objs), internal=internal)
        elif kind == "signal":
            op_objects(action,
                       check_objs(sc_objects_of_kinds(adapter,
                                                      ['sc_signal', 'sc_in',
                                                       'sc_out', 'sc_inout'])))
        elif kind == "socket":
            op_objects(action,
                       check_objs(sc_objects_of_kinds(adapter,
                                                      ['tlm_target_socket',
                                                       'tlm_initiator_socket'])))
    return f

def track_transactions_all_cmd(adapter):
    op_objects('transaction_tracker',
               check_objs(sc_objects_of_kinds(adapter,
                                              ['tlm_target_socket',
                                               'tlm_initiator_socket'])))

def untrack_transactions_all_cmd(adapter):
    op_objects('disable_transaction_tracker',
               check_objs(sc_objects_of_kinds(adapter,
                                              ['tlm_target_socket',
                                               'tlm_initiator_socket'])))

_initialized = False

def add_commands():
    # This function will be invoked once, via
    # simics::systemc::tools::initScTools, for each SystemC Adapter module
    # loaded into Simics. To prevent the 'redefining the ... command' error, we
    # must make sure to only run this code once per Simics session.
    global _initialized
    if _initialized:
        return
    _initialized = True

    kind_doc = {
        "event" : "an event is notified",
        "process" : "a process is triggered or resumed",
        "signal" : "a signal's value is changed",
        "socket" : "a method in a TLM socket is called",
    }

    # Trace/break on single object
    cli.new_command("trace-sc", enable_trace,
                    [cli.arg(cli.flag_t, "-i")],
                    type = ['Tracing', 'SystemC'],
                    iface = "sc_provider_controller",
                    short = "trace SystemC object",
                    see_also = ["<sc_provider_controller>.untrace-sc",
                                "<sc_provider_controller>.break-sc"],
                    doc = """
    Enables tracing on the SystemC object. Depends on the object kind, a log
    message is printed every time when following actions happen: %s, %s, %s
    and %s. Setting the flag <tt>-i</tt> additionally traces
    internal processes.""" % tuple(kind_doc.values()))

    cli.new_command("untrace-sc", disable_trace, [],
                    type = ['Tracing', 'SystemC'],
                    iface = "sc_provider_controller",
                    short = "untrace SystemC object",
                    see_also = ["<sc_provider_controller>.trace-sc",
                                "<sc_provider_controller>.unbreak-sc"],
                    doc = """
    Disables tracing on the SystemC object.
    """)

    cli.new_command("break-sc", enable_break,
                    [cli.arg(cli.flag_t, "-i")],
                    type = ['Breakpoints', 'SystemC'],
                    iface = "sc_provider_controller",
                    short = "break on SystemC object",
                    see_also = ["<sc_provider_controller>.unbreak-sc",
                                "<sc_provider_controller>.trace-sc"],
                    doc = """
    Enables breaking simulation on the SystemC object. Depends on the object
    kind, a log message is printed and simulation is stopped every time when
    following actions happen: %s, %s, %s and %s. Setting the flag <tt>-i</tt>
    additionally stops the simulation for internal processes.
                    """ % tuple(kind_doc.values()))

    cli.new_command("unbreak-sc", disable_break, [],
                    type = ['Breakpoints', 'SystemC'],
                    iface = "sc_provider_controller",
                    short = "unbreak on SystemC object",
                    see_also = ["<sc_provider_controller>.break-sc",
                                "<sc_provider_controller>.untrace-sc"],
                    doc = """
    Disables breaking simulation on the SystemC object.
    """)

    # Trace/break on all objects of kind
    for kind in kind_doc:
        cli.new_command("trace-sc-%s-all" % kind,
                        func_builder("trace", kind),
                        [cli.arg(cli.flag_t, "-i")] if kind == 'process' else [],
                        type = ['Tracing', 'SystemC'], iface = "sc_simcontext",
                        short = "trace all SystemC %s objects" % kind,
                        see_also = ["<sc_simcontext>.untrace-sc-%s-all" % kind,
                                    "<sc_provider_controller>.trace-sc"],
                        doc =
                        """Enables tracing on all SystemC %s objects. A log
                        message is printed every time %s. %s"""
                        % (kind, kind_doc[kind],
                           """Setting the flag <tt>-i</tt> additionally
                           traces internal processes.
                           """ if kind == 'process' else ""))

        cli.new_command("untrace-sc-%s-all" % kind,
                        func_builder("disable_trace", kind), [],
                        type = ['Tracing', 'SystemC'], iface = "sc_simcontext",
                        short = "untrace all SystemC %s objects" % kind,
                        see_also = ["<sc_simcontext>.trace-sc-%s-all" % kind,
                                    "<sc_provider_controller>.untrace-sc"],
                        doc =
                        "Disables tracing on all SystemC %s objects." % kind)

        cli.new_command("break-sc-%s-all" % kind,
                        func_builder("break", kind),
                        [cli.arg(cli.flag_t, "-i")] if kind == 'process' else [],
                        type = ['Breakpoints', 'SystemC'],
                        iface = "sc_simcontext",
                        short = "break all SystemC %s objects" % kind,
                        see_also = ["<sc_simcontext>.unbreak-sc-%s-all" % kind,
                                    "<sc_provider_controller>.break-sc"],
                        doc =
                        """Enables breaking simulation on the SystemC %s object.
                        A log message is printed and simulation is stopped every
                        time %s. %s"""
                        % (kind, kind_doc[kind],
                           """Setting the flag <tt>-i</tt> additionally stops
                           the simulation for internal processes.
                           """ if kind == 'process' else ""))

        cli.new_command("unbreak-sc-%s-all" % kind,
                        func_builder("disable_break", kind), [],
                        type = ['Breakpoints', 'SystemC'],
                        iface = "sc_simcontext",
                        short = "unbreak all SystemC %s objects" % kind,
                        see_also = ["<sc_simcontext>.break-sc-%s-all" % kind,
                                    "<sc_provider_controller>.unbreak-sc"],
                        doc =
                        "Disables breaking on all SystemC %s objects." % kind)

    # Track transactions
    # NOTE: iface differs. These are _not_ duplicated entries
    cli.new_command("track-transactions", enable_transaction_tracker, [],
                    type = ['Tracing', 'SystemC'],
                    iface = "sc_tlm_fw_transport",  # FW
                    short = "track transactions on a SystemC socket object",
                    see_also = ["<sc_tlm_fw_transport>.untrack-transactions",
                                "<sc_tlm_bw_transport>.untrack-transactions"],
                    doc = """
    Enables transaction tracking on a SystemC socket object. A log message is
    printed every time %s.""" % kind_doc["socket"])

    cli.new_command("track-transactions", enable_transaction_tracker, [],
                    type = ['Tracing', 'SystemC'],
                    iface = "sc_tlm_bw_transport",  # BW
                    short = "track transactions on a SystemC socket object",
                    see_also = ["<sc_tlm_bw_transport>.untrack-transactions",
                                "<sc_tlm_fw_transport>.untrack-transactions"],
                    doc_with = "<sc_tlm_fw_transport>.track-transactions")

    cli.new_command("untrack-transactions", disable_transaction_tracker, [],
                    type = ['Tracing', 'SystemC'],
                    iface = "sc_tlm_fw_transport",  # FW
                    short = "untrack transactions on a SystemC socket object",
                    see_also = ["<sc_tlm_fw_transport>.track-transactions",
                                "<sc_tlm_bw_transport>.track-transactions"],
                    doc = """
    Disables transaction tracking on a SystemC socket object.
    """)

    cli.new_command("untrack-transactions", disable_transaction_tracker, [],
                    type = ['Tracing', 'SystemC'],
                    iface = "sc_tlm_bw_transport",  # BW
                    short = "untrack transactions on a SystemC socket object",
                    see_also = ["<sc_tlm_bw_transport>.track-transactions",
                                "<sc_tlm_fw_transport>.track-transactions"],
                    doc_with = "<sc_tlm_fw_transport>.untrack-transactions")

    cli.new_command("track-transactions-all", track_transactions_all_cmd, [],
                    type = ['Tracing', 'SystemC'],
                    iface = "sc_simcontext",
                    short = "track transactions on all SystemC socket objects",
                    see_also = ["<sc_simcontext>.untrack-transactions-all",
                                "<sc_tlm_fw_transport>.track-transactions",
                                "<sc_tlm_bw_transport>.track-transactions"],
                    doc = """
    Enables transaction tracking on all SystemC socket objects.
    """)

    cli.new_command("untrack-transactions-all", untrack_transactions_all_cmd, [],
                    type = ['Tracing', 'SystemC'],
                    iface = "sc_simcontext",
                    short = "untrack transactions on all SystemC socket objects",
                    see_also = ["<sc_simcontext>.track-transactions-all",
                                "<sc_tlm_fw_transport>.untrack-transactions",
                                "<sc_tlm_bw_transport>.untrack-transactions"],
                    doc = """
    Disables transaction tracking on all SystemC socket objects.
    """)
