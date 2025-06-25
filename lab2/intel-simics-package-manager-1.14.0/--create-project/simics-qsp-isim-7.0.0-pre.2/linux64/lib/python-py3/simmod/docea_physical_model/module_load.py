import cli
import table

from . import docea_physical_model

docea_physical_model.docea_physical_model.register()


def compute_average_metrics_cmd(obj):
    rows = []
    cols = [[(table.Column_Key_Name, "Metric, averaged on all Cores")],
            [(table.Column_Key_Name, "Value")],
            [(table.Column_Key_Name, "Unit")]]

    props = [
        (table.Table_Key_Default_Sort_Column, "Metric, averaged on all Cores"),
        (table.Table_Key_Columns, cols)
    ]

    rows.append(["Frequency", obj.avg_metrics["Frequency"] * 1e-9, "Ghz"])
    rows.append([
        "Energy per instruction",
        obj.avg_metrics["Energy_Per_Instruction"] * 1e9,
        "nanojoules/instruction"
    ])

    tbl = table.Table(props, rows)
    msg = tbl.to_string(rows_printed=0,
                        no_row_column=True) if len(rows) else ""
    return cli.command_verbose_return(msg, rows)


cli.new_command("compute-average-metrics",
                compute_average_metrics_cmd, [],
                cls="docea_physical_model",
                type=["Instrumentation"],
                short="print simulation average metrics",
                doc=("""
Allows the user to observe simulation metrics.
Metrics are averaged on all cores and from the beginning of the simulation."""
                     ))
