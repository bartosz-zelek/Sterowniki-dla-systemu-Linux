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

#ifndef SIMICS_SIMULATOR_EMBED_H
#define SIMICS_SIMULATOR_EMBED_H

#include <simics/base-types.h>

#if defined _WIN32 && (defined PYWRAP || defined GULP_API_HELP)
 #include <winsock2.h>
 #include <windows.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef PYWRAP

/*
   <add id="device api types obsolete">
   <name index="true">init_prefs_t</name>
   <insert id="init_prefs_t DOC"/>
   </add> */

/* <add id="init_prefs_t DOC">
   <ndx>init_prefs_t</ndx>
   <doc>
   <doc-item name="NAME">init_prefs_t</doc-item>
   <doc-item name="SYNOPSIS">
   <small>
   <insert id="init_prefs_t def"/>
   </small>
   </doc-item>
   <doc-item name="DESCRIPTION">
   The <type>init_prefs_t</type> types are
   deprecated and should not be used in new code.
   </doc-item>
   </doc>
   </add>
*/

/* <add-type id="init_prefs_t def">
   </add-type> */
typedef struct {
        bool batch_mode;
        bool quiet;
        bool verbose;
        bool python_verbose;
        bool disable_istc;
        bool disable_dstc;
        bool module_cache_enable;
        bool rdp;
        bool sign_module;
        const char *log_file;

        /* The Simics project to use */
        const char *project;       // NULL to use saved prefs value

        const char *package_list;  // internal, do not use

        bool no_windows;
        bool fail_on_warnings;
        const char *deprecation_level; // see sim->warn_deprecated
        bool warn_deprecated;       // same as deprecation_level == 2
        bool no_warn_deprecated;    // same as deprecation_level == 0

        bool warn_multithread;  /* deprecated and ignored (bug 21597) */
        bool check_ifaces;
        bool no_global_settings;    // do not read preferences and recent-files

        /* the following should be -1 if not set by command line options
           to tell SIM_init_simulator() to use the saved preference value */
        int log_enable;
} init_prefs_t;

EXPORTED void SIM_init_environment(char **NOTNULL argv,
                                   bool handle_signals, bool allow_core_dumps);

/* <add id="device api types">
   <name index="true">init_arg_t</name>
   <insert id="init_arg_t DOC"/>
   </add> */

/* <add id="init_arg_t DOC">
     <ndx>init_arg_t</ndx>
     <name index="true">init_arg_t</name>
     <doc>
       <doc-item name="NAME">init_arg_t</doc-item>
       <doc-item name="SYNOPSIS">
         <insert id="init_arg_t def"/>
       </doc-item>
       <doc-item name="DESCRIPTION">
       Data structure used to pass an initialization argument to the
       <fun>SIM_init_simulator2</fun> function. The <tt>name</tt> field is
       mandatory and the associated data is either a boolean or a string
       (<tt>char *</tt>). A list of <type>init_arg_t</type> is passed to
       <fun>SIM_init_simulator2</fun> where the last entry has the
       <tt>name</tt> field set to <tt>NULL</tt>.
       </doc-item>
     </doc>
   </add> */

/* <add-type id="init_arg_t def"></add-type> */
typedef struct {
        const char *name;
        bool boolean;
        union {
                const char *string;
                bool enabled;
        } u;
} init_arg_t;

EXPORTED void SIM_init_simulator2(init_arg_t *NOTNULL init_args);

EXPORTED void SIM_set_frontend_context(void *context);

EXPORTED void VT_init_package_list(const char *package_list);

#endif /* !PYWRAP */

EXPORTED const char *SIM_get_init_arg_string(const char *NOTNULL name,
                                             const char *default_value);
EXPORTED bool SIM_get_init_arg_boolean(const char *NOTNULL name,
                                       bool default_value);

EXPORTED void SIM_init_command_line(void);

EXPORTED void SIM_main_loop(void);

EXPORTED void VT_check_package_updates(void);

#if defined(__cplusplus)
}
#endif

#endif
