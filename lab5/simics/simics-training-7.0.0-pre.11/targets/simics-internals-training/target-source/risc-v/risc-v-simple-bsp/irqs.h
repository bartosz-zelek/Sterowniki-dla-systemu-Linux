/*
  © 2024 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef __RISC_V_IRQS_H__
#define __RISC_V_IRQS_H__

typedef void (*irq_handler_t)();
void register_ext_irq(unsigned int id, irq_handler_t handler);

#endif

