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


from typing import Callable, NamedTuple
from blueprints import Builder, instantiate, State, Namespace
from blueprints import Default, Override
import stest
import simics
import conf

class Result: pass
result = Result()

def test_invalid_field_exception():
    class TestState(State):
        alpha = 10

    def my_bp(bp: Builder, name: Namespace):
        iface = bp.read_state(name, TestState, private=True)

        iface.alpha = 20
        try:
            iface.invalid = 30
        except AttributeError:
            result.error = True

    instantiate("", my_bp)
    stest.expect_true(hasattr(result, "error"))

class Function(NamedTuple):
    func: Callable

def test_function_values():
    def func(): pass
    class TestState(State):
        kappa = Function(func)
    def my_bp(bp: Builder, name: Namespace):
        iface = bp.read_state(name, TestState, private=True)
        result.equal = iface.kappa.func == func

    instantiate("", my_bp)
    stest.expect_true(result.equal)

def test_priority():
    class MyState(State):
        alpha = 10
        beta = 20
        gamma = 30
        delta = 40

    def my_bp(bp: Builder, name: Namespace):
        iface = bp.read_state(name, MyState, private=True)
        iface.alpha = Default(1)
        iface.alpha = Override(2)
        iface.beta = Default(3)
        iface.gamma = 4
        result.alpha = iface.alpha
        result.beta = iface.beta
        result.gamma = iface.gamma
        result.delta = iface.delta

    instantiate("", my_bp)
    stest.expect_equal(result.alpha, 2)
    stest.expect_equal(result.beta, 3)
    stest.expect_equal(result.gamma, 4)
    stest.expect_equal(result.delta, 40)

def test_nesting():
    class Nested(State):
        a = 20
    class Iface(State):
        alist = [Nested(), Nested()]
        dict = {"a": Nested(), 10: Nested()}
        plain = Nested()
    def my_bp(bp: Builder, name: Namespace):
        iface = bp.read_state(name, Iface, private=True)
        result.r = [
            iface.alist[0].a, iface.alist[1].a,
            iface.dict["a"].a, iface.dict[10].a,
            iface.plain.a
        ]
        iface.alist[0].a = 20
        iface.alist[1].a = 21
        iface.dict["a"].a = 22
        iface.dict[10].a = 23
        iface.plain.a = 24
    instantiate("", my_bp)
    stest.expect_equal(result.r, [20, 21, 22, 23, 24])

def test_implicit_read():
    class Iface(State):
        data = 0
    def sub_bp_arg(bp: Builder, name: Namespace, state: Iface):
        result.arg = state.data
    def my_bp(bp: Builder, name: Namespace):
        state = bp.expose_state(name, Iface)
        state.data = 1
        # No need to pass state arguments
        bp.expand(name, "arg", sub_bp_arg)
    instantiate("", my_bp)
    stest.expect_equal(result.arg, 1)

def test_state_dict():
    class MyState(State):
        data = 0
    # fisketur[lost-assignment]
    class MyClass:
        cls = simics.confclass("my-class")
        cls.attr.data("i")

    def my_bp(bp: Builder, name: Namespace):
        state = bp.expose_state(name, MyState)
        state.data = 1
        bp.obj(name.my_obj, "my-class", **state.asdict())
    instantiate("", my_bp)
    stest.expect_equal(conf.my_obj.data, 1)

test_invalid_field_exception()
test_function_values()
test_priority()
test_nesting()
test_implicit_read()
test_state_dict()
