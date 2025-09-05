# © 2010 Intel Corporation

import cli

class_name = 'first_device'

#
# ------------------------ info -----------------------
#

def get_info(obj):
    # USER-TODO: Return something useful here
    return []

cli.new_info_command(class_name, get_info)

#
# ------------------------ status -----------------------
#

def get_status(obj):
    # Keep status simple and robust for tests; avoid non-existent attributes
    # You can extend this to show register values if desired.
    return [("Registers",
             [("arg1", obj.regs_arg1),
              ("arg2", obj.regs_arg2),
              ("operation", obj.regs_operation),
              ("result", obj.regs_result)])]

cli.new_status_command(class_name, get_status)
