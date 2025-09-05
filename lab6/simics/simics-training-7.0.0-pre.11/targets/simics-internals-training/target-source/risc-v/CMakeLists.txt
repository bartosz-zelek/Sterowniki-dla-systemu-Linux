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
message(STATUS "CMake version: ${CMAKE_VERSION}")

# Use newer (and faster) handling of variable references and escape chars
cmake_policy(SET CMP0053 NEW)

# Name of project and languages used
project(all_target_sources LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(RISCV_IMAGES_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${RISCV_IMAGES_OUTPUT_DIRECTORY})
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../../images)
endif()

file(GLOB SUB_DIRS ${CMAKE_CURRENT_LIST_DIR}/*/CMakeLists.txt)
list(FILTER SUB_DIRS EXCLUDE REGEX .*/risc-v-simple-bsp/.*)
foreach(SUB_DIR ${SUB_DIRS})
  get_filename_component(SUB_DIR ${SUB_DIR} DIRECTORY)
  add_subdirectory(${SUB_DIR})
endforeach()


