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

#ifndef SIMICS_SIMULATOR_SNAPSHOTS_H
#define SIMICS_SIMULATOR_SNAPSHOTS_H

#include <simics/base/types.h>
#include <simics/base/attr-value.h>
#include <simics/base/conf-object.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum {
        /* Nothing went wrong. */
        Snapshot_Error_No_Error = 0,
        /* No snapshot name given. */
        Snapshot_Error_No_Name,
        /* No snapshot with the given name found. */
        Snapshot_Error_Snapshot_Not_Found,
        /* A snapshot with the given name already exists. */
        Snapshot_Error_Snapshot_Already_Exists,
        /* The current configuration can not be saved as a snapshot. */
        Snapshot_Error_Illegal_Configuration,
        /* Something unexpected went wrong.
           Partial state may have been restored. */
        Snapshot_Error_Internal_Error, 
} snapshot_error_t;

EXPORTED bool SIM_is_restoring_snapshot(void);

EXPORTED snapshot_error_t SIM_take_snapshot(const char *NOTNULL name);
EXPORTED snapshot_error_t SIM_restore_snapshot(const char *NOTNULL name);
EXPORTED snapshot_error_t SIM_delete_snapshot(const char *NOTNULL name);

EXPORTED attr_value_t SIM_list_snapshots(void);
EXPORTED attr_value_t SIM_get_snapshot_info(const char *NOTNULL name);

/* Used to prevent snapshots from touching certain classes/attributes */
EXPORTED void VT_snapshots_ignore_class(const char *NOTNULL class_name);
/* Used to prevent snapshots from trying to restore certain classes and
   attributes. This is only to work-around broken classes. */
EXPORTED void VT_snapshots_skip_class_restore(conf_class_t *NOTNULL cls);
EXPORTED void VT_snapshots_skip_attr_restore(conf_class_t *NOTNULL cls,
                                             const char *NOTNULL attr_name);
/* Dump the named snapshot as a big attr_value_t */
EXPORTED attr_value_t VT_dump_snapshot(const char *NOTNULL name);
/* How much data is used by the snapshot system to store attributes and pages */
EXPORTED attr_value_t VT_snapshot_size_used(void);

#if defined(__cplusplus)
}
#endif

#endif
