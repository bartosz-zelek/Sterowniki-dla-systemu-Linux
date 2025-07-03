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


from blueprints import instantiate, Builder, Namespace

def ram(bp: Builder, ns, size, **args):
    bp.obj(ns, 'ram', image = ns.image, **args)
    bp.obj(ns.image, 'image', size = size)

def memory_space(bp: Builder, name: Namespace, **args):
    bp.obj(name, 'memory-space', **args, log_level=4)

    # Extra fluff
    bp.obj(name.kappa, 'namespace')
    bp.obj(name.kappa.beta, 'namespace')
    bp.expand(name, "ram", ram, size = 0x100000)

def test_bp(bp: Builder, ns, freq_mhz):
    bp.obj(ns.clock, 'clock', freq_mhz=freq_mhz)
    bp.expand(ns, "space", memory_space, map=[], queue=ns.clock)


instantiate("", test_bp, freq_mhz=20)
