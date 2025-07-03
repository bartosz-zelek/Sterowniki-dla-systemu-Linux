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

import simics
simics.SIM_load_module("blueprints")

from blueprints import ParamGroup, Param, blueprint, instantiate, Namespace
from blueprints import Builder, Config, BlueprintError, expand
import cli
import conf
import stest

class ClockParams(Config):
    freq_mhz = 1000
    apic_freq_mhz = 200

class SubParams(Config):
    freq_mhz = 400

class Params(Config):
    num_cores = 2
    config_group = None
    enable_subsystem = False

@blueprint([
    Param(name="freq_mhz", desc="A conditional option", config=SubParams)
])
def subsystem(bp: Builder, name: Namespace):
    params = bp.expose_state(name, SubParams)
    bp.obj(name.clock, "clock", freq_mhz=params.freq_mhz)

@blueprint([
    Param(name="freq_mhz", desc="CPU frequency in MHz",
          config=ClockParams),
    Param(name="apic_freq_mhz", desc="APIC frequency in MHz",
          config=ClockParams)
])
def sub_bp(bp: Builder, name: Namespace):
    params = bp.expose_state(name, ClockParams)
    bp.obj(name.clock, "clock", freq_mhz=params.freq_mhz)

name = "top_bp"

@blueprint([
    ParamGroup(name="sub", desc="Subsystem parameters",
               import_bp="sub_bp", count="num_cores"),
    ParamGroup(name="subsystem", desc="Subsystem parameters",
               import_bp="subsystem", enable="enable_subsystem"),
    Param(name="num_cores", desc="Number of cores", config=Params),
    Param(name="enable_subsystem", desc="Subsystem switch",
          config=Params),
], name=name)
def test_bp(bp: Builder, name: Namespace):
    "A simple test blueprint for testing blueprint instantiation."

    params = bp.expose_state(name, Params)
    for i in range(params.num_cores):
        bp.expand(name, f"sub[{i}]", sub_bp)

    # Add subsystem, if enabled
    if params.enable_subsystem:
        bp.expand(name, "subsystem", subsystem)

# List parameters from CLI
print(cli.quiet_run_command(f"list-blueprint-params {name}")[1])

# Cannot set parameter if subsystem not enabled
with stest.expect_exception_mgr(BlueprintError):
    expand("top", test_bp, params={"subsystem:freq_mhz": 1000})

expand("top", test_bp, params={"enable_subsystem": True, "subsystem:freq_mhz": 1000})
stest.expect_equal(cli.quiet_run_command("print-blueprint-state")[1].split(),
                   """
Params[top]
  num_cores               2
  config_group            None
  enable_subsystem        True

ClockParams[top.sub[0]]
  freq_mhz                0x3e8
  apic_freq_mhz           0xc8

ClockParams[top.sub[1]]
  freq_mhz                0x3e8
  apic_freq_mhz           0xc8

SubParams[top.subsystem]
  freq_mhz                0x3e8
""".split())

# Instantiate and set some dynamic parameters

instantiate("top", test_bp, params={
    "num_cores": 3,
    "sub[0]:freq_mhz": 1000,
    "sub[1]:freq_mhz": 2000,
    "sub[2]:freq_mhz": 3000,
})

stest.expect_equal(cli.quiet_run_command("print-blueprint-state")[1].split(),
                   """
Params[top]
  num_cores               3
  config_group            None
  enable_subsystem        False

ClockParams[top.sub[0]]
  freq_mhz                0x3e8
  apic_freq_mhz           0xc8

ClockParams[top.sub[1]]
  freq_mhz                0x7d0
  apic_freq_mhz           0xc8

ClockParams[top.sub[2]]
  freq_mhz                0xbb8
  apic_freq_mhz           0xc8
""".split())

stest.expect_equal(conf.top.sub[0].clock.freq_mhz, 1000)
stest.expect_equal(conf.top.sub[1].clock.freq_mhz, 2000)
stest.expect_equal(conf.top.sub[2].clock.freq_mhz, 3000)
