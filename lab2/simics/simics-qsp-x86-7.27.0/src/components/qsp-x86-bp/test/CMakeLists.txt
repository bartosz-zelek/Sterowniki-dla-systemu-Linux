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

if(SIMICS_ENABLE_TESTS_FROM_PACKAGES)
  simics_add_test(disks)
  simics_add_test(network)
  simics_add_test(pcie-card-comp)
  simics_add_test(pcie-card-separate)
  simics_add_test(pcie-card)
  simics_add_test(qsp-params)
  simics_add_test(qsp-pcie)
  simics_add_test(qsp)
  simics_add_test(service-node)
  simics_add_test(usb-disk)
  simics_add_test(x86-simple)
endif()
