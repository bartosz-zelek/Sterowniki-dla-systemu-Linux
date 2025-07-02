# © 2010 Intel Corporation

from simics import SIM_create_object

# Common configurations
def create_device():
    return SIM_create_object('empty_device_tlm2', 'device')
