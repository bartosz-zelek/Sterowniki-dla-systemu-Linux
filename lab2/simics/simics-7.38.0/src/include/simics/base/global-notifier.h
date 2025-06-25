/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_BASE_GLOBAL_NOTIFIER_H
#define SIMICS_BASE_GLOBAL_NOTIFIER_H

#include <simics/base/types.h>
#include <simics/base/conf-object.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="device api types">
   <name>global_notifier_type_t</name>
   <insert id="global_notifier_type_t DOC"/>
   </add> */

/* <add id="global_notifier_type_t DOC">
   <ndx>global_notifier_type_t</ndx>
   <name index="true">global_notifier_type_t</name>
   <doc>
   <doc-item name="NAME">global_notifier_type_t</doc-item>
   <doc-item name="SYNOPSIS">
   <smaller>
   <insert id="global_notifier_type_t def"/>
   </smaller>
   </doc-item>
   <doc-item name="DESCRIPTION">

   This enum is used to identify pre-defined global notifier. The
   <tt>Sim_Global_Notify_Object_Delete</tt> notifier is triggered by Simics
   when objects are being deleted. In the callback, objects are still fully
   available, but <fun>SIM_marked_for_deletion</fun> can be used to determine
   if an object is being deleted.

   The <tt>Sim_Global_Notify_Objects_Finalized</tt> notifier is triggered by
   Simics when new objects have been finalized, after their
   <tt>objects_finalized</tt> methods have been called.

   The <tt>Sim_Global_Notify_Message</tt> notifier is used by
   <fun>SIM_trigger_global_message</fun>.

   The corresponding names used in e.g. <cmd>list-notifiers</cmd>
   are as follows:
   <ul>
   <li>"global-object-delete" (<var>Sim_Global_Notify_Object_Delete</var>)</li>
   <li>"global-objects-finalized" (<var>Sim_Global_Notify_Objects_Finalized</var>)</li>
   <li>"global-message" (<var>Sim_Global_Notify_Message</var>)</li>
   </ul>

   </doc-item>
   <doc-item name="SEE ALSO">
     <fun>SIM_add_global_notifier</fun>,
     <fun>SIM_add_global_notifier_once</fun>,
     <fun>SIM_delete_global_notifier</fun>,
   </doc-item>
   </doc>
   </add>

   <add-type id="global_notifier_type_t def"></add-type>
*/
typedef enum {
        Sim_Global_Notify_Object_Delete = 100,
        Sim_Global_Notify_Objects_Finalized,
        Sim_Global_Notify_Message,

        Sim_Global_Notify_Before_Snapshot_Restore = 150,
        Sim_Global_Notify_After_Snapshot_Restore,
} global_notifier_type_t;

/* Consumer API */

typedef struct global_notifier_callback global_notifier_callback_t;

EXPORTED global_notifier_callback_t *SIM_add_global_notifier(
        global_notifier_type_t what,
        conf_object_t *subscriber,
        void (*NOTNULL callback)(
                conf_object_t *subscriber,
                lang_void *data),
        lang_void *data);

EXPORTED global_notifier_callback_t *SIM_add_global_notifier_once(
        global_notifier_type_t what,
        conf_object_t *subscriber,
        void (*NOTNULL callback)(
                conf_object_t *subscriber,
                lang_void *data),
        lang_void *data);

EXPORTED void SIM_delete_global_notifier(
        global_notifier_callback_t *NOTNULL handle);

#if defined(__cplusplus)
}
#endif

#endif
