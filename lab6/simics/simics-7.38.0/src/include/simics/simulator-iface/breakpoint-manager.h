/*
  © 2011 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SIMULATOR_IFACE_BREAKPOINT_MANAGER_H
#define SIMICS_SIMULATOR_IFACE_BREAKPOINT_MANAGER_H

#include <simics/device-api.h>
#include <simics/base/cbdata.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* These properties have known semantics and structure. Each breakpoint should
   have them. A breakpoint is free to provide any additional properties it
   desires. */

/* Is the breakpoint enabled or not. A boolean. If false the breakpoint will
   never hit. */
#define BREAKPOINT_PROP_ENABLED "enabled"
/* Should the breakpoint be removed after it is first triggered. A boolean. */
#define BREAKPOINT_PROP_TEMPORARY "temporary"
/* For each instance of this breakpoint, ignore all hits until hit count >=
   ignore count. Ignore count is an unsigned integer. */
#define BREAKPOINT_PROP_IGNORE_COUNT "ignore count"
/* Hit count is a dictionary mapping from breakpoint instance name to number of
   times this instance has hit. */
#define BREAKPOINT_PROP_HIT_COUNT "hit count"
/* A short string describing the breakpoint. */
#define BREAKPOINT_PROP_DESCRIPTION "description"

SIM_INTERFACE(breakpoint_registration) {
        uint64 (*register_breakpoint)(
                conf_object_t *NOTNULL mgr,
                void (*delete_breakpoint)(cbdata_call_t data, uint64 id),
                cbdata_register_t delete_data,
                attr_value_t (*get_properties)(cbdata_call_t data, uint64 id),
                cbdata_register_t get_properties_data,
                void (*set_enabled)(cbdata_call_t data, uint64 id,
                                    bool enabled),
                cbdata_register_t set_enabled_data,
                void (*set_temporary)(cbdata_call_t data, uint64 id,
                                      bool temporary),
                cbdata_register_t set_temporary_data,
                void (*set_ignore_count)(cbdata_call_t data, uint64 id,
                                       uint64 ignore_count),
                cbdata_register_t set_ignore_count_data);
        void (*deleted)(conf_object_t *NOTNULL mgr, uint64 id);
};
#define BREAKPOINT_REGISTRATION_INTERFACE "breakpoint_registration"

/*
   <add id="breakpoint_type_interface_t">

   The <iface>breakpoint_type</iface> interface is implemented by the
   breakpoint manager and facilitates registering breakpoint types.

   This interface is currently a tech preview and can be changed at any time.

   The <fun>register_type</fun> method registers a breakpoint type and creates
   CLI commands for this type. The <arg>name</arg> parameter should be the
   unique name for the type, which is also used in the names of any commands
   registered on interfaces or class. The <arg>provider</arg> parameter should
   be the provider object, which must implement the
   <iface>breakpoint_type_provider</iface> interface.

   The registered commands are <cmd>break</cmd>, <cmd>run-until</cmd>,
   <cmd>wait-for</cmd>, <cmd>trace</cmd> and <cmd>untrace</cmd>, which are
   registered on the provider class. If <arg>cls</arg> or <arg>iface</arg> are
   non-NULL, commands are also registered on that class or interface. These
   commands are <cmd>bp-break-{name}</cmd>, <cmd>bp-run-until-{name}</cmd>,
   <cmd>bp-wait-for-{name}</cmd>, <cmd>bp-trace-{name}</cmd> and
   <cmd>bp-untrace-{name}</cmd>, where {name} is the <arg>name</arg> parameter.

   The <arg>cls</arg> and <arg>iface</arg> parameters cannot both be non-NULL.

   The <arg>args</arg> parameter is a list that defines the CLI command
   arguments. It should be a list of lists where each inner list contains the
   name of the cli.Arg_type subclass, such as <tt>str_t</tt>, <tt>flag_t</tt>
   etc, and then all parameters to its constructor (including values for
   parameters that have default values).

   The <arg>docs</arg> parameter should be an 8-element list with short and
   long command documentation texts for the four commands <cmd>break</cmd>,
   <cmd>run-until</cmd>, <cmd>wait-for</cmd> and <cmd>trace</cmd>.

   The <arg>object_required</arg> parameter indicates if the commands
   registered on the provider class (as opposed to the commands on the class or
   interface) should require an object (or if the command can use a default
   object). Note that there will only be an object parameter if <arg>cls</arg>
   or <arg>iface</arg> are non-NULL.

   The <arg>temporary_default</arg> parameter indicates if the breakpoints of
   this type should be temporary by default. This also results in the generated
   break command not having a -once flag.

   Setting <arg>recursive</arg> to true will append a '-recursive' flag to both
   global and object commands, and for the global command, the 'object'
   argument will have no specified class or iface.

   The <fun>trigger</fun> method must be called by the provider every time a
   breakpoint triggers, both breakpoints added with <fun>register_bp</fun>
   method and those added with <fun>add_bp</fun> method of the
   <iface>breakpoint_type_provider</iface> interface.

   <insert-until text="// ADD INTERFACE breakpoint_type_interface_t"/>
   </add>

   <add id="breakpoint_type_interface_exec_context"> Global Context
   </add>
*/

