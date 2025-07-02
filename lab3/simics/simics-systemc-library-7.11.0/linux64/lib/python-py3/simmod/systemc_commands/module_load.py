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


import cli
from simics import SIM_object_iterator_for_interface, SIM_shallow_object_iterator
from . import sc_process

# --- info command on interfaces
info_map = {}

def info_cmd(obj):
    title = "Information about %s [class %s]" % (obj.name, obj.classname)
    print(title)
    print("=" * len(title))
    for k, v in list(info_map.items()):
        iface = getattr(obj.iface, k, None)
        if iface:
            cli.print_info(v(obj, iface))

def new_info_command(iface, get_info):
    info_map[iface] = get_info
    cli.new_command("info",
                    info_cmd,
                    type = ['SystemC'],
                    iface = iface,
                    short = "print information about the device",
                    doc = ("Print detailed information about the configuration"
                           " of the device."))

def get_sockets(obj):
    # Initiator and Target sockets
    return [o for o in SIM_shallow_object_iterator(obj)
            if 'socket' in o.iface.sc_object.sc_kind()]

def get_signals(obj):
    # We don't handle sc_in/sc_out/sc_inout here, as that end is the sc_port
    # covered by sc_port_info and must be connected to an sc_signal. This extra
    # information is primarily intended for our own gaskets, which only have
    # sockets and sc_signal in them.
    return [o for o in SIM_shallow_object_iterator(obj)
            if 'signal' in o.iface.sc_object.sc_kind()]

def sc_object_info(obj, iface):
    info = [(None, [("Name", iface.sc_name()),
                    ("Kind", iface.sc_kind())])]

    # If object is a module, it might contain sockets.
    if iface.sc_kind() == 'sc_module':
        info += [(None, [("Sockets", get_sockets(obj))])]
        info += [(None, [("Signals", get_signals(obj))])]

    return info

new_info_command("sc_object", sc_object_info)

def sc_process_info(obj, iface):
    return [(None, [("Initialize", iface.initialize())])]

new_info_command("sc_process", sc_process_info)

def sc_port_info(obj, iface):
    if obj.iface.sc_object.sc_kind() in ["sc_in", "sc_out", "sc_inout"]:
        target = "Connected to"
    else:
        target = "Target"

    return [(None, [(target, iface.port_to_proxies())])]

new_info_command("sc_port", sc_port_info)

# Turns out, sockets and signals work almost the same w.r.t. sc_port
def sc_export_info(obj, iface):
    info = []
    # Find the initiator of this target socket
    if obj.iface.sc_object.sc_kind() in ["tlm_target_socket",
                                         "sc_signal"]:
        initiator = [
            port for port in SIM_object_iterator_for_interface(["sc_port"])
            if obj in port.iface.sc_port.port_to_proxies()]

        if initiator and obj.iface.sc_object.sc_kind() == "tlm_target_socket":
            info += [(None, [("Initiator", initiator)])]
        elif initiator and obj.iface.sc_object.sc_kind() == "sc_signal":
            info += [(None, [("Connected to", initiator)])]

    return info

new_info_command("sc_export", sc_export_info)
new_info_command("sc_signal_read", sc_export_info)

def sc_initiator_gasket_info(obj, iface):
    return []

new_info_command("sc_initiator_gasket", sc_initiator_gasket_info)

# --- status command on interfaces

status_map = {}

def status_cmd(obj):
    title = "Status of %s [class %s]" % (obj.name, obj.classname)
    print(title)
    print("=" * len(title))
    found = False
    for k in list(status_map.keys()):
        iface = getattr(obj.iface, k, None)
        if iface:
            status = status_map[k](obj, iface)
            if status:
                found = True
                cli.print_info(status)
    if not found:
        print("No status available")

def new_status_command(iface, get_status):
    status_map[iface] = get_status
    cli.new_command("status",
                    status_cmd,
                    type = ['SystemC'],
                    iface = iface,
                    short = "print status of the device",
                    doc = ("Print detailed information about the current status"
                           " of the device."))

def sc_object_status(obj, iface):
    # default status is no status
    return None

new_status_command("sc_object", sc_object_status)

def sc_initiator_gasket_status(obj, iface):
    return [(None, [("DMI enabled", iface.is_dmi_enabled())])]

new_status_command("sc_initiator_gasket", sc_initiator_gasket_status)

new_status_command("sc_process", sc_process.sc_process_status)
