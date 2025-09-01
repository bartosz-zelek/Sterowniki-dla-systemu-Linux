# © 2023 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

# NOTE: does not build outside of Base repo, need base_copy_images
simics_add_module(console-components
  COMPONENTS txt_console_comp gfx_console_comp
  SOURCES console_components.py module_load.py
  SIMICS_API latest
)

if(COMMAND base_copy_images)
  base_copy_images(console-components IMAGES textcon.png graphcon.png)
endif()
