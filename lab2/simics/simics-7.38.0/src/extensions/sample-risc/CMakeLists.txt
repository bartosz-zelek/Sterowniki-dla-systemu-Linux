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

simics_add_module(sample-risc
  CLASSES sample-risc sample-risc-core
  SOURCES sample-risc.c sample-risc-core.c sample-risc-queue.c sample-risc-frequency.c sample-risc-step.c sample-risc-cycle.c sample-risc-memory.c sample-risc-exec.c event-queue.c toy-risc-core.c sample-risc-direct-memory.c module_load.py
  SIMICS_API latest
  INIT_LOCAL
)
target_link_libraries(sample-risc
  PRIVATE cycle-common  # cycle-common is a STATIC library in core
)
add_compile_definitions(SAMPLE_RISC_CPU_CLASSNAME=sample-risc SAMPLE_RISC_CORE_CLASSNAME=sample-risc-core )

add_library(sample-risc-shared STATIC event-queue.c)
target_link_libraries(sample-risc-shared PRIVATE Simics::includes)
add_library(sample-risc::shared ALIAS sample-risc-shared)

add_subdirectory(test)
