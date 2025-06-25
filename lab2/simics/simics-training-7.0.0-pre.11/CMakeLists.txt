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

add_custom_target(training-repo-root-anchor)

set(subdirs "components" "devices")
foreach(subdir ${subdirs})
  file(GLOB cmake_files CONFIGURE_DEPENDS "src/${subdir}/*/CMakeLists.txt")
  foreach(cmake_file ${cmake_files})
    get_filename_component(sub_directory ${cmake_file} DIRECTORY)
    add_subdirectory(${sub_directory})
  endforeach()
endforeach()
