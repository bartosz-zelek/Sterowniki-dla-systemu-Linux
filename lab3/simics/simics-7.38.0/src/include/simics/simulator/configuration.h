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

#ifndef SIMICS_SIMULATOR_CONFIGURATION_H
#define SIMICS_SIMULATOR_CONFIGURATION_H

#include <simics/base/attr-value.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct pre_conf_object_set_t pre_conf_object_set_t;

EXPORTED void SIM_read_configuration(const char *NOTNULL file);

EXPORTED void SIM_set_configuration(attr_value_t conf);

EXPORTED void SIM_add_configuration(
        pre_conf_object_set_t *NOTNULL object_list, const char *file);

EXPORTED char *SIM_current_checkpoint_dir(void);

EXPORTED attr_value_t VT_add_objects(pre_conf_object_set_t *set);

typedef enum {
#if defined(SIMICS_6_API)
        Sim_Save_Gzip_Config = 4,  /* create gzipped config file */
#endif
        Sim_Save_Nobundle = 1,  /* old-style save without creating directory */
        Sim_Save_RLE_Data = 2,  /* use RLE for data values */
#if !defined(SIMICS_6_API)
        Sim_Save_Image_Separator = 8, /* internal */
        Sim_Save_No_Gzip_Config = 16,
        Sim_Save_Image_Uncompressed_Craff = 32,
        Sim_Save_Image_Raw = 64,
        Sim_Save_Image_VHDX = 128,
        Sim_Save_Standalone_Checkpoint = 256
#endif
} save_flags_t;

EXPORTED void SIM_write_configuration_to_file(
        const char *NOTNULL file, save_flags_t flags);

EXPORTED void SIM_write_persistent_state(
        const char *NOTNULL file, conf_object_t *root, save_flags_t flags);

EXPORTED pre_conf_object_set_t *VT_get_configuration(const char *NOTNULL file);

#if defined(__cplusplus)
}
#endif

#endif
