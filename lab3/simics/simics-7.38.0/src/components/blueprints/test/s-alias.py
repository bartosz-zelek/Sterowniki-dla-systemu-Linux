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


from blueprints import Builder, instantiate, Alias, State, Namespace
import stest

class Result: pass

def simple():
    r = Result()

    class Iface(State):
        a = 0
        b = 0
    def test_bp(bp: Builder, name: Namespace):
        t1 = bp.read_state(name, Iface, private=True)
        t1.a = Alias(t1).b
        t1.b = 22
        r.a = t1.a
        r.b = t1.b

    instantiate("", test_bp)
    stest.expect_true(r.a == r.b == 22)

simple()


# Test simple key aliasing
def test1():
    r = Result()

    class IfaceX(State):
        a = 0
        b = 0
        c = 0
    class IfaceY(State):
        aa = 0
        bb = 0

    def test_bp(bp: Builder, ns):
        t1 = bp.read_state(ns, IfaceX, private=True)
        t2 = bp.read_state(ns, IfaceY, private=True)
        t1.a = Alias(t2).aa
        t1.b = Alias(t2).bb
        t1.c = Alias(t1).a
        t1.a = 20
        t2.bb = 40
        r.a = t1.a
        r.b = t1.b
        r.c = t1.c
        r.aa = t2.aa
        r.bb = t2.bb

    instantiate("", test_bp)
    stest.expect_true(r.a == r.aa == r.c == 20)
    stest.expect_true(r.b == r.bb == 40)


# Test array aliasing
def test2():
    r = Result()

    class IfaceX(State):
        a = []
        b = []
        c = []
    class IfaceY(State):
        aa = []
        bb = []

    def test_bp(bp: Builder, ns):
        t1 = bp.read_state(ns, IfaceX, private=True)
        t2 = bp.read_state(ns, IfaceY, private=True)
        t1.a.append(20)
        t2.bb.append(40)
        t1.a = Alias(t2).aa
        t1.b = Alias(t2).bb
        t1.c = Alias(t1).a
        #t2.bb = 40
        r.a = t1.a
        r.b = t1.b
        r.c = t1.c
        r.aa = t2.aa
        r.bb = t2.bb

    instantiate("", test_bp)
    stest.expect_true(r.a == r.aa == r.c == [20])
    stest.expect_true(r.b == r.bb == [40])

test1()
#test2()
