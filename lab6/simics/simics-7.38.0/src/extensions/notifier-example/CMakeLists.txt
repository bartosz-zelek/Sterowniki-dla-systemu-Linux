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

simics_add_module(notifier-example
  CLASSES tcp_server connected_device
  SOURCES connected-device.dml notifier-example.c sock.c tcp-server.c
  INIT_LOCAL
  SIMICS_API latest
)
add_subdirectory(test)

if(WIN32)
  target_link_libraries(notifier-example PRIVATE ws2_32)
endif()
