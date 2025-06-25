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


from blueprints import Builder, State, ConfObject, instantiate, Namespace
from blueprints.types import BlueprintError
import conf
import stest

class Message:
    msgs: list
    cls = confclass("messages")
    cls.attr.msgs("[s*]", default=[])

class Msg(State):
    msgs = ConfObject()

def post_a(data=None):
    if data is not None:
        data.msgs.append("a")
    else:
        raise Exception("no data in post_a")

def post_b(data):
    if data is not None:
        data.msgs.append("b")
    else:
        raise Exception("no data in post_b")

def bp_a(bp: Builder, name: Namespace):
    iface = bp.read_state(name, Msg)
    bp.at_post_instantiate(name, post_a, data=iface.msgs)

def bp_b(bp: Builder, name: Namespace):
    iface = bp.read_state(name, Msg)
    bp.at_post_instantiate(name, post_b, data=iface.msgs)

def root_helper(bp: Builder, name: Namespace):
    iface = bp.expose_state(name, Msg)
    iface.msgs = bp.obj(name, Message.cls.classname)

def test_simple():
    def root(bp: Builder, name: Namespace):
        bp.obj(name, root_helper)
        bp.obj(name.a, bp_a)
    instantiate("root", root)
    stest.expect_equal(conf.root.msgs, ["a"])
    simics.SIM_delete_objects(list(simics.SIM_object_iterator(None)))

def test_ab():
    def root(bp: Builder, name: Namespace):
        bp.obj(name, root_helper)
        bp.obj(name.a, bp_a)
        bp.obj(name.b, bp_b)

    instantiate("root", root)
    stest.expect_equal(conf.root.msgs, ["a", "b"])
    simics.SIM_delete_objects(list(simics.SIM_object_iterator(None)))

def test_ba():
    def root(bp: Builder, name: Namespace):
        bp.obj(name, root_helper)
        bp.obj(name.b, bp_b)
        bp.obj(name.a, bp_a)

    instantiate("root", root)
    stest.expect_equal(conf.root.msgs, ["b", "a"])
    simics.SIM_delete_objects(list(simics.SIM_object_iterator(None)))

def test_error():
    def bp_a_err(bp: Builder, name: Namespace):
        bp.at_post_instantiate(name, post_a)
    def root(bp: Builder, ns):
        bp.expand(ns, "", root_helper)
        bp.expand(ns, "a", bp_a_err)
        bp.expand(ns, "b", bp_b)

    stest.expect_exception(instantiate, ("root", root),
                           BlueprintError)
    stest.expect_equal(conf.root.msgs, ["b"])
    simics.SIM_delete_objects(list(simics.SIM_object_iterator(None)))

test_simple()
test_ab()
test_ba()
test_error()
