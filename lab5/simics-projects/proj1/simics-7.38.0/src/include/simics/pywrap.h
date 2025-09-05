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

#ifndef SIMICS_PYWRAP_H
#define SIMICS_PYWRAP_H

#if defined(PYWRAP) && !defined(SIMICS_CORE)
 #define SIM_INTERFACE(name)                                            \
        typedef SIM_INTERFACE name ## _interface name ## _interface_t;  \
        SIM_INTERFACE name ## _interface
#else
 #define SIM_INTERFACE(name)                                    \
        typedef struct name ## _interface name ## _interface_t; \
        struct name ## _interface

 /* this is just a dummy expansion which will be ignored by the compiler */
 #define SIM_PY_ALLOCATABLE(typename) struct dummy_expansion_sim_py_allocatable
#endif

#endif
