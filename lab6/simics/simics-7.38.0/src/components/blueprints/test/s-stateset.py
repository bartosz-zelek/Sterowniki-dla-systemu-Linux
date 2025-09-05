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


from blueprints import expand, Default, State, Namespace, Builder
import stest

class Test(State):
    n_test = 0

def subbp(comp, name, iface: Test):
    iface.n_test = 10

def my_bp(bp: Builder, name: Namespace):
    iface = bp.read_state(name, Test, private=True)

    # Set a default value which is used if 'n_test' is not set
    iface.n_test = Default(5)

    # Populate blueprint, using the state variable
    for i in range(iface.n_test):
        bp.obj(name.child[i], "testobj", some_attr=i)

    # Add a sub-blueprint which will set the state variable
    bp.expand(name, "kappa", subbp, iface=iface)

c = expand("", my_bp)
stest.expect_equal(len(c._obj), 10)
