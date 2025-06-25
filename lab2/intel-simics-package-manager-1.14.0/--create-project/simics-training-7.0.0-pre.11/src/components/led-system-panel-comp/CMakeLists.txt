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

simics_add_module(led-system-panel-comp
  COMPONENTS led_system_panel_comp
  SOURCES led_system_panel_comp.py module_load.py
  SIMICS_API latest
)
add_subdirectory(test)

if(NOT TARGET all-training-modules)
  add_custom_target(all-training-modules)
endif()

add_dependencies(all-training-modules led-system-panel-comp)

set(IMAGE_DIR ${SIMICS_HOST_LIB_DIR}/images)
set(SRC_IMAGES bg-board-1000x500.png  led-background.png  button-A.png
               button-B.png button-A-highlight.png  button-B-highlight.png
               led-blue.png led-green.png led-red.png
               led-black.png led-cyan.png  led-magenta.png
               led-white.png toggle-1.png led-yellow.png
               toggle-0.png toggle-change.png)


# FIXME: Switch over to a centralized "copy images" feature once available
foreach(SRC_IMAGE ${SRC_IMAGES})
  add_custom_command(
    OUTPUT ${IMAGE_DIR}/${SRC_IMAGE}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${SRC_IMAGE} ${IMAGE_DIR}/${SRC_IMAGE}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SRC_IMAGE}
  )
endforeach()

list(TRANSFORM SRC_IMAGES PREPEND ${IMAGE_DIR}/)

add_custom_target(led-system-panel-comp-copy-images
  DEPENDS ${SRC_IMAGES})
add_dependencies(led-system-panel-comp led-system-panel-comp-copy-images)

