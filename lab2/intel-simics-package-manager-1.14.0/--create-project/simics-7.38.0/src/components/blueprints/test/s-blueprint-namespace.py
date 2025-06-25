# © 2024 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

import conf
import simics
import stest
from blueprints import instantiate, Builder, Namespace
from simmod.blueprints.classes.namespace import SubsystemNamespace

# Test adding an explicit namespace object at the top
def explicit_namespace():
    def foo(builder: Builder, name: Namespace):
        builder.obj(name, "blueprint-namespace", blueprint="foo", info="bar")

    instantiate("foo", foo)
    stest.expect_equal(conf.foo.classname, "blueprint-namespace")
    stest.expect_equal(conf.foo.blueprint, "foo")
    stest.expect_equal(conf.foo.info, "bar")
    simics.SIM_delete_objects(list(simics.SIM_object_iterator(None)))

# Test using bespoke namespace using confclass
def custom_namespace_class():
    # fisketur[lost-assignment]
    class CustomNamespace(SubsystemNamespace):
        cls = simics.confclass('custom-namespace',
                               parent=SubsystemNamespace.cls)
        cls.attr.extra_data("i", default=42)

    def foo(builder: Builder, name: Namespace):
        builder.obj(name, "custom-namespace", blueprint="foo", info="bar")

    instantiate("foo", foo)
    stest.expect_equal(conf.foo.classname, "custom-namespace")
    stest.expect_equal(conf.foo.blueprint, "foo")
    stest.expect_equal(conf.foo.info, "bar")
    stest.expect_equal(conf.foo.extra_data, 42)
    simics.SIM_delete_objects(list(simics.SIM_object_iterator(None)))

explicit_namespace()
custom_namespace_class()
