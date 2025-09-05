# © 2021 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import instrumentation
import cli
import table
import simics

def quote(txt):
    return f"\'{txt}\'"

def tool_status_cmd(obj):
    cons = len(obj.connections)
    iclasses = [ic[0] for ic in obj.instruction_classes]
    iclasses_str = ", ".join(iclasses)

    return [(None,
             [("Connections", f"{cons}"),
              ("Instructions classes", iclasses_str)])]

cli.new_status_command("sample_core_timing_tool", tool_status_cmd)

def list_instr_cls_cmd(obj):
    rows = []
    cols = [
        [(table.Column_Key_Name, "Instruction Pattern")],
        [(table.Column_Key_Name, "Extra Cycles")],
        [(table.Column_Key_Name, "Extra Activity")],
    ]

    props = [
        (table.Table_Key_Default_Sort_Column, "Instruction Pattern"),
        (table.Table_Key_Columns, cols)
    ]

    for cls in obj.instruction_classes:
        row = [cls[0], cls[1], cls[2]]
        rows.append(row)

    tbl = table.Table(props, rows)
    msg = tbl.to_string(rows_printed=0, no_row_column=True) if len(rows) else ""
    return cli.command_verbose_return(msg, rows)

def is_installed_iclass(obj, instruction):
    for ic in obj.instruction_classes:
        if ic[0] == instruction:
            return True
    return False

def add_instr_cls_cmd(obj, instruction, extra_cycles, extra_activity):
    if is_installed_iclass(obj, instruction):
        return cli.command_return("'%s' already added, not updated!" % instruction, 1)
    else:
        obj.instruction_classes.append([instruction, extra_cycles, extra_activity])
        return cli.command_return("'%s' added" % instruction, 0)

def remove_instr_cls_cmd(obj, instruction):
    if is_installed_iclass(obj, instruction):
        new_installed_iclasses = [ic for ic in obj.instruction_classes if ic[0] != instruction]
        obj.instruction_classes = new_installed_iclasses
        return cli.command_return("'%s' removed" % instruction, 0)
    else:
        return cli.command_return("'%s' not found" % instruction, 1)

def set_background_activity_cmd(obj, activity):
    obj.background_activity = activity
    return cli.command_return("Background activity per cycle updated to %g" % activity, 0)

def set_base_activity_cmd(obj, activity):
    obj.base_activity = activity
    return cli.command_return("Base activity per instruction updated to %g" % activity, 0)

def set_base_cpi_cmd(obj, cycles):
    msg = []
    for conn in obj.connections:
        msg.append(conn.cpu.cli_cmds.set_step_rate(ipc= (1.5 / (cycles))))
    obj.base_cycles = cycles
    return cli.command_return("Base cycles per instruction updated to %g" % cycles)

def set_cycles_per_read_cmd(obj, cycles):
    obj.read_cycles = cycles
    return cli.command_return("Extra cycles per read operation updated to %g" % cycles, 0)

def set_cycles_per_write_cmd(obj, cycles):
    obj.write_cycles = cycles
    return cli.command_return("Extra cycles per write operation updated to %g" % cycles, 0)

def set_activity_per_read_cmd(obj, activity):
    obj.read_activity = activity
    return cli.command_return("Extra activity per read operation updated to %g" % activity, 0)

def set_activity_per_write_cmd(obj, activity):
    obj.write_activity = activity
    return cli.command_return("Extra activity per write operation updated to %g" % activity, 0)


cli.new_command("list-instruction-classes", list_instr_cls_cmd,
                [],
                cls = "sample_core_timing_tool",
                type = ["Instrumentation"],
                short = "list instruction classes",
                doc = ("""List all instruction classes together with CPI and CDyn adjustments."""))

cli.new_command("add-instruction-class", add_instr_cls_cmd,
                [cli.arg(cli.str_t, "instruction"),
                 cli.arg(cli.float_t, "extra_cycles"),
                 cli.arg(cli.float_t, "extra_activity", spec="?", default=0.0)],
                cls = "sample_core_timing_tool",
                type = ["Instrumentation"],
                short = "add instruction class with ipc and cdyn adjustments",
                doc = ("""Add an <arg>instruction</arg> pattern together with <arg>extra_cycles</arg>
 and <arg>extra_activity</arg>.
 Note, removing instruction classes after the performance model has been executed can lead to
 unexpected results."""))

def installed_instr_classes_expander(prefix, obj):
    candidates = [ic[0] for ic in obj.instruction_classes]
    print("Candidates: %s" % str(candidates))
    return cli.get_completions(prefix, candidates)

cli.new_command("remove-instruction-class", remove_instr_cls_cmd,
                [cli.arg(cli.str_t, "instruction", expander = installed_instr_classes_expander)],
                cls = "sample_core_timing_tool",
                type = ["Instrumentation"],
                short = "remove instruction class",
                doc = ("""Remove one <arg>instruction</arg> class from the
 performance model.
 Note, removing instruction classes after the performance model has been run can
 lead to unexpected results."""))

cli.new_command("set-background-activity-per-cycle", set_background_activity_cmd,
                [cli.arg(cli.float_t, "activity")],
                cls = "sample_core_timing_tool",
                type = ["Instrumentation"],
                short = "set background activity per cycle",
                doc = ("""Set background <arg>activity</arg> per cycle.
This activity will also be reported for idle periods but not for sleeping cycles."""))

