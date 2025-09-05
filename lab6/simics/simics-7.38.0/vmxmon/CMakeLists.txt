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

cmake_minimum_required(VERSION 3.20.3)
project(vmxmon LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
  include(cmakelists-windows.cmake)
else()
  include(cmakelists-linux.cmake)
endif()

add_subdirectory(util)

# Some of the tests are not possible to build during a Coverity build; nor
# during the Conan validation builds. Nor are they needed when end-user is
# building the kernel during <base>/bin/vmp-kernel-install. Add option to turn
# them off.
if(NOT DISABLE_TESTS)

include(CTest)  # must be done *before* add_subdirectory or recursion will fail
add_subdirectory(test)
add_subdirectory(benchmark)

endif()  # DISABLE_TESTS
