# © 2010 Intel Corporation
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
project(RISCV_BSP LANGUAGES C CXX ASM)

# Setup default C and C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT TARGET risc-v-simple-bsp)

  add_library(risc-v-simple-bsp STATIC
              board_init.c
              timer.c
              uart.c
              shared_console.c
              counter.c
              mailbox.c
              sync.c)

  target_include_directories(risc-v-simple-bsp PUBLIC include)
  target_link_options(risc-v-simple-bsp INTERFACE -T ${CMAKE_CURRENT_SOURCE_DIR}/link.ld)

  # An x86 objcopy can dump the checksums to standard out like so
  #  objcopy -Ielf64-x86-64 --dump-section .source_shas=$(tty) riscv-executable

  function(add_source_check target)
    list(SUBLIST ARGV 1 -1 file_list)
    find_program(SHA256 sha256sum REQUIRED)
    get_filename_component(COMPILER_PATH ${CMAKE_C_COMPILER} DIRECTORY)
    get_filename_component(COMPILER_EXEC ${CMAKE_C_COMPILER} NAME)
    string(REGEX REPLACE "gcc$" "objcopy" OBJCOPY_EXEC ${COMPILER_EXEC})
    if(NOT EXISTS ${COMPILER_PATH}/${OBJCOPY_EXEC})
      message(FATAL_ERROR "Could not find ${OBJCOPY_EXEC} in ${COMPILER_PATH}")
    endif()
    get_target_property(BSP_SRC risc-v-simple-bsp SOURCES)
    get_target_property(BSP_DIR risc-v-simple-bsp SOURCE_DIR)
    list(TRANSFORM BSP_SRC PREPEND ${BSP_DIR}/)
    file(GLOB_RECURSE BSP_HDS FOLLOW_SYMLINKS ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/*.h)
    add_custom_command(TARGET ${target}
                       POST_BUILD
                       COMMAND ${SHA256} ${file_list} ${BSP_SRC} ${BSP_HDS} ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/CMakeLists.txt> source_shas.txt
                       COMMAND ${COMPILER_PATH}/${OBJCOPY_EXEC} --add-section .source_shas=source_shas.txt
                               --set-section-flags .source_shas=noload $<TARGET_FILE:${target}>
                       COMMAND_EXPAND_LISTS)
  endfunction()

endif()
