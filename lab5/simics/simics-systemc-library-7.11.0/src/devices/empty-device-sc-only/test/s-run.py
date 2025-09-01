# © 2015 Intel Corporation

import simics
from config import create_test_device

devs = create_test_device()
simics.SIM_continue(100000000)
