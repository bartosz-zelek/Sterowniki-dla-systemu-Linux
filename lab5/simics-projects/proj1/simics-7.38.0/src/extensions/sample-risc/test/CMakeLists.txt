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
  set(tests "add;branch-direct;branch-indirect;config-sanity;exec-bp-logical;"
    "exec-bp-physical-nested;exec-bp-physical;load-stall;load;nop;"
    "read-bp-logical;read-bp-physical-nested;read-bp-physical;store;"
    "write-bp-logical;write-bp-physical-nested;write-bp-physical")
  foreach(test ${tests})
    simics_add_test(${test} ENV "SAMPLE_RISC_CLASS=sample-risc;SAMPLE_RISC_CORE_CLASS=sample-risc-core")
  endforeach()
endif()

