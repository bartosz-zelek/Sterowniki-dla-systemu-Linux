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

if(SIMICS_ENABLE_TESTS_FROM_PACKAGES)
  simics_add_test(info-status)
  simics_add_test(virtio-blk)
  simics_add_test(virtio-entropy)
  simics_add_test(virtio-fs)
  simics_add_test(virtio-net)
  simics_add_test(virtio-pcie-blk)
  simics_add_test(virtio-pcie-sriov-blk)
endif()
