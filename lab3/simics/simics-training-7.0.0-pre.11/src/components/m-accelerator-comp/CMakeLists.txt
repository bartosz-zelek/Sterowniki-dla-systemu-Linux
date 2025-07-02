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

simics_add_module(m-accelerator-comp
  COMPONENTS m_accelerator_comp
  SOURCES m_accelerator_comp.py module_load.py
  SIMICS_API latest
)

if(NOT TARGET all-training-modules)
  add_custom_target(all-training-modules)
endif()

add_dependencies(all-training-modules m-accelerator-comp)

set(IMAGE_DIR ${SIMICS_HOST_LIB_DIR}/images)
set(SRC_IMAGES mandel-chip-icon.png)

# FIXME: Switch over to a centralized "copy images" feature once available
foreach(SRC_IMAGE ${SRC_IMAGES})
  add_custom_command(
    OUTPUT ${IMAGE_DIR}/${SRC_IMAGE}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${SRC_IMAGE} ${IMAGE_DIR}/${SRC_IMAGE}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SRC_IMAGE}
  )
endforeach()

list(TRANSFORM SRC_IMAGES PREPEND ${IMAGE_DIR}/)

add_custom_target(m-accelerator-comp-images
  DEPENDS ${SRC_IMAGES})
add_dependencies(m-accelerator-comp m-accelerator-comp-images)

