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

if(SIMICS_REPO_ROOT)
  # NOTE: does not build outside of Base repo, source marked as internal
  add_library(network-common STATIC
    arp.c
    icmp.c
    icmp6.c
    ip-addr.c
    ip-stack.c
    ip.c
    ip6-head.c
    ip6-neigh.c
    ip6.c
    net-utils.c
  )
  simics_add_ctest(network-common ip-addr)

else()
  # Public/shared target used by ftp-service module
  add_library(network-common STATIC
    ip-addr.c
    net-utils.c
  )
endif()
target_include_directories(network-common
  # Need include path for service-node module to provide service-node.h where
  # Snlog_IP6 enum is defined. There is a cyclic dependency between
  # network-common and service-node, so this cannot be resolved via
  # target_link_libraries(). Adding as a relative path works as both these
  # modules must be distributed together
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../service-node
  PUBLIC .
)
target_link_libraries(network-common
  PRIVATE Simics::includes
)
