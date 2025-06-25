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

# do not clobber simics namespace
import sys as _sys
import runsimics as _runsimics

if _sys.version_info.major < 3 or _sys.version_info.minor < 10:
    print("Simics requires at least Python 3.10", file=_sys.stderr)
    _sys.exit(1)

_runsimics.setup_paths()

from _simics.api import *
if not 'sim_init' in _sys.modules:
    CORE_init_from_python()
    import link_components

del _sys
del _runsimics