SIM_INTERFACE(breakpoint_type) {
        /* Returns false if type is already registered. */
        bool (*register_type)(conf_object_t *NOTNULL mgr,
                              const char *NOTNULL name,
                              conf_object_t *NOTNULL provider,
                              /* CLI command arguments as a list */
                              attr_value_t args,
                              /* Class and/or iface for command */
                              const char *cls,
                              const char *iface,
                              /* 8-element list with short and long
                                 command help texts */
                              attr_value_t docs,
                              /* Is an object required
                                 (or can it default to something) */
                              bool object_required,
                              /* Should breakpoints be temporary by default? */
                              bool temporary_default,
                              /* Set to true to allow all objects in 'object' argument */
                              bool recursive);
        /* Notify manager that breakpoint has triggered, given ID returned by
           add_bp. Returns true if simulation was stopped or otherwise the
           breakpoint action happened. Optionally provide trigger object and
           message, used if the simulation is stopped. */
        bool (*trigger)(conf_object_t *NOTNULL mgr,
                        conf_object_t *NOTNULL provider, uint64 bp_id,
                        conf_object_t *trigger, const char *msg);
        /* Return provider ID from breakpoint manager ID. */
        uint64 (*get_break_id)(conf_object_t *NOTNULL mgr, uint64 bm_id);
        uint64 (*get_manager_id)(conf_object_t *NOTNULL mgr,
                                 conf_object_t *NOTNULL provider, uint64 bp_id);
};
#define BREAKPOINT_TYPE_INTERFACE "breakpoint_type"
// ADD INTERFACE breakpoint_type_interface_t

/* Sent as flags to breakpoint_type_provider.add_bp, for providers that need
   to know from which command the breakpoint comes. */
typedef enum {
        Breakpoint_Type_Default,
        Breakpoint_Type_Break,
        Breakpoint_Type_Run_Until,
        Breakpoint_Type_Wait_For,
        Breakpoint_Type_Trace,
} sim_bp_type_t;

/*
   <add id="breakpoint_type_provider_interface_t">

   The <iface>breakpoint_type_provider</iface> interface should be implemented
   by objects that wish to act as breakpoint type providers towards the
   breakpoint manager. An object that is passed to the breakpoint manager via
   the <fun>register_type</fun> method in the <iface>breakpoint_type</iface>
   interface must implement <iface>breakpoint_type_provider</iface>.

   This interface is currently a tech preview and can be changed at any time.

   The <fun>register_bp</fun> and <fun>add_bp</fun> methods receives the
   command arguments, corresponding to what the breakpoint manager received in
   the <fun>register_type</fun>. The <fun>register_bp</fun> method should set
   up a breakpoint, register it with the breakpoint manager via the
   <iface>breakpoint_registration</iface> interface, and return the breakpoint
   manager ID for that breakpoint. The <fun>add_bp</fun> method should set up
   an internal breakpoint, which is not registered with the breakpoint manager,
   and return a provider-specific ID for this breakpoint. The breakpoint
   manager will use this ID to remove the breakpoint via the
   <fun>remove_bp</fun> method, and this ID should also be used when calling
   the <fun>trigger</fun> method in the <iface>breakpoint_type</iface>
   interface.

   The <fun>register_bp</fun> and <fun>add_bp</fun> methods should return 0 to
   indicate an error in setting up the breakpoint.

   The <fun>break_msg</fun> method should return the message that should be
   printed by the <cmd>bp.break</cmd> command after the breakpoint is set
   up. It receives the breakpoint manager ID for the breakpoint.

   The <fun>trace_msg</fun> method should return the message that should be
   printed when an (internal) trace breakpoint has hit. It receives the
   provider specific ID for the breakpoint.

   The <fun>wait_msg</fun> method should return the message that is attached to
   the script branch while waiting for a breakpoint to hit (displayed by
   e.g. <cmd>list-script-branches</cmd>). It receives the provider specific ID
   for the breakpoint.

   The optional method <fun>break_data</fun> can be implemented to make the
   <cmd>wait-for</cmd> and <cmd>run-until</cmd> commands return something. It
   receives the provider specific ID for the breakpoint.

   The method <fun>values</fun> must be implemented if the provider has
   specified that CLI command expanders should be used, when registering the
   breakpoint type. Otherwise the method is not called by the breakpoint
   manager. It should return the possible values for the command argument
   <arg>arg</arg>, which will be one of the argument names used when
   registering the type. The parameter <arg>prev_args</arg> will be the list of
   preceding argument values.

   The <fun>deleted</fun> method is typically optional. If implemented, it is
   called by the <fun>deleted</fun> function of
   the <iface>breakpoint_registration</iface> interface. Normally,
   breakpoint manager
   registered breakpoints are deleted using the function that was given
   to the <fun>register_breakpoint</fun> method of the
   <iface>breakpoint_registration</iface> interface, which is used by the
   <cmd>bp.delete</cmd>, but if the breakpoint can be removed by other means,
   then this method can be implemented.

   <insert-until text="// ADD INTERFACE breakpoint_type_provider_interface_t"/>
   </add>

   <add id="breakpoint_type_provider_interface_exec_context">
   Global Context
   </add>
*/

