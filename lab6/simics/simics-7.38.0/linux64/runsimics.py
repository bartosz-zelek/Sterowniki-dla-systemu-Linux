# © 2025 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

"""Run Simics as an application.

The only supported use of this module is to use it to launch the Simics
simulator by running `python -m runsimics`.
"""

# These do not need to be cleaned up as they are part of the standard
# prelude anyway
import sys

def cleanup():
    global cleanup, setup_paths, main
    del cleanup
    del setup_paths
    del main

def setup_paths():
    import os
    from pathlib import Path
    import simicsutils.internal
    from simicsutils.host import host_type

    simics_base = Path(simicsutils.internal.simics_base()) / host_type()

    sys.path.append(str(simics_base / "bin" / "py3"))

    # Make sim_init module find Simics DLLs
    if sys.platform.startswith('win'):
        import ctypes
        # LOAD_LIBRARY_SEARCH_DEFAULT_DIRS
        ctypes.windll.kernel32.SetDefaultDllDirectories(0x1000)
        python_package = os.getenv("SIMICS_PYTHON_PACKAGE")
        if python_package:
            # Allow link to Python DLL
            ctypes.windll.kernel32.AddDllDirectory(
                str(Path(python_package) / "win64" / "bin"))
        # Allow link to win64/bin/libsimics-common.dll
        ctypes.windll.kernel32.AddDllDirectory(str(simics_base / "bin"))

def main(cleanup=lambda: None):
    setup_paths()
    import sim_init
    # We don't want any remnants of the bootstrapping in the global
    # namespace, so we clean it up before launching simics.
    cleanup()
    sim_init.launch_simics([sys.executable] + sys.argv[1:])
    # We must not import simics before launch_simics as that would
    # implicitly initialize simics ignoring command line arguments.
    import simics
    simics.SIM_init_command_line()
    simics.SIM_main_loop()

if __name__ == '__main__':
    main(cleanup)
