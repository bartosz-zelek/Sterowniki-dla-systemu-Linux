# © 2018 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import common

from cli import run_command
from stest import expect_equal, expect_false, expect_true

def test(_class):
    (_, se_banks, _) = common.create_test_device(_class, 'lunarstorm_se')
    (_, uk_banks, _) = common.create_test_device(_class, 'lunarstorm_uk')
    (_, sp_banks, _) = common.create_test_device(_class, 'stajlplejs')

    tool_name = common.create_tool(se_banks['bank0'])
    common.add_instrumentation(tool_name, uk_banks['bank0'])
    common.add_instrumentation(tool_name, sp_banks['bank0'])

    def coverage(regexp = '.*'):
        return { device : coverage for (device, _, _, coverage)
                 in run_command('%s.coverage device-regexp="%s"' % (tool_name,
                                                                    regexp)) }

    expect_equal(len(coverage('helgon')), 0)

    expect_true('lunarstorm_se' in coverage())
    expect_true('lunarstorm_uk' in coverage())
    expect_true('stajlplejs' in coverage())

    expect_true('lunarstorm_se' in coverage('lunarstorm_.*'))
    expect_true('lunarstorm_uk' in coverage('lunarstorm_.*'))
    expect_false('stajlplejs' in coverage('lunarstorm_.*'))

    expect_false('lunarstorm_se' in coverage('stajlplejs'))
    expect_false('lunarstorm_uk' in coverage('stajlplejs'))
    expect_true('stajlplejs' in coverage('stajlplejs'))
