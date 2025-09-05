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


from blueprints import Builder, Namespace, expand
from blueprints.types import Dict, State, StateMap, List, NSState, Set
import stest

def test_namespace():
    ns = Namespace("qsp.system.mb")
    stest.expect_equal(str(ns), "qsp.system.mb")
    stest.expect_equal(str(ns.kappa), "qsp.system.mb.kappa")
    stest.expect_equal(str(ns.kappa[1][2]), "qsp.system.mb.kappa[1][2]")
    stest.expect_equal(str(ns.dev@2), "qsp.system.mb.dev2")
    # dataclass implements == operator by default
    stest.expect_equal(ns, Namespace("qsp.system.mb"))

    # Sanity check of Namespace "in" operator
    stest.expect_exception(
        lambda: [s.cpu for s in Namespace("x").foo], [], TypeError)

class TestIface(State):
    d = Dict[str, int]()
    q = Set[int]()

def test_list():
    class Test(State):
        list = List[int]()
        list2 = []
        list3 = []
        lists = ([], [])

    def test_bp(bp: Builder, name: Namespace):
        i = bp.read_state(name, Test, private=True)
        i.list.append(20)
        i.list.extend([40, 50])
        i.list.append(30)
        i.list2.append(1)
        i.lists[1].extend([3, 4])

    x = expand("", test_bp)
    iface = x.read_state_data(Namespace(""), Test, private=True)
    stest.expect_equal(iface.list, [20, 40, 50, 30])
    stest.expect_equal(list(iface.list), [20, 40, 50, 30])
    stest.expect_equal(iface.list2, [1])
    stest.expect_equal(iface.list3, [])
    stest.expect_equal(iface.lists[1], [3, 4])
    stest.expect_true(20 in iface.list)
    stest.expect_false(99 in iface.lists[0])

def test_dict():
    def my_bp(bp: Builder, name: Namespace):
        iface = bp.read_state(name, TestIface, private=True)
        iface.d['kappa'] = 20
        iface.d['gamma'] = 30

    x = expand("", my_bp)
    iface = x.read_state_data(Namespace(""), TestIface, private=True)
    stest.expect_equal(iface.d.get('kappas', default="hello"), "hello")
    stest.expect_equal(iface.d['kappa'], 20)
    stest.expect_equal(iface.d['gamma'], 30)
    stest.expect_equal(iface.d.get('unset'), None)
    stest.expect_true("kappa" in iface.d)
    stest.expect_false("unknown" in iface.d)
    stest.expect_equal(set(x for x in iface.d), {"gamma", "kappa"})
    d = dict(iface.d.items())
    stest.expect_equal(set(d.values()), set(d.values()))
    stest.expect_equal(set(d.items()), set(d.items()))

def test_set():
    def my_bp(bp: Builder, name: Namespace):
        iface = bp.read_state(name, TestIface, private=True)
        iface.q.add(20)
        iface.q.add(30)

    x = expand("", my_bp)
    iface = x.read_state_data(Namespace(""), TestIface, private=True)
    stest.expect_true(20 in iface.q)
    stest.expect_false(21 in iface.q)
    stest.expect_equal(set(iface.q), {20, 30})

def test_nsiface():
    class NSTest(NSState[int]):
        delta = 30
        gamma = 40
        epsilon: str = ""

    def my_bp(bp: Builder, name: Namespace):
        iface = bp.read_state(name, NSTest, private=True)
        iface.kappa = 20
        iface.alpha = 10
        iface.gamma = 50
    x = expand("", my_bp)
    iface = x.read_state_data(Namespace(""), NSTest, private=True)
    stest.expect_equal(iface.kappa, 20)
    stest.expect_equal(iface.alpha, 10)
    stest.expect_equal(iface.delta, 30)
    stest.expect_equal(iface.gamma, 50)
    stest.expect_true(hasattr(iface, "alpha"))
    stest.expect_false(hasattr(iface, "notset"))
    stest.expect_equal(list(iface), [
        'alpha', 'delta', 'epsilon', 'gamma', 'kappa'])

def test_ifacemap():
    class Inner(State):
        a = 10

    class Test(State):
        test = StateMap(Inner)
        maplist = [StateMap[int, Inner](Inner) for x in range(10)]
        inner = Inner()
        untouched = Inner()
        freq_mhz = 200
        cpu_list = []
        value = [20, "hello"]

    def test_comp(bp: Builder, name: Namespace):
        i = bp.read_state(name, Test, private=True)
        _ = i.inner.a
        i.inner.a = 20
        i.test[3].a = 30
        i.maplist[1][20].a = 40

    x = expand("", test_comp)
    iface = x.read_state_data(Namespace(""), Test, private=True)
    stest.expect_equal(iface.inner.a, 20)
    stest.expect_equal(iface.test[3].a, 30)
    stest.expect_equal(iface.maplist[1][20].a, 40)
    stest.expect_equal(iface.untouched.a, 10)

# Test state references
def test_iface_ref():
    class Inner(State):
        a = 10
        b = 30

    class Test(State):
        main = Inner()
        ref = Inner()

    def test_comp(bp: Builder, name: Namespace):
        i = bp.read_state(name, Test, private=True)
        i.main.a = 20
        i.ref = i.main
        i.ref.b = 30

    x = expand("", test_comp)
    iface = x.read_state_data(Namespace(""), Test, private=True)
    stest.expect_equal(iface.main.a, 20)
    stest.expect_equal(iface.main.b, 30)
    stest.expect_equal(iface.ref.a, 20)
    stest.expect_equal(iface.ref.b, 30)

test_namespace()
test_list()
test_dict()
test_set()
test_nsiface()
test_ifacemap()
test_iface_ref()
