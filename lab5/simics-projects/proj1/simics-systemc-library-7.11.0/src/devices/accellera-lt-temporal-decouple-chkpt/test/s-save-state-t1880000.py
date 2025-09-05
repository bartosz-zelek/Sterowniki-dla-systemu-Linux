# © 2016 Intel Corporation
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
import stest

dut = SIM_create_object("lt_temporal_decouple_chkpt_example", 'dut', log_level=2)

SIM_continue(1880000)

simics.SIM_write_configuration_to_file(stest.scratch_file('accellera-lt-temporal-decouple-chkpt-state-t1880000'), 0)
