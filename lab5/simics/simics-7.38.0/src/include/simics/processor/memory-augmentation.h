/*
  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_PROCESSOR_MEMORY_AUGMENTATION_H
#define SIMICS_PROCESSOR_MEMORY_AUGMENTATION_H

#include <simics/base/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef PYWRAP

#define TAG_GRANULARITY 4     /* log2(memory bytes per tag) */
#define TAG_BYTE_PACKING 3    /* log2(how many tags are packed in each byte) */

/* <add-fun id="simics api processor">
     <short>convenience functions to work with augmented memory bits</short>

     The <fun>SIM_clear_augmentation_bit</fun>,
     <fun>SIM_get_augmentation_bit</fun>,
     and <fun>SIM_set_augmentation_bit</fun> functions are used to modify
     and read augmented memory bits. The <iface>direct_memory_tags</iface>
     interface supports augmented memory through the <tt>data</tt>
     field in the <tt>direct_memory_tags_t</tt> struct.

     <di name="EXECUTION CONTEXT">Cell Context</di>
   </add-fun> */
FORCE_INLINE void
SIM_clear_augmentation_bit(uint8 *tag_page_data, unsigned pofs)
{
        /* clear the tag bit corresponding to page offset pofs */
        unsigned k = pofs >> TAG_GRANULARITY;
        tag_page_data[k >> 3] &= (uint8)~(1 << (k & 7));
}

/* <append-fun id="SIM_clear_augmentation_bit"/> */
FORCE_INLINE int
SIM_get_augmentation_bit(uint8 *tag_page_data, unsigned pofs)
{
        unsigned k = pofs >> TAG_GRANULARITY;
        return (tag_page_data[k >> 3] >> (k & 7)) & 1;
}

/* <append-fun id="SIM_clear_augmentation_bit"/> */
FORCE_INLINE void
SIM_set_augmentation_bit(uint8 *tag_page_data, unsigned pofs)
{
        unsigned k = pofs >> TAG_GRANULARITY;
        int bit_pos = k & 7;

        tag_page_data[k >> 3] = (tag_page_data[k >> 3] & ~(1 << bit_pos))
                                | 1 << bit_pos;
}

/* aliases kept for binary compatibility; to be removed in the next version */
EXPORTED void VT_clear_augmentation_bit(uint8 *page_data, unsigned pofs);
EXPORTED int VT_get_augmentation_bit(uint8 *page_data, unsigned pofs);
EXPORTED void VT_set_augmentation_bit(uint8 *page_data, unsigned pofs);

#endif /* !PYWRAP */

#if defined(__cplusplus)
}
#endif

#endif
