# © 2015 Intel Corporation

import cli

device_class_name = "empty_device_sc_only"

def get_device_info(obj):
    return []

def get_device_status(obj):
    return []

cli.new_info_command(device_class_name, get_device_info)
cli.new_status_command(device_class_name, get_device_status)
