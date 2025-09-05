# © 2015 Intel Corporation
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
from cli import run_command
import stest
import re

def create_configuration(classname, log_file):
    run_command('log-setup -overwrite logfile = %s' % log_file)
    return simics.SIM_create_object(classname, 'dut', log_level=2)

def remove_trailing_file_info(string):
    return re.sub(r' in \S+:\d+$', '', string)

def compare_logs(expected, actual):
    stest.expect_equal(len(actual), len(expected))  # zip will truncate
    for (lineno, (exp_line, act_line)) in enumerate(zip(expected, actual)):
        stest.expect_equal(remove_trailing_file_info(act_line),
                           remove_trailing_file_info(exp_line),
                           'Logs did not match on line %d' % (lineno + 1))

def run_test(classname):
    log_file = stest.log_file(classname + '.log')
    create_configuration(classname, log_file)
    # Arbitrary large number (1 seconds)
    run_command('c 1 s')
    compare_logs(open('master.log').readlines(),
                 open(log_file).readlines())
