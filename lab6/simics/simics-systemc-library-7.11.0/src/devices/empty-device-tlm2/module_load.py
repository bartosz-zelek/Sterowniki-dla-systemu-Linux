# © 2010 Intel Corporation

import cli

device_class_name = "empty_device_tlm2"

#
# ------------------------ info -----------------------
#

def get_info(obj):
    return []

cli.new_info_command(device_class_name, get_info)

#
# ------------------------ status -----------------------
#

def get_status(obj):
    return []

cli.new_status_command(device_class_name, get_status)
