# © 2018 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import pyobj
import simics
from instrumentation import get_filter_source, delete_filter
import cli

class systemc_filter(pyobj.ConfObject):
    """SystemC instrumentation filter, used together with instrumentation tools.
    Connections are enabled/disabled based on their connected object's kind.
    """

    _class_desc = "a SystemC instrumentation filter"

    def _initialize(self):
        super()._initialize()
        self._slaves = {}
        self._source_id = 0

    def _status(self):
        return [("Attributes", [("Kind", self.kind.val)])]

    def _pre_delete(self):
        for s_iface in self._slaves.values():
            s_iface.enable(self._source_id)
        super()._pre_delete()

    class kind(pyobj.SimpleAttribute(None, 's')):
        """The kind of SystemC object to pass through the filter.
        The valid values are: event, process, signal and socket."""

    class instrumentation_filter_master(pyobj.Interface):
        def set_source_id(self, source_id):
            self._up._source_id = source_id

        def add_slave(self, slave, provider_obj):
            if hasattr(provider_obj.iface, 'sc_object'):
                pkind = provider_obj.iface.sc_object.sc_kind()
            else:
                pkind = provider_obj.classname

            kinds = {
                "event" : ("ScEventObject", "ScEventObjectAllDynamic"),
                "process" : ("sc_thread_process", "sc_method_process",
                             "sc_method_process_all_dynamic"),
                "signal" : ("sc_signal",),
                "socket" : ("tlm_initiator_socket", "tlm_target_socket"),
            }
            enable = pkind in kinds[self._up.kind.val]

            slave_iface = slave.iface.instrumentation_filter_slave
            self._up._slaves[slave] = slave_iface
            if enable:
                slave_iface.enable(self._up._source_id)
            else:
                slave_iface.disable(self._up._source_id)
            return True

        def remove_slave(self, slave, provider_obj):
            self._up._slaves[slave].enable(self._up._source_id)
            del self._up._slaves[slave]

        def short_filter_config(self):
            return "Only enables connections for SystemC kind: %s" % self._up.kind.val

def register_sc_filter_commands():
    def new_sc_filter(kind, name):
        if not name:
            name = cli.get_available_object_name("sc_filter")

        try:
            filt = simics.SIM_create_object("systemc_filter", name, kind=kind)
        except simics.SimExc_General as msg:
            raise cli.CliError("Cannot create %s: %s" % (name, msg))

        source_id = get_filter_source(filt.name)
        filt.iface.instrumentation_filter_master.set_source_id(source_id)

        return cli.command_return(message = "Created SystemC filter %s" % name,
                                  value = filt)

    def kind_expander(string, obj):
        return cli.get_completions(string, all_kinds)

    all_kinds = ['signal', 'event', 'process', 'socket']
    cli.new_command("new-systemc-filter", new_sc_filter,
                    args = [
                        cli.arg(cli.str_t, "kind", expander = kind_expander),
                        cli.arg(cli.str_t, "name", "?"),
                    ],
                    type = ["SystemC", "Instrumentation"],
                    short = "filter on specified SystemC objects",
                    see_also = ["<systemc_filter>.delete", ],
                    doc = """
                    Creates a new filter with the name given by the
                    <arg>name</arg> argument and filter the instrumentation
                    connections based on the required <arg>kind</arg> argument.
                    The <arg>kind</arg> argument corresponds to a pre-defined set
                    of SystemC objects, currently: %s.
                    """ % ', '.join(all_kinds))

    def delete_cmd(obj):
        delete_filter(obj)
        simics.SIM_delete_object(obj)

    cli.new_command("delete", delete_cmd,
                    args = [],
                    cls = "systemc_filter",
                    type = ["SystemC", "Instrumentation"],
                    short = "remove the filter",
                    see_also = ["new-systemc-filter",
                    ],
                    doc = """
                    Removes the filter.
                    """)
