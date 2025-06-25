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

# Setup blueprint python files
set(python_files
  # Classes
  classes/__init__.py
  classes/x86_classes.py

  # QSP X86
  qsp_x86/__init__.py
  qsp_x86/board.py
  qsp_x86/disks.py
  qsp_x86/ich10.py
  qsp_x86/state.py
  qsp_x86/x58.py

  # X86 CPU
  x86_cpu/__init__.py
  x86_cpu/cpu.py
  x86_cpu/state.py
  x86_cpu/system.py

  # Tests
  tests/__init__.py
  tests/disks.py
  tests/network_setup.py
  tests/pcie.py
  tests/qsp_platform.py
)

simics_add_module(qsp-x86-bp
  CLASSES x58-mb
  SOURCES module_load.py ${python_files}
  SIMICS_API latest
)
add_subdirectory(test)
