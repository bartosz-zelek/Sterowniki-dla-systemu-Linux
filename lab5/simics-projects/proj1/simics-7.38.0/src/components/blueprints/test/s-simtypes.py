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


from blueprints import ConfObject, expand, State, Builder
from blueprints.simtypes import DefaultTarget, MemMap, Port
from blueprints.types import Namespace
import stest

name = Namespace("")

def test_conf_object():
    class Test(State):
        obj = ConfObject()
        obj2 = ConfObject()
    def test_bp(bp: Builder, name: Namespace):
        i = bp.read_state(name, Test, private=True)
        i.obj = ConfObject(name)
    x = expand("", test_bp)
    iface = x.read_state_data(name, Test, private=True)
    # Empty string is false
    stest.expect_false(iface.obj)
    stest.expect_false(iface.obj2)
    stest.expect_true(iface.obj is not None)
    stest.expect_true(iface.obj2 is not None)
    stest.expect_equal(iface.obj.obj, name)
    stest.expect_true(iface.obj2.obj is None)

    ns = Namespace("foo")
    x = expand(str(ns), test_bp)
    iface = x.read_state_data(ns, Test, private=True)
    # Non-empty string is true
    stest.expect_true(iface.obj)
    stest.expect_false(iface.obj2)
    stest.expect_true(iface.obj is not None)
    stest.expect_true(iface.obj2 is not None)
    stest.expect_equal(iface.obj.obj, ns)
    stest.expect_true(iface.obj2.obj is None)

def test_port():
    class Test(State):
        port = Port()
        port2 = Port()
        port3 = Port()
    def test_bp(bp: Builder, name: Namespace):
        i = bp.read_state(name, Test, private=True)
        i.port = Port(name.kappa, "hello")
        i.port2 = Port(name.gamma)

    x = expand("", test_bp)
    iface = x.read_state_data(name, Test, private=True)
    stest.expect_equal(iface.port, Port(name.kappa, "hello"))
    stest.expect_equal(iface.port2, Port(name.gamma))
    stest.expect_false(iface.port3)
    stest.expect_true(iface.port2)
    stest.expect_equal(iface.port, (name.kappa, "hello"))
    stest.expect_equal(iface.port2.obj, name.gamma)

def test_mem_map():
    class Test(State):
        map: MemMap|None = None
        map2: MemMap|None = None
    def test_bp(bp: Builder, name: Namespace):
        i = bp.read_state(name, Test, private=True)
        i.map = MemMap(
            base=0, port=Port(name.port), offset=0x10, length=0x2000,
            function=0)
        i.map2 = MemMap(0, name.port2, 0, 0x1000, 0x2000)
    x = expand("", test_bp)
    iface = x.read_state_data(name, Test, private=True)
    stest.expect_equal(iface.map,
        (0, Port(name.port), 0, 0x10, 0x2000, None, 0, 0, 0))
    stest.expect_equal(iface.map2,
        (0, name.port2, 0, 0x1000, 0x2000, None, 0, 0, 0))
    assert iface.map
    stest.expect_equal(iface.map.resolve(), (0, Port(name.port), 0, 0x10,
                                             0x2000))

def test_default_target():
    class Test(State):
        dt: DefaultTarget|None = None
        dt2: DefaultTarget|None = None
    def test_bp(bp: Builder, name: Namespace):
        i = bp.read_state(name, Test, private=True)
        i.dt = DefaultTarget(name.target)
        i.dt2 = DefaultTarget(Port(name.target), offset=0x123, target=name.gw)
    x = expand("", test_bp)
    iface = x.read_state_data(name, Test, private=True)
    stest.expect_equal(iface.dt, (name.target, 0, 0, None, 0, 0))
    stest.expect_equal(iface.dt2, (Port(name.target), 0, 0x123, name.gw, 0, 0))

test_conf_object()
test_port()
test_mem_map()
test_default_target()
