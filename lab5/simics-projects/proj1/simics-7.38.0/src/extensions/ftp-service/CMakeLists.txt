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

# NOTE: does not build outside of Base repo
simics_add_module(ftp-service
  CLASSES ftp-service ftp-control ftp-data
  SOURCES ftp-service.c module_load.py
  INIT_LOCAL
  SIMICS_API latest
)
target_link_libraries(ftp-service
    PRIVATE network-common  # network-common is a STATIC library
)
