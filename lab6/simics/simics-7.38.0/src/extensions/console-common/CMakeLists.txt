# © 2021 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

# Common deps on headers only
add_library(console-common INTERFACE)
target_include_directories(console-common INTERFACE .)

# NOTE: only used by graphcon and textcon
add_library(console-common-graphcon INTERFACE)
target_sources(console-common-graphcon INTERFACE
  break-strings.c
  break-strings.h
  console-frontend.c
  console-frontend.h
)
target_include_directories(console-common-graphcon INTERFACE .)

# NOTE: only used by sdl-, x11- and xterm-console
add_library(console-common-x11 INTERFACE)
target_sources(console-common-x11 INTERFACE
  x11-utils.c
  x11-utils.h
)
target_include_directories(console-common-x11 INTERFACE .)

# TODO(ah): should we list the headers?
# clipboard.h
# simics-terminal-argb-icon.h
# simics_icon.h

# TODO(ah): how to handle the icons?
# simics-terminal-16x16x32.png
# simics-terminal-32x32x32.png
# simics-terminal-48x48x32.png
# simics-terminal.bmp
# simics-terminal.ico
