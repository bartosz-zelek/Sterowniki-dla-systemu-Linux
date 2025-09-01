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

add_executable(check-hw-util check-hw-util.c)
target_compile_options(check-hw-util PRIVATE
  -Wall -Werror -Wpedantic)

add_executable(vmxmon-version vmxmon-version.c)
target_compile_options(vmxmon-version PRIVATE
  -Wall -Werror -Wpedantic)
target_include_directories(vmxmon-version PRIVATE
  ${CMAKE_SOURCE_DIR}/module/include)

add_executable(simcall simcall.c)
target_compile_options(simcall PRIVATE -Wall -Werror -Wpedantic)
target_include_directories(simcall PRIVATE ${CMAKE_SOURCE_DIR}/module/common)
target_include_directories(simcall PRIVATE ${CMAKE_SOURCE_DIR}/module/include)
