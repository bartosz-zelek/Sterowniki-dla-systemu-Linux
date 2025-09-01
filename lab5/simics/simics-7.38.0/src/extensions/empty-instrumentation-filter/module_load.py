# © 2017 Intel Corporation

import cli
import instrumentation
import simics

def get_info(obj):
    return []

def get_status(obj):
    return []

cli.new_info_command("empty_instrumentation_filter", get_info)
cli.new_status_command("empty_instrumentation_filter", get_status)

# USER-TODO: Add filter specific commands which configures the
# filter.

def delete_cmd(obj):
    instrumentation.delete_filter(obj)
    simics.SIM_delete_object(obj)

cli.new_command(
    "delete", delete_cmd,
    args = [],
    cls = "empty_instrumentation_filter",
    type = ["Instrumentation"],
    short = "remove the filter",
    # USER-TODO: Add correct see_also references
    see_also = ["new-empty-instrumentation-filter",
                ],
    doc = """
    Removes the filter.""")
