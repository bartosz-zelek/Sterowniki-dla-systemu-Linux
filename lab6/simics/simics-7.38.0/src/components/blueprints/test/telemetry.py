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

import os
import json
import pathlib
import testparams as tp

def check_data(f):
    data = json.loads(f.read_text())
    assert data["core.platform"]["num-top-level-comp"] == 1
    assert data["core.platform"]["top_level_classes"] == ["blueprint-namespace"]

def test():
    dump_file = pathlib.Path(tp.sandbox_path()) / "telemetry.json"

    # Run telemetry and report test data
    env = os.environ.copy()
    env['SIMICS_TELEMETRY_DUMP_FILE'] = str(dump_file)
    tp.add_subtest("s-uart-example.py", name="check-telemetry-1", env=env)

    # Check dump of telemetry test data
    tp.add_testfun("check-telemetry-2", check_data, args=[dump_file])
