# © 2024 Intel Corporation
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
import re
import table
import commands
import textwrap

def list_pcie_hierarchies_cmd(namespace, skip_info, include_bus, indicate_legacy_pcie):
    dps = None
    legacy_dps = None

    def attr_object(attr):
        if attr is None:
            return None
        elif isinstance(attr, simics.conf_object_t):
            return attr
        elif len(attr) == 2 and isinstance(attr[0], simics.conf_object_t) and isinstance(attr[1], str):
            p = getattr(attr[0], "port", None)
            if p is None:
                raise Exception("Unknown attribute object", attr)
            p = getattr(p, attr[1], None)
            if p is None:
                raise Exception("Unknown attribute object", attr)

            return simics.SIM_get_object(f"{simics.SIM_object_name(attr[0])}.port.{attr[1]}")
        else:
            raise Exception("Unknown attribute object", attr)

    def find_downstream_ports():
        d = cli.quiet_run_command('list-objects class = "pcie-downstream-port"')[0]
        d += cli.quiet_run_command('list-objects class = "pcie-downstream-port-legacy"')[0]
        # cxl ports are not released
        try:
            simics.SIM_get_class("cxl-downstream-port")
            simics.SIM_get_class("cxl-hdm-port")
            d += cli.quiet_run_command('list-objects class = "cxl-downstream-port"')[0]
            d += cli.quiet_run_command('list-objects class = "cxl-hdm-port"')[0]
        except simics.SimExc_General:
            pass
        return d
    def find_legacy_pcie_buses():
        # This is to prevent loading the pcie-bus module by the next command
        # Since the pcie-bus modules is deprecated it will print out a warning
        # that this command really should not generate.
        v = cli.quiet_run_command('list-classes -l module = pcie-bus')[0]
        if len(v) == 0:
            return []
        d = cli.quiet_run_command('list-objects class = "pcie-bus"')[0]
        return d

    def find_function_parent_dev(function):
        p = function
        while p is not None:
            try:
                if simics.SIM_get_interface(p, "pcie_device") is not None:
                    return p, False
            except simics.SimExc_Lookup:
                try:
                    if simics.SIM_get_interface(p, "pci_device") is not None:
                        return p, True
                except simics.SimExc_Lookup:
                    pass
            p = simics.SIM_object_parent(p)
        return None, None

    def get_topology_next_depth(dev, depth, downstream_list):
        nonlocal dps
        for d in (dps + legacy_dps):
            d_obj = simics.SIM_get_object(d)
            # Handle transparent target
            if dev.classname == "pcie-downstream-port.downstream":
                t_dobj = simics.SIM_object_parent(simics.SIM_object_parent(dev))
                if d_obj == t_dobj:
                    downstream_list.append(get_topology(d_obj, depth + 1))
            elif d_obj.upstream_target == dev:
                downstream_list.append(get_topology(d_obj, depth + 1))

    def dev_attr_to_dev(dev_attr):
        if isinstance(dev_attr, simics.conf_object_t):
            return dev_attr
        if len(dev_attr) == 2:
            return dev_attr[1]
        return dev_attr[2]


    def get_topology(dp, depth):
        max_depth = 15
        if depth > max_depth:
            raise cli.CliError(f"Aborting command, PCIe depth exceeded {max_depth}")


        dev_info = []
        pcie_lvl = (simics.SIM_object_name(dp), dev_info)
        devices = list(getattr(dp, 'devices', [])) + list(getattr(dp, 'pci_devices', []))
        for dev_attr in devices:
            dev = dev_attr_to_dev(dev_attr)
            name = simics.SIM_object_name(dev)
            downstream = []
            dev_info.append((name, downstream))
            if dev == dp.upstream_target:
                continue
            get_topology_next_depth(dev, depth,  downstream)

        if dp.classname != "pcie-bus":
            for df, f_dev in dp.functions:
                dev, _ = find_function_parent_dev(f_dev)
                name = simics.SIM_object_name(dev)
                downstream = []

                found = False
                for (n, _) in dev_info:
                    if name == n:
                        found = True
                if not found:
                    dev_info.append((name, downstream))
                    get_topology_next_depth(dev, depth,  downstream)

        return pcie_lvl


    def get_register_value(bank, offset, size):
        cmd = f'get-device-offset bank = {simics.SIM_object_name(bank)} offset = {offset} size = {size}'
        v = cli.quiet_run_command(cmd)[0]
        return v

    def find_capability(bank, id):
        cap_ptr = get_register_value(bank, 0x34, 1)
        while cap_ptr != 0:
            val = get_register_value(bank, cap_ptr, 4)
            cur_id = val & 0xff
            next_cap_ptr = (val >> 8) & 0xff
            if cur_id == id:
                return cap_ptr

            cap_ptr = next_cap_ptr
        return None

    def dp_type_to_str(dp_type):
        if dp_type is None:
            return None
        elif dp_type == 0:
            return "Endpoint"
        elif dp_type == 0b1:
            return "Legacy Endpoint"
        elif dp_type == 0b1001:
            return "RCiEP"
        elif dp_type == 0b1010:
            return "Root complex Event collector"
        elif dp_type == 0b0100:
            return "Root port of Root Complex"
        elif dp_type == 0b0101:
            return "Switch Upstream port"
        elif dp_type == 0b0110:
            return "Switch Downstream port"
        elif dp_type == 0b0111:
            return "PCIe to PCI/PCI-X Bridge"
        elif dp_type == 0b1000:
            return "PCI/PCI-X to PCIe Bridge"

    def get_dp_type(bank):
        cap_ptr = find_capability(bank, 0x10)
        if cap_ptr is not None:
            v = get_register_value(bank, cap_ptr + 0x2, 1)
            return v >> 4
        return None

    def get_dp_type_str(bank):
        t = get_dp_type(bank)
        if t is not None: # PCIe
            return dp_type_to_str(t)
        else:
            header_type = get_register_value(bank, 0xE, 1)
            layout = header_type & 0x7f
            if layout == 0:
                return "PCI Endpoint"
            elif layout == 1:
                return "PCI to PCI Bridge"

        return None



    def topology_to_table(table_data, info, depth):
        def shift_str(s, depth):
            return f"{s:>{len(s) + depth * 4}}"
        dp_name, dev_info = info
        dp_obj = simics.SIM_get_object(dp_name)
        sec_bus_num = dp_obj.sec_bus_num if dp_obj.classname != "pcie-bus" else dp_obj.bus_number
        disabled_funcs = []
        if dp_obj.classname != "pcie-bus":
            functions = dp_obj.functions
            disabled_funcs = dp_obj.disabled
        else:
            functions = []
            for m in dp_obj.conf_space.map:
                # When mapped bus number is same as pcie-bus.bus_number
                # it means the mapping is a function.
                # Otherwise it is a mapping to a second downstream pcie-bus
                if (m[0] >> 20) != dp_obj.bus_number:
                    continue
                if isinstance(m[1], list):
                    print("Downstream port", dp_name, "Skipping function",
                          m[1], "Port object not permitted")
                    continue
                functions.append([(m[0] >> 12) & 0xff, m[1]])
            # Print disabled devices as well with no functions mapped
            for d, f, o, en in dp_obj.pci_devices:
                if en == 0:
                    if isinstance(o, list):
                        print("Downstream port", dp_name, "Skipping device",
                              o, "Port object not permitted")
                        continue
                    item_info = [shift_str(f"{sec_bus_num:02x}:{d:02x}.{f}", depth),
                             dp_name,
                             simics.SIM_object_name(o),
                             "Disabled",
                             textwrap.shorten(o.class_desc, width=40, placeholder="...") if o.class_desc is not None else "",
                             "",
                             "Legacy",]
                    table_data += [item_info]

        for f_num, func in sorted(functions, key=lambda x: x[0]):
            pcie_dev, legacy = find_function_parent_dev(func)
            if pcie_dev is None:
                print("Downstream port", dp_name,
                      "Skipping function",
                      simics.SIM_object_name(func),
                      "cannot find its pcie device")
                continue
            if legacy:
                try:
                    try:
                        simics.SIM_get_interface(func, "register_view")
                        bank = func
                    except simics.SimExc_Lookup:
                        bank = pcie_dev.bank.pci_config
                except AttributeError:
                    bank = None
            else:
                bank = func
            dev_name = simics.SIM_object_name(pcie_dev)

            dev_id = None
            id_bit_shift = 16 if dp_obj.classname != "pcie-bus" else 12
            cfg_space_map = (
                dp_obj.cfg_space.map
                if dp_obj.classname != "pcie-bus"
                else dp_obj.conf_space.map
            )
            for m in cfg_space_map:
                if m[1] == func:
                    dev_id = (m[0] >> id_bit_shift)
                    if dp_obj.classname != "pcie-bus":
                        dev_id += (sec_bus_num << 8)
                    break
            disabled = False
            if dev_id is None:
                if f_num in disabled_funcs:
                    disabled = True
                    dev_id = (sec_bus_num << 8) | f_num
                else:
                    print(f"Cannot find device id for {func}. Will show incorrect BDF.")
                    dev_id = 0

            bdf = f"{dev_id >> 8:02x}:{(dev_id >> 3) & 0b11111:02x}.{dev_id & 0b111}"

            dp_type = get_dp_type_str(bank) if bank is not None else None
            bdf_str = shift_str(bdf, depth)
            bdf_str += " (disabled)" if disabled else ""

            item_info = [bdf_str,
                    dp_name,
                    dev_name,
                    simics.SIM_object_name(func),
                    textwrap.shorten(pcie_dev.class_desc, width=40, placeholder="...") if pcie_dev.class_desc is not None else "",
                    dp_type if dp_type is not None else "",
                    "Legacy" if legacy else "New",]
            table_data += [item_info]
            for dn, ds_info in dev_info:
                if dn == dev_name:
                    for d in ds_info:
                        topology_to_table(table_data, d, depth + 1)
        # Handle transparent downstream ports separately
        # Transparent downstream port are added under devices
        # but does not show up in the functions attribute.
        if dp_obj.classname != "pcie-bus":
            for dev in dp_obj.devices:
                if isinstance(dev, list):
                    continue
                # Transparent downstream port
                dev_name = simics.SIM_object_name(dev)
                for dn, ds_info in dev_info:
                    if dn == dev_name:
                        for d in ds_info:
                            topology_to_table(table_data, d, depth)

    def find_root_ports(namespace):
        nonlocal dps
        nonlocal legacy_dps
        dps = find_downstream_ports()
        legacy_dps = find_legacy_pcie_buses()

        upstream_targets = []
        downstream_targets = []
        for dp in dps:
            dp_obj = simics.SIM_get_object(dp)

            if dp_obj.upstream_target is not None:
                upstream_targets.append((dp_obj.upstream_target, dp_obj))

            devs = dp_obj.devices
            for d in devs:
                if not isinstance(d, list):
                    downstream_targets.append(d)
                else:
                    if d[-1] == dp_obj.upstream_target:
                        continue
                    downstream_targets.append(d[-1])
            for df, func in dp_obj.functions:
                d, legacy = find_function_parent_dev(func)
                if d is not None and d not in devs:
                    if d == dp_obj.upstream_target:
                        continue
                    downstream_targets.append(d)

        for dp in legacy_dps:
            dp_obj = simics.SIM_get_object(dp)
            if dp_obj.upstream_target is not None:
                upstream_targets.append((attr_object(dp_obj.upstream_target), dp_obj))
            # Legacy pcie forwards the transaction downstream if upstream target not present
            # Has to accept that scenario as a valid subsystem
            elif dp_obj.memory_space is not None:
                upstream_targets.append((attr_object(dp_obj.memory_space), dp_obj))
            devs = dp_obj.pci_devices
            for d in devs:
                if attr_object(d[2]) == attr_object(dp_obj.upstream_target):
                    continue
                downstream_targets.append(d[2])

        roots = []
        for upt, dp in upstream_targets:
            functions = len(dp.functions) if dp.classname != "pcie-bus" else len(dp.pci_devices)
            if upt not in downstream_targets and functions > 0:
                if (namespace is None or namespace == ""
                    or simics.SIM_object_name(upt).startswith(f"{namespace}.")
                    or simics.SIM_object_name(upt) == namespace):
                    roots.append((upt, dp))

        return roots


    roots = find_root_ports(namespace)

    msg = ""
    tables = []
    header = ["BDF", "Bus", "Device", "Function", "Info", "Device/Port Type", "PCIe library"]
    if not indicate_legacy_pcie:
        header.pop(6)
    if skip_info:
        header.pop(4)
    if not include_bus:
        header.pop(1)
    for i, (upt, dp) in enumerate(roots):
        info = get_topology(dp, 1)

        if dp.classname == "pcie-bus":
            bridge = simics.SIM_object_name(upt)
            if dp.bridge is not None and dp.upstream_target != attr_object(dp.bridge):
                bridge += ", " + simics.SIM_object_name(attr_object(dp.bridge))
        else:
            bridge = simics.SIM_object_name(upt)
        props = [(table.Table_Key_Columns,
                  [[(table.Column_Key_Name, n)] for n in header]),
                    (table.Table_Key_Extra_Headers, [
                    (table.Extra_Header_Key_Row, [ #  row 1
                        [(table.Extra_Header_Key_Name, f"PCIe Subsystem #{i}")],
                    ]),
                    (table.Extra_Header_Key_Row, [ #  row 1
                        [(table.Extra_Header_Key_Name, f"Host CPU/Memory Bridge: {bridge}")],
                    ]),
                  ])
                ]
        table_data = []
        depth = 0
        topology_to_table(table_data, info, depth)
        if not indicate_legacy_pcie:
            for t in table_data:
                t.pop(6)
        if skip_info:
            for t in table_data:
                t.pop(4)
        if not include_bus:
            for t in table_data:
                t.pop(1)
        t = table.Table(props, table_data)
        msg += t.to_string(rows_printed=0, no_row_column=True)
        msg += "\n"
        tables.append(table_data)

    return cli.command_verbose_return(msg, tables)

