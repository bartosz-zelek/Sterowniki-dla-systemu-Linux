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

#ifndef SIMICS_MODEL_IFACE_BRANCH_RECORDER_H
#define SIMICS_MODEL_IFACE_BRANCH_RECORDER_H

#include <simics/base/types.h>
#include <simics/base/attr-value.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="simics api types">
   <name index="true">addr_prof_iter_t</name>
   <insert id="addr_prof_iter_t DOC"/>
   </add> */

/* <add id="addr_prof_iter_t DOC">
     <ndx>addr_prof_iter_t</ndx>
     <name index="true">addr_prof_iter_t</name>
     <doc>
       <doc-item name="NAME">addr_prof_iter_t</doc-item>
       <doc-item name="SYNOPSIS">
         <insert id="addr_prof_iter_t def"/>
       </doc-item>
       <doc-item name="DESCRIPTION">
  
         An address profile iterator will iterate over a specified portion of the
         address space in some unspecified order, and return every nonzero counter
         value exactly once. When done, it will return 0.
  
       </doc-item>
     </doc>
  </add> */

/* <add-type id="addr_prof_iter_t def"></add-type> */
typedef struct addr_prof_iter {
        uint64 (*next)(struct addr_prof_iter *i);
        void (*destroy)(struct addr_prof_iter *i);
        generic_address_t addr;
} addr_prof_iter_t;

/*
  <add-type id="branch_arc_type_t def"></add-type> 
*/
typedef enum {
        Branch_Arc_Branch,
        Branch_Arc_Exception,
        Branch_Arc_Exception_Return,
        Branch_Arc_Max
} branch_arc_type_t;

typedef struct {
        uint64 addr_from;
        uint64 addr_to;
        int64 count;
        branch_arc_type_t type;
} branch_arc_t;

typedef struct branch_arc_iter {
        branch_arc_t *(*next)(struct branch_arc_iter *i);
        void (*destroy)(struct branch_arc_iter *i);
} branch_arc_iter_t; 

/*
  <add-type id="branch_recorder_direction_t def"></add-type> 
*/
typedef enum {
        BR_Direction_From,
        BR_Direction_To
} branch_recorder_direction_t;

typedef struct branch_recorder branch_recorder_t;

typedef addr_prof_iter_t *(*iter_func_t)(branch_recorder_t *br,
                                         generic_address_t start,
                                         generic_address_t stop);

/* Functions and stuff that depend(s) on the cpu type but not on a particular
   branch recorder instance. */
/*
   <add id="branch_recorder_handler_interface_t">
   An internal interface. Can be changed at any time.
   </add>

   <add id="branch_recorder_handler_interface_exec_context">
   Internal.
   </add>
*/
SIM_INTERFACE(branch_recorder_handler) {
        // Attach/detach returns zero on error.
        int (*PYTHON_METHOD attach_branch_recorder)(
                conf_object_t *cpu, conf_object_t *NOTNULL brec);
        int (*PYTHON_METHOD detach_branch_recorder)(
                conf_object_t *cpu, conf_object_t *NOTNULL brec);
#if !defined(PYWRAP)
        void (*create_cpu_branch_recorder)(branch_recorder_t *br);
        void (*destroy_cpu_branch_recorder)(branch_recorder_t *br);
        void (*delete_arcs)(branch_recorder_t *br);
        int (*num_branches)(branch_recorder_t *br);
        attr_value_t (*get_branches)(branch_recorder_t *br);
        iter_func_t *iter;
        branch_arc_iter_t *(*iter_branch_arc)(branch_recorder_t *br,
                                              generic_address_t start,
                                              generic_address_t stop,
                                              branch_recorder_direction_t dir);
        int (*eec_interrupt_instruction)(conf_object_t *cpu, int extype);
        unsigned granularity_log2;
        char const **view_descriptions;
        int num_views;
        int pa_bits;
        int va_bits;
#endif
};

#define BRANCH_RECORDER_HANDLER_INTERFACE "branch_recorder_handler"

#if defined(__cplusplus)
}
#endif

#endif