SIM_INTERFACE(breakpoint_type_provider) {
        /* Register breakpoint in manager.
           Return breakpoint manager ID, or 0 on error. */
        uint64 (*register_bp)(conf_object_t *NOTNULL obj, uint64 bp_id);
        /* Add breakpoint and return provider specific ID, or 0 on error. */
        uint64 (*add_bp)(conf_object_t *NOTNULL obj,
                         int flags, attr_value_t data);
        /* Remove breakpoint, given ID returned by add_bp. */
        void (*remove_bp)(conf_object_t *NOTNULL obj, uint64 bp_id);
        /* Return trace message, given ID returned by add_bp. */
        char *(*trace_msg)(conf_object_t *NOTNULL obj, uint64 bp_id);
        /* Message returned by break command, given ID returned by add_bp. */
        char *(*break_msg)(conf_object_t *NOTNULL obj, uint64 bp_id);
        /* Script branch wait message, given ID returned by add_bp. */
        char *(*wait_msg)(conf_object_t *NOTNULL obj, uint64 bp_id);
        /* Optional return value from wait-for and run-until commands,
           given ID returned by add_bp. */
        attr_value_t (*break_data)(conf_object_t *NOTNULL obj, uint64 bp_id);
        /* Return possible values for command argument.
           Optional unless expanders used. */
        attr_value_t (*values)(conf_object_t *NOTNULL obj,
                               const char *arg, attr_value_t prev_args);
        /* Optional trace output function. The default is to log on the
           provider with level 1 and group 0. */
        void (*trace)(conf_object_t *NOTNULL obj, const char *msg);
};
#define BREAKPOINT_TYPE_PROVIDER_INTERFACE "breakpoint_type_provider"
// ADD INTERFACE breakpoint_type_provider_interface_t

/*
   <add id="breakpoint_manager_interface_t">
   An internal interface used by the breakpoint manager. Can be changed
   at any time.
   </add>

   <add id="breakpoint_manager_interface_exec_context">
   Internal.
   </add>
*/
SIM_INTERFACE(breakpoint_manager) {
        /* Return a list of the breakpoint ids of all breakpoints. */
        attr_value_t (*list_breakpoints)(conf_object_t *NOTNULL mgr);
        void (*delete_breakpoint)(conf_object_t *NOTNULL mgr, uint64 id);
        /* Return a dictionary with the properties for a specific breakpoint */
        attr_value_t (*get_properties)(conf_object_t *NOTNULL mgr, uint64 id);
        bool (*set_enabled)(conf_object_t *NOTNULL mgr,
                            uint64 id, bool enabled);
        bool (*set_temporary)(conf_object_t *NOTNULL mgr, uint64 id,
                              bool temporary);
        bool (*set_ignore_count)(conf_object_t *NOTNULL mgr, uint64 id,
                                 uint64 ignore_count);
};
#define BREAKPOINT_MANAGER_INTERFACE "breakpoint_manager"
// ADD INTERFACE breakpoint_manager

#if defined(__cplusplus)
}
#endif

#endif /* SIMICS_SIMULATOR_IFACE_BREAKPOINT_MANAGER_H */