cli.new_command("list-pcie-hierarchies", list_pcie_hierarchies_cmd,
                args = [cli.arg(cli.str_t, "namespace", "?", "",
                                expander = commands.cn_expander),
                        cli.arg(cli.flag_t, "-skip-info", "?", False),
                        cli.arg(cli.flag_t, "-include-bus", "?", False),
                        cli.arg(cli.flag_t, "-indicate-legacy-pcie", "?", False),],
                type=["Inspection"],
                short="list PCIe hierarchies in the system",
                doc="""
Tries to find all PCIe subsystems and traverses the
hierarchies from root port down to all endpoints and prints out
the hierarchy in a table format.

The command supports the new Simics PCIe library and the legacy Simics PCIe library.
The command also tries to traverse hybrid PCIe systems where a root port is implemented
with the new Simics PCIe library but endpoints and switches underneath could be based on the
legacy Simics PCIe library.

The new Simics PCIe library utilizes the
classes <class>pcie-downstream-port</class> and
<class>pcie-downstream-port-legacy</class> to build up the PCIe topology.
The legacy Simics PCIe library utilizes the
classes <class>pcie-bus</class> and <class>pci-bus</class> to build up the PCIe topology.
The command finds all instantiated objects of these classes, and tries to construct
the hierarchy based on how these class instances are connected to one another.

The <tt>-include-bus</tt> flag adds an addition column in the table,
listing which bus object the <tt>device</tt> and <tt>function</tt> are underneath.

The <tt>-skip-info</tt> flag can be used to hide the <tt>info</tt>
column in the table that contains the class description of the device.

The <tt>-indicate-legacy-pcie</tt> flag can be used to show if the
PCIe device is modeled based on the new or legacy Simics PCIe interfaces.

The <arg>namespace</arg> argument can be used to only traverse PCIe hierarchies
where the host bridge is part of the namespace.
""")
