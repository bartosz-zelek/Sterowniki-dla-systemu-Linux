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

# Configuration
message(STATUS "Detecting current kernel...")
execute_process(
  COMMAND uname -r OUTPUT_VARIABLE KERNEL_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE)
set(KERNEL_SOURCE "/lib/modules/${KERNEL_VERSION}/build"
  CACHE INTERNAL "Kernel source location")
message(STATUS "found at: ${KERNEL_SOURCE}")

if (KERNEL_VERSION VERSION_LESS 3.1.0)
    message(FATAL_ERROR "Linux kernel version is too old")
endif()

set(VMXMON_MODULE "${CMAKE_BINARY_DIR}/module"
  CACHE INTERNAL "vmxmon module location")

# We are not building any libraries or binaries using CMake rules; we are
# building a kernel modules using "KMake"
add_custom_target(vmxmon ALL DEPENDS ${VMXMON_MODULE}/vmxmon.ko)

# When loading the driver, the script invokes the utility programs so make
# sure they are also built by the same top-level target for simplicity
add_dependencies(vmxmon check-hw-util vmxmon-version copy-scripts)

set(KMAKE make --no-print-directory -C ${KERNEL_SOURCE}
  CACHE INTERNAL "Kernel Make invocation")
add_custom_command(OUTPUT ${VMXMON_MODULE}/vmxmon.ko
  COMMENT "Main target; builds the vmxmon kernel module"
  DEPENDS ${VMXMON_MODULE}/Makefile
  COMMAND ${KMAKE} M=${VMXMON_MODULE} CC=${CMAKE_C_COMPILER}
  BYPRODUCTS ${VMXMON_MODULE}/common/vmx.o)

# TODO(ah): We wanted to use O=<path> option but it did not seem to work or do
# what I wanted (i.e. output build artifacts to specified path leaving source
# directly clean). So we *copy* module to build directory instead. Simply
# symlinking will not work as kernel build emits -o files to linux/ subdir
set(VMXMON_MODULE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/module")
add_custom_command(OUTPUT ${VMXMON_MODULE}/Makefile
  COMMENT "Setup kernel module build-infra (copy to build tree)"
  DEPENDS ${VMXMON_MODULE_SRC}/Makefile
  COMMAND mkdir -p ${VMXMON_MODULE}
  COMMAND cp -a ${VMXMON_MODULE_SRC}/common ${VMXMON_MODULE}/
  COMMAND cp -a ${VMXMON_MODULE_SRC}/include ${VMXMON_MODULE}/
  COMMAND cp -a ${VMXMON_MODULE_SRC}/linux ${VMXMON_MODULE}/
  COMMAND cp ${VMXMON_MODULE_SRC}/Makefile ${VMXMON_MODULE}/Makefile)

set_directory_properties(PROPERTIES
  ADDITIONAL_MAKE_CLEAN_FILES ${VMXMON_MODULE})

add_custom_target(copy-scripts ALL
  COMMENT "Copying scripts to build directory"
  COMMAND cp -a ${CMAKE_SOURCE_DIR}/scripts ${CMAKE_BINARY_DIR})

# Utility targets
add_custom_target(load
  COMMENT "Load vmxmon kernel module"
  DEPENDS vmxmon
  COMMAND ${CMAKE_BINARY_DIR}/scripts/load-modules)

add_custom_target(unload
  COMMENT "Unload vmxmon kernel module"
  COMMAND ${CMAKE_BINARY_DIR}/scripts/unload-modules)

# NOTE(ah): CMake has its own concept of installation so we cannot use the
# `install` target for installing vmxmon
add_custom_target(vmxmon-install
  COMMENT "Install (via sudo) the vmxmon kernel module"
  DEPENDS vmxmon
  COMMAND sudo ${CMAKE_BINARY_DIR}/scripts/install)

add_custom_target(vmxmon-uninstall
  COMMENT "Uninstall (via sudo) the vmxmon kernel module"
  COMMAND sudo ${CMAKE_BINARY_DIR}/scripts/uninstall)
