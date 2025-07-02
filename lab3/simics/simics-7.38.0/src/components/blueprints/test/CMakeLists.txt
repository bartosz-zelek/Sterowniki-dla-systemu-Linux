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
  simics_add_test(adapter)
  simics_add_test(alias)
  simics_add_test(binding)
  simics_add_test(blueprint-namespace)
  simics_add_test(compobj)
  simics_add_test(data)
  simics_add_test(list-blueprints)
  simics_add_test(port-obj)
  simics_add_test(post-instantiate)
  simics_add_test(simtypes)
  simics_add_test(state-eval)
  simics_add_test(state)
  simics_add_test(stateset)
  simics_add_test(types)
  simics_add_test(uart-example)
endif()
