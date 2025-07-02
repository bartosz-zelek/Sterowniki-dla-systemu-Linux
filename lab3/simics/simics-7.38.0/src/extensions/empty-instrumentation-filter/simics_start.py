# © 2017 Intel Corporation

import simics
import instrumentation
import cli

def new_empty_instrumentation_filter(name):
    if not name:
        # USER-TODO: Select default name for the filter
        name = cli.get_available_object_name("myfilt")

    msg = "Created filter %s" % name
    try:
        filt = simics.SIM_create_object("empty_instrumentation_filter",
                                        name, [])
    except simics.SimExc_General as msg:
        raise cli.CliError("Cannot create %s: %s" % (name, msg))

    source_id = instrumentation.get_filter_source(filt.name)
    filt.iface.instrumentation_filter_master.set_source_id(source_id)

    return cli.command_return(message = msg, value = filt)

cli.new_command(
    "new-empty-instrumentation-filter",
    new_empty_instrumentation_filter,
    args = [
        cli.arg(cli.str_t, "name", "?"),
        # USER-TODO: add argument allowing the filter to be configured
        # directly when it is created
    ],
    type = ["Instrumentation"],
    short = "filter on certain USER-TODO: what?",
    see_also = ["<empty_instrumentation_filter>.delete"],
    doc = """
    Creates a new filter with the name given by the <arg>name</arg>
    argument.  USER-TODO: Write this.""")
