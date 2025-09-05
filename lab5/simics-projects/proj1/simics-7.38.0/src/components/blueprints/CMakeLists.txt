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

# Setup blueprint python files
set(python_files
  # CLI commands
  cmds.py

  # Classes
  classes/__init__.py
  classes/namespace.py
)

simics_add_module(blueprints
  CLASSES subsystem-namespace blueprint-namespace
  SOURCES module_load.py ${python_files}
  SIMICS_API latest
)
add_subdirectory(test)
