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

#ifndef SIMICS_MODEL_IFACE_CPU_GROUP_H
#define SIMICS_MODEL_IFACE_CPU_GROUP_H

#include <simics/base/types.h>
#include <simics/util/vect.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* A class implementing the cpu_group interface should also have
   cpu_list attribute */
#define CPU_GROUP_INTERFACE "cpu_group"
typedef VECT(conf_object_t *) cpu_list_t;
SIM_INTERFACE(cpu_group) {
        const cpu_list_t *(*get_cpu_list)(conf_object_t *cpu_group);
};

#if defined(__cplusplus)
}
#endif

#endif
