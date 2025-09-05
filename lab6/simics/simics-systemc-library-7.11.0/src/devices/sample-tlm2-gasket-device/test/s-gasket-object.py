# © 2017 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

import stest
import dev_util as du
import conf
# SIMICS-21543
conf.sim.deprecation_level = 0

from sample_gasket_comp import *

create_sample_gasket_comp()

reg1 = du.Register_LE(conf.component0.gasket32, 0, size = 4)
reg2 = du.Register_LE(conf.component0.gasket64, 4, size = 8)
reg3 = du.Register_LE(conf.component0.multi_gasket, 12, size = 4)
reg4 = du.Register_LE(conf.component0.multi_gasket64, 16, size = 8)

reg1.write(1)
stest.expect_equal(reg1.read(), 1)

reg2.write(2)
stest.expect_equal(reg2.read(), 2)

reg3.write(3)
stest.expect_equal(reg3.read(), 3)

reg4.write(4)
stest.expect_equal(reg4.read(), 4)

stest.expect_equal(conf.component0.signal_object.state, False)
conf.component0.signal_in.iface.signal.signal_raise()
stest.expect_equal(conf.component0.signal_object.state, True)
conf.component0.signal_in.iface.signal.signal_lower()
stest.expect_equal(conf.component0.signal_object.state, False)

stest.expect_equal(conf.component0.signal_object_2.state, True)
conf.component0.signal_in_2.iface.signal.signal_lower()
stest.expect_equal(conf.component0.signal_object_2.state, False)
conf.component0.signal_in_2.iface.signal.signal_raise()
stest.expect_equal(conf.component0.signal_object_2.state, True)
