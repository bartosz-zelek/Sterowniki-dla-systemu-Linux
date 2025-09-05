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

#ifndef SIMICS_BASE_NOTIFIER_H
#define SIMICS_BASE_NOTIFIER_H

#include <simics/base/types.h>
#include <simics/base/conf-object.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="device api types">
   <name>notifier_type_t</name>
   <insert id="notifier_type_t DOC"/>
   </add> */

/* <add id="notifier_type_t DOC">
   <ndx>notifier_type_t</ndx>
   <name index="true">notifier_type_t</name>
   <doc>
   <doc-item name="NAME">notifier_type_t</doc-item>
   <doc-item name="SYNOPSIS">
   <smaller>
   <insert id="notifier_type_t def"/>
   </smaller>
   </doc-item>
   <doc-item name="DESCRIPTION">
   Values of the <type>notifier_type_t</type> type identify notification events.
   A <type>notifier_type_t</type> value should be obtained from
   the <fun>SIM_notifier_type</fun> function and can then be used in other
   functions such as <fun>SIM_register_notifier</fun>,
   <fun>SIM_add_notifier</fun>, <fun>SIM_notify</fun>.

   A few notification events have predefined (constant) values. They are listed
   below where a notifier type in the form of a string (as accepted by
   <fun>SIM_notifier_type</fun>) is followed by the constant of the
   <type>notifier_type_t</type> type corresponding to the notification event
   (as returned by <fun>SIM_notifier_type</fun>):
   <ul>
   <li>"queue-change" (<var>Sim_Notify_Queue_Change</var>)</li>
   <li>"cell-change" (<var>Sim_Notify_Cell_Change</var>)</li>
   <li>"frequency-change" (<var>Sim_Notify_Frequency_Change</var>)</li>
   <li>"concurrency-change" (<var>Sim_Notify_Concurrency_Change</var>)</li>
   <li>"object-delete" (<var>Sim_Notify_Object_Delete</var>)</li>
   <li>"map-change" (<var>Sim_Notify_Map_Change</var>)</li>
   <li>"state-change" (<var>Sim_Notify_State_Change</var>)</li>
   <li>"freerunning-mode-change"
        (<var>Sim_Notify_Freerunning_Mode_Change</var>)</li>
   <li>"bank-register-value-change"
        (<var>Sim_Notify_Bank_Register_Value_Change</var>)</li>
   </ul>
   </doc-item>
   <doc-item name="SEE ALSO">
     <fun>SIM_notify</fun>,
     <fun>SIM_add_notifier</fun>,
     <fun>SIM_register_notifier</fun>,
     <fun>SIM_notifier_type</fun>,
     <fun>SIM_describe_notifier</fun>,
     <fun>SIM_notifier_description</fun>
   </doc-item>
   </doc>
   </add>

   <add-type id="notifier_type_t def"></add-type>
*/

/* Note that notifier types can be added either by modifying this enum or
   by using SIM_notifier_type. The latter is typically preferred since it does
   not change the Simics API. */
typedef enum {
        Sim_Notify_Queue_Change,
        Sim_Notify_Cell_Change,
        Sim_Notify_Frequency_Change,
        Sim_Notify_Concurrency_Change,
        Sim_Notify_Object_Delete,
        Sim_Notify_Map_Change,
        Sim_Notify_State_Change,
        Sim_Notify_Freerunning_Mode_Change,
        Sim_Notify_Bank_Register_Value_Change,
} notifier_type_t;

EXPORTED notifier_type_t SIM_notifier_type(
        const char *NOTNULL type);
        
/* Consumer API */

typedef struct notifier_handle notifier_handle_t;

EXPORTED notifier_handle_t *SIM_add_notifier(
        conf_object_t *NOTNULL obj,
        notifier_type_t what,
        conf_object_t *subscriber,
        void (*callback)(
                conf_object_t *subscriber,
                conf_object_t *NOTNULL notifier,
                lang_void *data),
        lang_void *data);

EXPORTED void SIM_delete_notifier(
        conf_object_t *NOTNULL obj,
        notifier_handle_t *handle);

EXPORTED bool SIM_has_notifier(
        conf_object_t *NOTNULL obj,
        notifier_type_t what);

EXPORTED bool SIM_class_has_notifier(
        conf_class_t *NOTNULL cls,
        notifier_type_t what);


/* Producer API */

EXPORTED void SIM_register_notifier(
        conf_class_t *NOTNULL cls,
        notifier_type_t what,
        const char *desc);

EXPORTED void SIM_register_tracked_notifier(
        conf_class_t *NOTNULL cls,
        notifier_type_t what,
        const char *desc,
        void (*subscribed_changed)(conf_object_t *obj, notifier_type_t type,
                                   bool has_subscribers));

EXPORTED void SIM_notify(
        conf_object_t *NOTNULL obj,
        notifier_type_t type);

EXPORTED void SIM_describe_notifier(
        notifier_type_t type,
        const char *NOTNULL generic_desc);

EXPORTED const char * SIM_notifier_description(
        notifier_type_t type);


#if defined(__cplusplus)
}
#endif

#endif
