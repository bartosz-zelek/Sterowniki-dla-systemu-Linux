# © 2021 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


from blueprints import instantiate, Builder, State, Namespace
import stest

class Result: pass

class Local(State):
    kappa = 0
    gamma = 1

# Test that evaluation is recursive
def test1():
    result = Result()

    def my_bp(bp: Builder, name: Namespace):
        local = bp.read_state(name, Local, private=True)
        local.kappa = [20, local.gamma]
        local.gamma = 30
        result.kappa = local.kappa
        result.gamma = local.gamma

    instantiate("", my_bp)
    stest.expect_equal(result.gamma, 30)
    stest.expect_equal(result.kappa, [20, 30])

test1()
