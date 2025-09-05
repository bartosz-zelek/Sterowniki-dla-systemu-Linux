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

if(SIMICS_ENABLE_TESTS_FROM_PACKAGES)
  simics_add_test(access-count-12)
  simics_add_test(access-count-14)
  simics_add_test(connection-name)
  simics_add_test(coverage-12)
  simics_add_test(coverage-14)
  simics_add_test(coverage-read-only-12)
  simics_add_test(coverage-read-only-14)
  simics_add_test(create-12)
  simics_add_test(create-14)
  simics_add_test(empty-12)
  simics_add_test(empty-14)
  simics_add_test(export-12)
  simics_add_test(export-14)
  simics_add_test(inquiry-12)
  simics_add_test(inquiry-14)
  simics_add_test(read-12)
  simics_add_test(read-14)
  simics_add_test(regexp-12)
  simics_add_test(regexp-14)
  simics_add_test(save-12-1)
  simics_add_test(save-12-2)
  simics_add_test(save-14-1)
  simics_add_test(save-14-2)
  simics_add_test(write-12)
  simics_add_test(write-14)

  # Need to save the checkpoint before we can read it
  set_tests_properties(bank-coverage-tool::save-12-2
    PROPERTIES DEPENDS bank-coverage-tool::save-12-1)
  set_tests_properties(bank-coverage-tool::save-14-2
    PROPERTIES DEPENDS bank-coverage-tool::save-14-1)

  # DML 1.2 and 1.4 variants both call into save_1.py writing to the same file
  set_tests_properties(bank-coverage-tool::save-14-1
    PROPERTIES DEPENDS bank-coverage-tool::save-12-1)
endif()
