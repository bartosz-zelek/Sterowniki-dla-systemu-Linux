/*
  sample-risc-cycle.h - Supporting the cycle queue

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SAMPLE_RISC_CYCLE_H
#define SAMPLE_RISC_CYCLE_H

#ifndef SAMPLE_RISC_HEADER
 #define SAMPLE_RISC_HEADER "sample-risc.h"
#endif
#include SAMPLE_RISC_HEADER

#if defined(__cplusplus)
extern "C" {
#endif

void instantiate_cycle_queue(sample_risc_t *sr);
void register_cycle_queue(conf_class_t *cls);

#if defined(__cplusplus)
}
#endif
        
#endif