cli.new_command("set-base-activity-per-instruction", set_base_activity_cmd,
                [cli.arg(cli.float_t, "activity")],
                cls = "sample_core_timing_tool",
                type = ["Instrumentation"],
                short = "set activity per instruction",
                doc = ("""Set <arg>activity</arg> per instruction.
This activity will be added for all instructions."""))

cli.new_command("set-base-cycles-per-instruction", set_base_cpi_cmd,
                [cli.arg(cli.float_t, "cycles")],
                cls = "sample_core_timing_tool",
                type = ["Instrumentation"],
                short = "set base cycles per instruction",
                doc = ("""Set base <arg>cycles</arg>, cycles or fraction of cycles,
 per instruction."""))

cli.new_command("set-cycles-per-read", set_cycles_per_read_cmd,
                [cli.arg(cli.float_t, "cycles")],
                cls = "sample_core_timing_tool",
                type = ["Instrumentation"],
                short = "set extra cycles per read operation",
                doc = ("""Set extra <arg>cycles</arg> per read operation."""))

cli.new_command("set-activity-per-read", set_activity_per_read_cmd,
                [cli.arg(cli.float_t, "activity")],
                cls = "sample_core_timing_tool",
                type = ["Instrumentation"],
                short = "set extra activity per read operation",
                doc = ("""Set extra <arg>activity</arg> per read operation."""))

cli.new_command("set-cycles-per-write", set_cycles_per_write_cmd,
                [cli.arg(cli.float_t, "cycles")],
                cls = "sample_core_timing_tool",
                type = ["Instrumentation"],
                short = "set extra cycles per write operation",
                doc = ("""Set extra <arg>cycles</arg> per write operation."""))

cli.new_command("set-activity-per-write", set_activity_per_write_cmd,
                [cli.arg(cli.float_t, "activity")],
                cls = "sample_core_timing_tool",
                type = ["Instrumentation"],
                short = "set extra activity per write operation",
                doc = ("""Set extra <arg>activity</arg> per write operation."""))

def get_stats(obj):
    stats = []
    for c in obj.connections:
        one_stats = {}
        one_stats["cpu"] = c.cpu.name
        for s in c.static_events:
            one_stats[s[0]] = s[1]
        for d in c.dynamic_events:
            one_stats[quote(d[0])] = d[1]
        stats.append(one_stats)
    return stats

def list_metrics_cmd(obj, _totals):

    stats = get_stats(obj)
    rows = []
    cols = [[(table.Column_Key_Name, "Metric/instruction pattern")]]

    if not _totals:
        for s in stats:
            cols.append([(table.Column_Key_Name, s["cpu"]),
                         (table.Column_Key_Int_Radix, 10)])

        for k in stats[0].keys():
            if k != 'cpu':
                row = [ k ]
                for s in stats:
                    row.append(s[k])
                rows.append(row)
    else:
        cols.append([(table.Column_Key_Name, "totals"),
                     (table.Column_Key_Int_Radix, 10)])

        for k in stats[0].keys():
            if k != 'cpu':
                total = sum([s[k] for s in stats])
                rows.append([k, total])

    props = [
        (table.Table_Key_Columns, cols)
    ]

    tbl = table.Table(props, rows)
    msg = tbl.to_string(rows_printed=0, no_row_column=True) if len(rows) else ""
    return cli.command_verbose_return(msg, rows)


def list_performance_metrics_cmd(obj):

    def avg_key(key):
        return key + "_avg"

    rows = []
    cols = [[(table.Column_Key_Name, "Metric")]]

    rowd = {}

    for c in obj.connections:
        cols.append([(table.Column_Key_Name, c.cpu.name),
                     (table.Column_Key_Int_Radix, 10),
                     (table.Column_Key_Int_Grouping, True)])

        for k, v, a in c.telemetry_data:
            if k in rowd:
                rowd[k].append(v)
                rowd[avg_key(k)].append(a)
            else:
                rowd[k] = [v]
                rowd[avg_key(k)] = [a]

    for k, vs in rowd.items():
        row = [k]
        for v in vs:
            row.append(v)
        rows.append(row)

    props = [
        (table.Table_Key_Columns, cols)
    ]

    tbl = table.Table(props, rows)
    msg = tbl.to_string(rows_printed=0, no_row_column=True) if len(rows) else ""
    return cli.command_verbose_return(msg, rows)

cli.new_command("list-metrics", list_metrics_cmd,
                [cli.arg(cli.flag_t, "-totals", spec="?", default=False)],
                cls = "sample_core_timing_tool",
                type = ["Instrumentation"],
                short = "list metrics of the simple performance model",
                doc = ("""List metrics of the simple performance model.
The flag <tt>-totals</tt> controls if the data is presented as a combined total or
separately for each connected."""))

cli.new_command("list-performance-metrics", list_performance_metrics_cmd,
                cls = "sample_core_timing_tool",
                type = ["Instrumentation"],
                short = "list performance metrics from the model",
                doc = ("""List performance metrics from the model."""))

def connection_info_cmd(obj):
    return [(None,
             [("Core", obj.cpu.name)])]

def connection_status_cmd(obj):
    status = []
    for k, v, a in obj.telemetry_data:
        status.append((k, f"{v}"))
        status.append((f"Average {k}", f"{a}"))
    for s in obj.static_events:
        status.append((s[0], f"{s[1]}"))
    for d in obj.dynamic_events:
        status.append((quote(d[0]), f"{d[1]}"))

    return [(None, status)]

cli.new_info_command("sample_core_timing_connection", connection_info_cmd)
cli.new_status_command("sample_core_timing_connection", connection_status_cmd)
