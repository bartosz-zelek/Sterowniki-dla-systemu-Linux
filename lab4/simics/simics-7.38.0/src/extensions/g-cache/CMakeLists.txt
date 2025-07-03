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

simics_add_module(g-cache
  CLASSES g-cache trans-splitter trans-sorter
  SOURCES gc-attributes.c gc-common-attributes.c gc-common.c gc-cyclic-repl.c gc-interface.c gc-lru-repl.c gc-random-repl.c gc.c sorter.c splitter.c gcache_common.py module_load.py simics_start.py
  INIT_LOCAL
  SIMICS_API latest
)
target_link_libraries(g-cache
  PRIVATE scalar-time-common
)
add_compile_definitions(GC_USE_STC)
add_subdirectory(test)

# TODO(ah): fail!! We dont' want this, so why do we need it in CMake?
target_compile_options(g-cache PRIVATE -Wno-array-bounds)
