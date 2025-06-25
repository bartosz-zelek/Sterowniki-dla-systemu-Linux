# © 2022 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

from blueprints import instantiate, Builder, Namespace, BlueprintError, ConfObject
import simics
import stest
import conf

# Test that bp.set can set port-object attributes
def test_port_obj_attr():
    # fisketur[lost-assignment]
    class TestClass:
        cls = simics.confclass('test')
        cls.attr.main_attr("i", default=20)
        pcls = cls.o.bank.testbank()
        pcls.attr.port_attr("i", default=100)

    def my_bp(bp: Builder, name: Namespace):
        bp.obj(name.top, 'test', main_attr=21)
        bp.set(name.top.bank.testbank, port_attr=101)

    instantiate("", my_bp)
    stest.expect_equal(conf.top.classname, 'test')
    stest.expect_equal(conf.top.bank.classname, 'namespace')
    stest.expect_equal(conf.top.bank.testbank.classname, 'test.testbank')
    stest.expect_equal(conf.top.main_attr, 21)
    stest.expect_equal(conf.top.bank.testbank.port_attr, 101)
    simics.SIM_delete_objects(list(simics.SIM_object_iterator(None)))

# Test that bp.set requires bp.add in the same blueprint
def test_invalid_bp_set():
    # fisketur[lost-assignment]
    class TestClass:
        cls = simics.confclass('test2')
        cls.attr.main_attr("i", default=20)

    def bp1(bp: Builder, name: Namespace):
        bp.set(name.top, main_attr=101)

    def bp22(bp: Builder, name: Namespace):
        bp.set(name.top, main_attr=101)

    def bp2(bp: Builder, name: Namespace):
        bp.expand(name, "top", bp22)
        bp.obj(name.top.top, 'test2')

    stest.expect_exception(instantiate, [bp1], BlueprintError)
    stest.expect_exception(instantiate, [bp2], BlueprintError)
    simics.SIM_delete_objects(list(simics.SIM_object_iterator(None)))

# Test that invalid ports cannot be used
def test_invalid_port_obj():
    # fisketur[lost-assignment]
    class TestClass:
        cls = simics.confclass('test3')
        cls.attr.main_attr("i", default=20)
        pcls = cls.o.bank.testbank()
        pcls.attr.port_attr("i", default=100)

    def my_bp(bp: Builder, name: Namespace):
        bp.obj(name.top, 'test3', main_attr=21)
        # Port object never created
        bp.set(name.top.bank.testbank2, port_attr=101)

    stest.expect_exception(instantiate, ["", my_bp], BlueprintError)
    simics.SIM_delete_objects(list(simics.SIM_object_iterator(None)))

# Test that port objects can be referenced without using bp.set
def test_port_obj_ref():
    # fisketur[lost-assignment]
    class TestClass1:
        cls = simics.confclass('test4')
        pcls = cls.o.bank.testbank()

    # fisketur[lost-assignment]
    class TestClass2:
        cls = simics.confclass('test5')
        cls.attr.ref("o")

    def my_bp(bp: Builder, name: Namespace):
        # This has an automatically created .bank.testbank
        bp.obj(name.top1, 'test4')
        # Refer to bank object, this should be allowed
        bp.obj(name.top2, 'test5', ref=ConfObject(name.top1.bank.testbank))

    instantiate("", my_bp)
    stest.expect_equal(conf.top1.bank.testbank, conf.top2.ref)
    simics.SIM_delete_objects(list(simics.SIM_object_iterator(None)))

test_port_obj_attr()
test_invalid_bp_set()
test_invalid_port_obj()
test_port_obj_ref()
