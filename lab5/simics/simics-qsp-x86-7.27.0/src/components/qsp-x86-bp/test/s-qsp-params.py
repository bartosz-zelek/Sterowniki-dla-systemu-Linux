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

import simics
simics.SIM_load_module("blueprints")
#simics.SIM_load_module("qsp-x86-bp")

from blueprints import blueprint, ParamGroup, Namespace, Builder, instantiate
from simmod.qsp_x86_bp.tests.qsp_platform import qsp_platform, QSPSystemParams
import cli
import stest
import conf

# SIMICS-21634
conf.sim.deprecation_level = 0

@blueprint([ParamGroup("test", "", import_bp="qsp_platform")],
           "qsp_test")
def qsp_test(bp: Builder, name: Namespace):
    bp.expand(name, "", qsp_platform)

@blueprint([ParamGroup("test", "", import_bp="qsp_platform")],
           "qsp_test")
def qsp_test_overwrite(bp: Builder, name: Namespace):
    # Create config on sub-node since only one state type per node allowed
    conf = bp.create_config(name.qsp, QSPSystemParams)
    conf.board_name = "test"
    bp.expand(name, "qsp", qsp_platform)

def qsp_test_wrapper(bp: Builder, name: Namespace):
    conf = bp.create_config(name.test1, QSPSystemParams)
    conf.board_name = "board0"
    bp.expand(name, "test1", qsp_test)

    # This config is ignored
    conf = bp.create_config(name.test2, QSPSystemParams)
    conf.board_name = "board1"
    bp.expand(name, "test2", qsp_test_overwrite)

# Verify imported parameter
stest.expect_true(
    "test:board_name"
    in set(cli.global_cmds.list_blueprint_params(blueprint="qsp_test")))

instantiate("", qsp_test_wrapper)

objs = set(cli.global_cmds.list_objects(_all=True))

# First board works as expected
stest.expect_true(conf.test1.board0 in objs)

# Second board ignores the board name in our config
stest.expect_false(hasattr(conf.test2.qsp, "board1"))
stest.expect_true(conf.test2.qsp.test in objs)
