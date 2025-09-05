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

#ifndef SIMICS_MODEL_IFACE_SAVE_STATE_H
#define SIMICS_MODEL_IFACE_SAVE_STATE_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* not actually used - kept for source compatibility */
typedef enum {
	Sim_State_Default = 1
} state_save_kind_t;

/* internal use only */
SIM_INTERFACE(save_state) {

        /* return the CVS version of the object */
        int (*get_version)(conf_object_t *obj);
        /* return the size needed for the current state */
        int (*get_size)(conf_object_t *obj, state_save_kind_t stype);
        /* return the alignment needed for the structure */
        int (*get_align)(conf_object_t *obj, state_save_kind_t stype);


        /* network-endian functions */

        /* save the state in dest. dest must contains enough space
           (get_size with the same parameter should return the size needed) */
        int (*save_state)(conf_object_t *obj, state_save_kind_t stype,
                          void *dest);
        /* load the state from src */
        int (*load_state)(conf_object_t *obj, state_save_kind_t stype,
                          void *src);
        /* compare the state and return 0 if the same */
        int (*compare_state)(conf_object_t *obj, state_save_kind_t stype,
                             void *src);
        /* compare the state and return 0 if the same. print out
           all/the difference according to flags */
        int (*print_state)(conf_object_t *obj, state_save_kind_t stype,
                           void *src, int symnames, int diff);

        /* host-endian functions (eventually faster, not mandatory) */
        int (*save_state_he)(conf_object_t *obj, state_save_kind_t stype,
                             void *dest);
        int (*load_state_he)(conf_object_t *obj, state_save_kind_t stype,
                             void *src);
        int (*compare_state_he)(conf_object_t *obj,
                                state_save_kind_t stype, void *src);
        int (*print_state_he)(conf_object_t *obj,
                              state_save_kind_t stype,
                              void *src, int symnames, int diff);
};

#define SAVE_STATE_INTERFACE "save_state"

#if defined(__cplusplus)
}
#endif

#endif
