# © 2022 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

# sample-device-* modules ship with CTest by default as an example; but they
# are not expected to run by default in a user's project. User can opt-in (and
# run these tests) by copying the module to the project and modify this
# if-statement; or set the flag in their configuration
if(SIMICS_ENABLE_TESTS_FROM_PACKAGES)
  simics_add_test(after)
  simics_add_test(after-bank)
  simics_add_test(attribute)
  simics_add_test(bank)
  simics_add_test(event)
  simics_add_test(info-status)
  simics_add_test(interface)
  simics_add_test(logging)
  simics_add_test(multi)
  simics_add_test(port)
endif()
