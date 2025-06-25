/*
  gc-interface.c

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <simics/simulator-api.h>

#include "gc-common.h"
#include "gc.h"

/*
  Return 1 is the transaction is uncacheable.
*/
int
is_uncacheable(generic_cache_t *gc, generic_transaction_t *mem_op, 
               map_list_t *map, conf_object_t *o)
{
        switch(SIM_get_mem_op_ini_type(mem_op) & 0xFF00) {

#ifdef GC_DEBUG
                /* this shouldn't happen */
        case Sim_Initiator_Illegal:
                SIM_LOG_ERROR(o, GC_Log_Cache,
                              "is_uncacheable: Illegal initiator "
                              "type for memory transaction");
                break;
#endif

        case Sim_Initiator_CPU_PPC:
        {
                ppc_memory_transaction_t *mt =
                        (ppc_memory_transaction_t *)mem_op;
                return mt->wimg & 0x4; /* the I (cache inhibit) bit */
        }

        case Sim_Initiator_CPU_X86:
        {
                x86_memory_transaction_t *mt =
                        (x86_memory_transaction_t *) mem_op;
                if (mt->effective_type == X86_Write_Back)
                        return 0;
                else
                        return 1;
                break;
        }

        case Sim_Initiator_CPU_MIPS: 
        {
                mips_memory_transaction_t *mt = 
                        (mips_memory_transaction_t *) mem_op;

                switch (mt->cache_coherency) {
                case 2: /* Uncached, blocking. */
                case 6: /* Uncached, non-blocking. */
                        return 1;
                case 0: /* Writethrough without write-allocate,
                           non-blocking. */
                case 1: /* Writethrough with write-allocate,
                           non-blocking. */
                case 3: /* Writeback, non-blocking. */
                case 4: /* Coherent writeback, exclusive allocate */
                case 5: /* Coherent writeback */
                case 7: /* Bypass, non-blocking. */
                        return 0;
                }
                SIM_LOG_ERROR(o, GC_Log_Cache,
                              "Unknown cache coherency "
                              "value: %d", mt->cache_coherency);
                return 1;
        }

        case Sim_Initiator_Cache:
        {
                return 0;
        }

        case Sim_Initiator_Device:
        {
                if (gc->config.cache_device_accesses) {
                        for (unsigned cd = 0;
                             cd < gc->cacheable_devices_size;
                             cd++) {
                                if (SIM_get_mem_op_initiator(mem_op)
                                    == gc->cacheable_devices[cd])
                                        return 0;
                        }
                }
                /* device transaction is uncacheable */
                return 1;
        }

        default:
                if ((SIM_get_mem_op_ini_type(mem_op) & 0xF000)
                    == Sim_Initiator_CPU)
                        return 0; /* unknown cpu transaction are cacheable */
                else
                        return 1; /* unknown transaction */
        }

        /* error is uncacheable */
        return 1;
}
