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


import simics
import cli
import table
from . import profiling_commands

profiling_commands.add_commands()

# Other commands
class DmiSupport:
    def __init__(self, *new_command_args):
        (self.obj,) = new_command_args
        assert isinstance(self.obj, simics.conf_object_t)
    def what(self):
        return "DMI support"
    def is_enabled(self):
        return self.obj.iface.sc_initiator_gasket.is_dmi_enabled()
    def set_enabled(self, enable):
        self.obj.iface.sc_initiator_gasket.set_dmi(enable)

cli.new_command("enable-dmi", cli.enable_cmd(DmiSupport), [],
                iface = "sc_initiator_gasket",
                type = [ 'SystemC' ],
                short = "enable use of DMI in gasket",
                doc = """
Enables use of DMI in the gasket. DMI can be disabled with the
<cmd class="sc_initiator_gasket">disable-dmi</cmd> command.""")

cli.new_command("disable-dmi", cli.disable_cmd(DmiSupport), [],
                iface = "sc_initiator_gasket",
                type = [ 'SystemC' ],
                short = "disable use of DMI in gasket",
                doc = """
Disables use of DMI in the gasket. DMI can be enabled with the
<cmd class="sc_initiator_gasket">enable-dmi</cmd> command.
""")


def print_dmi_table_initiator_gasket(obj):
    obj.iface.sc_initiator_gasket.print_dmi_table()

cli.new_command("print-dmi-table", print_dmi_table_initiator_gasket, [],
                iface = "sc_initiator_gasket",
                type = [ 'Debugging', 'SystemC' ],
                short = "print DMI Table",
                doc = """
Print the content of the DMI table used by the gasket.
""")

def print_connections(obj, proxies):
    if not proxies:
        print("No connections")
    else:
        print("Connections to proxies:")
        for c in proxies:
            if c is not obj:
                print(c.name)

def print_sc_port(obj):
    proxies = obj.iface.sc_port.port_to_proxies()
    print_connections(obj, proxies)


def print_sc_export(obj):
    proxy = obj.iface.sc_export.export_to_proxy()
    if proxy:
        print_connections(obj, [proxy])
    else:
        print_connections(obj, None)


cli.new_command("connections", print_sc_port,
                iface = "sc_port",
                type = [ 'Debugging', 'SystemC' ],
                short = "print connections to other SystemC objects",
                doc = """
Print the connections originating from this SystemC port object to other
SystemC objects.
""")

cli.new_command("connections", print_sc_export,
                iface = "sc_export",
                type = [ 'Debugging', 'SystemC' ],
                short = "print connections to other SystemC objects",
                doc = """
Print the connections originating from this SystemC export object to other
SystemC objects.
""")

# version command, show version of SC kernel and SC library
def version_cmd(obj):
    versions = obj.iface.sc_version.versions()
    compile_kernel = versions['library_kernel']
    compile_library = versions['library']
    runtime_kernel = versions['kernel']

    compile_str = "Compiled with SystemC Kernel: '%s'" % compile_kernel
    if 'isctlm' in versions:
        compile_str += ", ISCTLM: '%s'" % versions['isctlm']

    compile_str += " and SystemC Library (build-id): '%s'" % compile_library

    runtime_str = "Running with SystemC Kernel : '%s'" % runtime_kernel

    print(compile_str)
    print(runtime_str)

cli.new_command("version",
                version_cmd,
                type = ['Help', 'SystemC'],
                iface = "sc_version",
                short = "print SystemC kernel and SystemC Library version",
                doc = """
Print the version of the SystemC Kernel this module was compiled with,
the version of the SystemC Library this module was compiled with and
the version of the SystemC Kernel this module is currently running with.
""")


def print_table(title, headers, rows):
    if not rows:
        return

    props = [ (table.Table_Key_Name, title),
              (table.Table_Key_Columns, [
                 [ (table.Column_Key_Name, headers[0]) ],
                 [ (table.Column_Key_Name, headers[1]) ],
                 [ (table.Column_Key_Name, headers[2]) ],
                 [ (table.Column_Key_Name, headers[3]) ],
             ])]

    t = table.Table(props, rows)
    print(title)
    print(t.to_string(rows_printed=0, no_row_column=True))


def gasket_cmd(obj):
    print_table('simics2tlm', ['OBJECT', 'INTERFACE', 'GASKET', 'SOCKET'],
                obj.iface.sc_gasket_info.simics2tlm())
    print_table('tlm2simics', ['SOCKET', 'GASKET', 'INTERFACE', 'OBJECT'],
                obj.iface.sc_gasket_info.tlm2simics())
    print_table('simics2systemc', ['OBJECT', 'INTERFACE', 'GASKET', 'SIGNAL'],
                obj.iface.sc_gasket_info.simics2systemc())
    print_table('systemc2simics', ['SIGNAL', 'GASKET', 'INTERFACE', 'OBJECT'],
                obj.iface.sc_gasket_info.systemc2simics())

cli.new_command("gasket-info",
                gasket_cmd,
                type = ['Help', 'SystemC'],
                iface = "sc_gasket_info",
                short = "print Simics to SystemC connection information",
                doc = """
Print the connections between Simics interfaces and SystemC gaskets
""")
