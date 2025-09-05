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

cmake_minimum_required(VERSION 3.22)

# Use newer (and faster) handling of variable references and escape chars
cmake_policy(SET CMP0053 NEW)

# Name of project and languages used
project(hello_world LANGUAGES C CXX ASM)

# Setup default C and C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(../risc-v-simple-bsp risc-v-simple-bsp)

add_executable(helloworld.rv.baremetal.elf
               helloworld.c)

target_link_libraries(helloworld.rv.baremetal.elf  PRIVATE risc-v-simple-bsp c)
target_link_options(helloworld.rv.baremetal.elf  PUBLIC -Wl,-Map runme.map)

add_source_check(helloworld.rv.baremetal.elf ${CMAKE_CURRENT_SOURCE_DIR}/helloworld.c
                                   ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)


