/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SIMULATOR_IFACE_DEBUGGER_H
#define SIMICS_SIMULATOR_IFACE_DEBUGGER_H

#include <simics/device-api.h>
#include <simics/pywrap.h>
#include <simics/base/cbdata.h>
#include <simics/processor/types.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef uint64 debug_cancel_id_t;
/* <add id="debugger_error_t">
<insert-until text="// END debugger_error_t"/>
</add> */
typedef enum {
        Debugger_No_Error = 0,
        Debugger_Not_Allowed_In_Execution_Context,
        Debugger_Unknown_Context,
        Debugger_Not_Supported_For_Context,
        Debugger_Context_Does_Not_Have_State,
        Debugger_Context_Is_Not_Active,
        Debugger_Lookup_Failure,
        Debugger_Failed_To_Get_Stack_Frame,
        Debugger_Failed_To_Get_PC,
        Debugger_Failed_To_Read,
        Debugger_Failed_To_Write,
        Debugger_Frame_Outside_Of_Known_Stack,
        Debugger_Failed_To_Evaluate_Expression,
        Debugger_Incorrect_Type,
        Debugger_Incorrect_Size,
        Debugger_Incorrect_Context_Query,
        Debugger_Unknown_Id,
        Debugger_Source_Not_Found,
        Debugger_File_Not_Found,
        Debugger_Unrecognized_File_Format,
        Debugger_Unsupported_For_File_Format,
        Debugger_Failed_To_Open_File,
        Debugger_Not_Relocatable,
        Debugger_Segment_Info_Missing,
        Debugger_Section_Info_Missing,
        Debugger_Segment_Not_Found,
        Debugger_Section_Not_Found,
        Debugger_Already_Running,
        Debugger_Failed_To_Continue,
        Debugger_No_Context_For_Object,
        Debugger_Invalid_Path,
        Debugger_Missing_Object,
        Debugger_Unexpected_Error,
        Debugger_Step_Interrupted,
} debugger_error_t;
// END debugger_error_t

/* <add id="debug_attr_value_return_type">

   For all functions that return an <type>attr_value_t</type>, that return value
   will consists of a list with two elements. The first element is an error code
   of <type>debugger_error_t</type> type (see <iface>debug_query</iface>
   interface documentation for definition). The second element depends on the
   first. If the first element is <type>Debugger_No_Error</type>, meaning that
   the function went well, then the second element will contain the expected
   return value that is specified per function below. If the first element is
   another error code, then the second element will be a string describing the
   error that occurred.

   </add> */

/* <add id="debug_notification_interface_t">
   <name>debug_notification_interface_t</name>
   <ndx>debug_notification_interface_t</ndx>

   This interface is used to get notifications from events in the
   debugger. Examples of events are when certain functions, addresses or code
   lines are hit.

   In order to be able to get notifications for symbols, the symbol file
   containing the debug information must have been added using the
   <iface>debug_setup</iface> interface or in some other way.

   All notifications take callback functions that are called when the debugger
   event occurs. The notifications will not stop the simulation, to do so
   <fun>SIM_break_simulation</fun> can be called in the callback.

   <insert id="debug_attr_value_return_type"/>

   <ndx>notify_context_creation!debug_notification interface method</ndx>
   <fun>notify_context_creation</fun> provides a callback when a new context
   that matches the context query <arg>query</arg> is created or renamed. The
   callback will also be triggered when a context is renamed, so if the context
   query matches both the name before and after the context was renamed then
   there will be two creation callbacks for the same context. When the callback
   is triggered because of a rename the <arg>updated</arg> argument of the
   callback will be true, otherwise if it is triggered because a new context
   was created the <arg>updated</arg> argument will be false.

   <fun>notify_context_destruction</fun> provides a callback when a context
   that matches the context query <arg>query</arg> is destroyed.

   The callbacks for <fun>notify_context_creation</fun> and
   <fun>notify_context_destruction</fun> will contain an ID to the context that
   was created, updated or destroyed, <arg>ctx_id</arg>, the tcf_agent object
   <arg>obj</arg> and some custom <arg>data</arg>.

   <note>The <fun>notify_context_creation</fun> and
   <fun>notify_context_destruction</fun> callbacks will only trigger for
   contexts that have state, this is most commonly the leaf nodes in an OS
   Awareness node tree.</note>

   <fun>notify_location</fun> will give a callback when a memory access of type
   <arg>access</arg> is done at the address specified by <arg>location</arg>,
   for contexts matching <arg>query</arg>. The <arg>size</arg> argument is used
   to specify the width of the symbol provided by <arg>location</arg>. For
   execution notifications a <arg>size</arg> of 1 is usually used. The maximum
   value of <arg>size</arg> is 0x7fffffff. Notifying on location will only work
   for global symbols. For symbols that are not global, no errors will be given,
   and no callback will be triggered.

   <fun>notify_address</fun> will provide a callback when an <arg>address</arg>
   is hit with a certain <arg>access</arg> type for contexts matching
   <arg>query</arg>. The <arg>size</arg> argument specifies the width of the
   breakpoint for the notification, with a maximum value of 0x7fffffff.
   The notification can be set to notify on <arg>physical</arg> breakpoints
   instead of virtual, but for that to work a processor or memory space context
   must be covered by the <arg>query</arg>. For process related contexts
   <arg>physical</arg> should be false.

   <fun>notify_line</fun> will provide a callback when a specific
   <arg>line</arg> and <arg>column</arg> in a specific source <arg>file</arg>
   for a context matching <arg>query</arg> is executed. The <arg>column</arg>
   argument can be set to 0 to not care about column.

   The notification functions <fun>notify_location</fun>,
   <fun>notify_address</fun> and <fun>notify_line</fun> will all provide
   callbacks on the same format. They will pass the context ID,
   <arg>ctx_id</arg>, for which the access occurred. A processor,
   <arg>cpu</arg>, which did the access is provided. The
   <arg>instruction_address</arg> is the address of the instruction that
   performed the access. For execution callbacks the callback will occur before
   the instruction has run, but for read or write accesses the callback will
   occur after the instruction has run. The <arg>data_address</arg> will
   provide which data address was accesses, for execution accesses this is the
   same as <arg>instruction_address</arg> but for read or write accesses this
   is where the actual access was. And <arg>size</arg> specifies the actual
   size of the access that was made to trigger the notification, for execution
   this size is 1.

   <fun>notify_activated</fun> and <fun>notify_deactivated</fun> are used to
   notify when a context, that matches the <arg>query</arg>, gets activated or
   deactivated. The callback for this will include which context,
   <arg>ctx_id</arg>, was (de)activated and on what processor, <arg>cpu</arg>.

   For all notifications functions in this interface, on success, the returned
   value of a notification function will be a cancel ID which can be used to
   cancel the notification.

   If several callbacks occur on the same cycle, then the order for which the
   callbacks are called is not determined. This means that a
   <fun>notify_activated</fun> callback for one processor can occur before a
   <fun>notify_deactivated</fun> callback on the same processor.

   <fun>notify_callbacks_done</fun> will be called once all other callbacks
   that happen because of the same event are done. For example when a move to
   and a move from is done in the same step then this callback can be used to
   keep grouped notifications together that occurred at the same time. This
   will always be called after one or more callbacks have been called.

   <fun>cancel</fun> is used to cancel a notification by providing it with a
   cancel ID, <arg>cid</arg>, that was returned from the notification function.
   When this goes well the returned value will be nil.

   Errors specific to this function: <ul>

   <li><em>Debugger_Unknown_Id</em> - The cancel ID, <arg>cid</arg>, is
   unknown.</li>

   </ul>

   All callbacks except <fun>notify_location</fun>, <fun>notify_address</fun>,
   and <fun>notify_line</fun>, with execution access, will occur after the
   instruction triggering the callbacks has executed. For the callbacks
   specified here, when using execution access, the callback will occur before
   the instruction at that location, address or line has executed.

   <note>In order to get contexts for processes, OS Awareness with a properly
   configured tracker has to exist. A context will then be created for each OS
   Awareness node. Without any OS Awareness tracker enabled for the system,
   contexts will only be available for processors and some memory spaces. The
   <fun>notify_activated</fun> and <fun>notify_deactivated</fun> functions will
   only give callbacks when a tracker is used, because processor contexts are
   always active. More information about configuring and using OS Awareness and
   trackers can be found in the <cite>Analyzer User's Guide</cite></note>

   <note>For functions that take <arg>query</arg> as argument, having this set
   to nil will work the same way as for <tt>"*"</tt>. A bad context query will
   result in a <type>Debugger_Incorrect_Context_Query</type> error.</note>

   <insert-until text="// ADD INTERFACE debug_notification_interface"/>
   </add>

   <add id="debug_notification_interface_exec_context">
   Global Context for all methods.
   </add>
*/

SIM_INTERFACE(debug_notification) {
        attr_value_t (*notify_context_creation)(
                conf_object_t *NOTNULL obj, const char *query,
                void (*cb)(cbdata_call_t data, conf_object_t *obj,
                           const char *ctx_id, bool updated),
                cbdata_register_t data);
        attr_value_t (*notify_context_destruction)(
                conf_object_t *NOTNULL obj, const char *query,
                void (*cb)(cbdata_call_t data, conf_object_t *obj,
                           const char *ctx_id), cbdata_register_t data);
        attr_value_t (*notify_location)(
                conf_object_t *NOTNULL obj, const char *query,
                const char *NOTNULL location, unsigned size, access_t access,
                void (*cb)(cbdata_call_t data, conf_object_t *obj,
                           const char *ctx_id, conf_object_t *cpu,
                           uint64 instruction_address, uint64 data_address,
                           unsigned size), cbdata_register_t data);
        attr_value_t (*notify_address)(
                conf_object_t *NOTNULL obj, const char *query, uint64 address,
                unsigned size, access_t access, bool physical,
                void (*cb)(cbdata_call_t data, conf_object_t *obj,
                           const char *ctx_id, conf_object_t *cpu,
                           uint64 instruction_address, uint64 data_address,
                           unsigned size), cbdata_register_t data);
        attr_value_t (*notify_line)(
                conf_object_t *NOTNULL obj, const char *query,
                const char *NOTNULL file, unsigned line, unsigned column,
                void (*cb)(cbdata_call_t data, conf_object_t *obj,
                           const char *ctx_id, conf_object_t *cpu,
                           uint64 instruction_address, uint64 data_address,
                           unsigned size), cbdata_register_t data);
        attr_value_t (*notify_activated)(
                conf_object_t *NOTNULL obj, const char *query,
                void (*cb)(cbdata_call_t data, conf_object_t *obj,
                           const char *ctx_id, conf_object_t *cpu),
                cbdata_register_t data);
        attr_value_t (*notify_deactivated)(
                conf_object_t *NOTNULL obj, const char *query,
                void (*cb)(cbdata_call_t data, conf_object_t *obj,
                           const char *ctx_id, conf_object_t *cpu),
                cbdata_register_t data);
        attr_value_t (*notify_callbacks_done)(
                conf_object_t *NOTNULL obj,
                void (*cb)(cbdata_call_t data, conf_object_t *obj),
                cbdata_register_t data);
        attr_value_t (*cancel)(conf_object_t *NOTNULL obj,
                               debug_cancel_id_t cid);
};
#define DEBUG_NOTIFICATION_INTERFACE "debug_notification"
// ADD INTERFACE debug_notification_interface

/* <add id="debug_setup_interface_t">
   <name>debug_setup_interface_t</name>
   <ndx>debug_setup_interface_t</ndx>

   Functions in the <iface>debug_setup</iface> interfaces are used to provide
   the debugger with symbol files and paths. There are also functions for
   listing what symbol files and paths have been added.

   <insert id="debug_attr_value_return_type"/>

   Upon success, all functions for adding symbols will return an <tt>id</tt>
   that can be used with <fun>remove_symbol_file</fun>.

   <ndx>add_symbol_file!debug_setup interface method</ndx>
   <fun>add_symbol_file</fun> adds a symbol file, <arg>symbol_file</arg>, used
   for debugging contexts that match the context-query <arg>query</arg>. The
   <arg>address</arg> argument specifies the address that the file should be
   mapped to. If <arg>absolute_address</arg> is set then the given
   <arg>address</arg> will be the absolute address of the first relocatable
   segment and other segments will be added with their given offsets to that
   segment.

   Errors specific to this function:

   <ul>

   <li><em>Debugger_Failed_To_Open_File</em> - File cannot be opened.</li>

   <li><em>Debugger_Unrecognized_File_Format</em> - The file format is not
   recognized.</li>

   <li><em>Debugger_Segment_Info_Missing</em> - If an ELF file is being added,
   but no valid segments can be found.</li>

   </ul>

   <ndx>add_symbol_segment!debug_setup interface method</ndx>
   <fun>add_symbol_segment</fun> adds symbols from the specified
   <arg>segment</arg> of an ELF symbol file. Other arguments are the same as
   for <fun>add_symbol_file</fun>. The address of the segment is specified with
   the <arg>address</arg> argument. If <arg>absolute_address</arg> is set this
   <arg>address</arg> will be an absolute address otherwise it will be an
   offset to the address found in the symbol file for that segment.

   Errors specific to this function: <ul>

   <li><em>Debugger_Segment_Not_Found</em> - Segment not found when adding a
   segment.</li>

   <li><em>Debugger_Segment_Info_Missing</em> - Segment information is missing
   or incomplete.</li>

   <li><em>Debugger_Not_Relocatable</em> - The segment is not relocatable.</li>

   <li><em>Debugger_Unsupported_For_File_Format</em> - File format is not
   ELF.</li>

   </ul>

   <ndx>add_symbol_section!debug_setup interface method</ndx>
   <fun>add_symbol_section</fun> adds symbols from the specified
   <arg>section</arg> of an ELF symbol file. Other arguments are the same as
   for <fun>add_symbol_file</fun>. The address of the section is specified with
   the <arg>address</arg> argument. If <arg>absolute_address</arg> is set this
   <arg>address</arg> will be an absolute address otherwise it will be an
   offset to the address found in the symbol file for that section.

   Errors specific to this function: <ul>

   <li><em>Debugger_Section_Not_Found</em> - Section not found when adding a
   section.</li>

   <li><em>Debugger_Section_Info_Missing</em> - Section information is missing
   or incomplete.</li>

   <li><em>Debugger_Not_Relocatable</em> - The section is not relocatable.</li>

   <li><em>Debugger_Unsupported_For_File_Format</em> - File format is not
   ELF.</li>

   </ul>

   <note>Adding the same symbol file, section or segment more than once might
   result in unexpected behavior and is not supported.</note>

   <ndx>remove_symbol_file!debug_setup interface method</ndx>
   <fun>remove_symbol_file</fun> removes the debugger's knowledge of symbols
   that was added with any of the functions for adding symbols. The
   <arg>id</arg> argument is the id returned from the add function.

   Errors specific to this function: <ul>

   <li><em>Debugger_Unknown_Id</em> - The <arg>id</arg> is unknown.</li>

   </ul>

   <ndx>clear_symbol_files!debug_setup interface method</ndx>
   <fun>clear_symbol_files</fun> removes the debugger's knowledge about all
   symbol files added by <arg>add_symbol_file</arg>.

   <ndx>symbol_files!debug_setup interface method</ndx> <fun>symbol_files</fun>
   lists all added symbol files. A dictionary, with <tt>id</tt> as key will be
   returned. An id is always bound to one query and one symbol file, but it can
   contain several memory maps. The listed <arg>id</arg> is the argument passed
   to <fun>remove_symbol_file</fun>. The dictionary values have the following
   format:

   <ul>

   <li><arg>query</arg> <type>(string)</type> - The context query the symbol is
   valid for.</li>

   <li><arg>relocation</arg> <type>(uint64)</type> - The relocation address
   provided when the symbol file was added.</li>

   <li><arg>symbol-file</arg> <type>(string)</type> - The file containing the
   symbol information.</li>

   <li><arg>memory-maps</arg> <type>([&lt;dict&gt;, ...])</type> - A list of
   memory maps that are added togheter as the same <tt>id</tt>, see format
   below:</li>

   </ul>

   The dictionary describing a memory map has the following format:
   <ul>

   <li><arg>address</arg> <type>(uint64)</type> - The address of the section in
   memory.</li>

   <li><arg>size</arg> <type>(uint64)</type> - The section size in memory.</li>

   <li><arg>flags</arg> <type>(uint64)</type> - Format specific flags describing
   the section.</li>

   <li><arg>section</arg> <type>(string)</type> - The name of the section.</li>

   <li><arg>file-offset</arg> <type>(uint64)</type> - Offset in symbol file for
   the section.</li>

   <li><arg>file-size</arg> <type>(uint64)</type> - Size of the section in the
   symbol file.</li>

   </ul>

   <ndx>symbol_files_for_ctx!debug_setup interface method</ndx>
   <fun>symbol_files_for_ctx</fun> is the same as <fun>symbol_files</fun>
   except that it only returns symbol files that are valid for the given
   context id, <arg>ctx_id</arg>.

   <ndx>list_all_mappings!debug_setup interface method</ndx>
   <fun>list_all_mappings</fun> lists all symbol mappings for a certain context
   <arg>ctx_id</arg>. This will be all mappings from symbol files added by
   users plus any symbol mappings added elsewhere, from trackers for example.
   The returned value is a dictionary on the following format:

   <ul>

   <li><arg>filename</arg> <type>(string)</type> - The file backing the memory
   map.</li>

   <li><arg>query</arg> <type>(string)</type> - The query the map is valid
   for.</li>

   <li><arg>address</arg> <type>(uint64)</type> - The map's address in context
   memory.</li>

   <li><arg>size</arg> <type>(uint64)</type> - The size of the map in
   memory.</li>

   <li><arg>flags</arg> <type>(uint64)</type> - Read, write, and execute flags,
   bit 0 is set if readable, bit 1 if writeable and bit 2 if executable. If this
   value is 0 this is the same as if all flags are set.</li>

   <li><arg>section-name</arg> <type>(string)</type> - The section name, or
   NIL.</li>

   <li><arg>file-offset</arg> <type>(int64)</type> - Offset into the backing
   file.</li>

   <li><arg>file-size</arg> <type>(uint64)</type> - Size of the map in the
   backing file.</li>

   <li><arg>relocation</arg> <type>(uint64)</type> - The offset from the address
   in the symbol file to where the mappings is actually loaded in memory. This
   is not always present in the dictionary.</li>

   </ul>

   Some other internal entries could possibly also be present in the
   dictionary.


   <ndx>add_path_map_entry!debug_setup interface method</ndx>
   <fun>add_path_map_entry</fun> adds a path math entry that maps a source
   file from the <arg>source</arg> in the symbol file to the actual
   destination, <arg>dest</arg>, where it is located on disk. The
   <arg>query</arg> argument specifies for which context-queries the mapping
   should apply. The returned id can be used with
   <fun>remove_path_map_entry</fun> to remove the added path map. The source
   path may not be empty or be just "." or "./".

   Errors specific to this function: <ul>

   <li><em>Debugger_Invalid_Path</em> - The source path is not valid.</li>

   </ul>

   <ndx>remove_path_map_entry!debug_setup interface method</ndx>
   <fun>remove_path_map_entry</fun> removes an entry that was added with
   <fun>add_path_map_entry</fun>. The <arg>id</arg> is the value returned from
   the add function.

   Errors specific to this function: <ul>

   <li><em>Debugger_Unknown_Id</em> - The provided <arg>id</arg> is
   unknown.</li>

   </ul>

   <ndx>clear_path_map_entries!debug_setup interface method</ndx>
   <fun>clear_path_map_entries</fun> removes all knowledge about all path map
   entries added with <fun>add_path_map_entry</fun>.

   <ndx>path_map_entries!debug_setup interface method</ndx>
   <fun>path_map_entries</fun> lists all path map entries that have been added
   with <fun>add_path_map_entry</fun>, that matches the given context id. If
   the context id is nil, then all path maps will be listed. The format of the
   entries in the returned list are dictionaries, with an <tt>id</tt> of type
   debug_setup_id_t as key:

   <ul>

   <li><arg>query</arg> <type>(string)</type> - The context query the path map
   is valid for.</li>

   <li><arg>source</arg> <type>(string)</type> - The source to translate
   from.</li>

   <li><arg>destination</arg> <type>(string)</type> - The destination to
   translate to.</li>

   </ul>

   <ndx>path_map_entries_for_ctx!debug_setup interface method</ndx>
   <fun>path_map_entries_for_ctx</fun> is the same as
   <fun>path_map_entries</fun> except that it only lists path maps that are
   valid for the given context id, <arg>ctx_id</arg>.


   <ndx>apply_path_map!debug_setup interface method</ndx>
   <fun>apply_path_map</fun> applies any added path map to a file path,
   <arg>filename</arg>, for a given context with ID <arg>ctx_id</arg>. The path
   with the path map applied will be returned. The path map will only apply if
   the destination file exists and if the path given in <arg>filename</arg>
   does not, otherwise the provided file will be returned. The returned path
   will always contain forward slashes as path separator, regardless of what
   the host system uses, or if any path map was applied or not.

   <note>For functions that take <arg>query</arg> as argument, having this set
   to nil will work the same way as for <tt>"*"</tt>. A bad context query will
   result in a <type>Debugger_Incorrect_Context_Query</type> error.</note>

   <insert-until text="// ADD INTERFACE debug_setup_interface"/>
   </add>

   <add id="debug_setup_interface_exec_context">
   Global Context for all methods.
   </add>
*/

typedef int64 debug_setup_id_t;

SIM_INTERFACE(debug_setup) {
        attr_value_t (*add_symbol_file)(conf_object_t *NOTNULL obj,
                                        const char *query,
                                        const char *NOTNULL symbol_file,
                                        uint64 address, bool absolute_address);
        attr_value_t (*add_symbol_segment)(conf_object_t *NOTNULL obj,
                                           const char *query,
                                           const char *NOTNULL symbol_file,
                                           unsigned segment, uint64 address,
                                           bool absolute_address);
        attr_value_t (*add_symbol_section)(conf_object_t *NOTNULL obj,
                                           const char *query,
                                           const char *NOTNULL symbol_file,
                                           const char *NOTNULL section,
                                           uint64 address,
                                           bool absolute_address);
        attr_value_t (*remove_symbol_file)(conf_object_t *NOTNULL obj,
                                           debug_setup_id_t id);
        void (*clear_symbol_files)(conf_object_t *NOTNULL obj);
        attr_value_t (*symbol_files)(conf_object_t *NOTNULL obj);
        attr_value_t (*symbol_files_for_ctx)(conf_object_t *NOTNULL obj,
                                             const char *NOTNULL ctx_id);
        attr_value_t (*list_all_mappings)(conf_object_t *NOTNULL obj,
                                          const char *NOTNULL ctx_id);
        attr_value_t (*add_path_map_entry)(conf_object_t *NOTNULL obj,
                                           const char *query,
                                           const char *NOTNULL source,
                                           const char *NOTNULL dest);
        attr_value_t (*remove_path_map_entry)(conf_object_t *NOTNULL obj,
                                              debug_setup_id_t id);
        void (*clear_path_map_entries)(conf_object_t *NOTNULL obj);
        attr_value_t (*path_map_entries)(conf_object_t *NOTNULL obj);
        attr_value_t (*path_map_entries_for_ctx)(conf_object_t *NOTNULL obj,
                                                 const char *NOTNULL ctx_id);
        attr_value_t (*apply_path_map)(conf_object_t *NOTNULL obj,
                                       const char *NOTNULL ctx_id,
                                       const char *NOTNULL filename);
};
#define DEBUG_SETUP_INTERFACE "debug_setup"
// ADD INTERFACE debug_setup_interface


/* <add id="debug_query_interface_t">
   <name>debug_query_interface_t</name>
   <ndx>debug_query_interface_t</ndx>

   This interface provides functions for querying the debugger for contexts and
   their information. Functions here will be used to find specific contexts or
   to create context-queries, that can be used with other functions in debugger
   interfaces.

   Most functions in this interface take a Context ID, <arg>ctx_id</arg>, as
   argument. This ID is passed as an argument to most callback functions in the
   <iface>debug_notification</iface> interface and returned by some functions
   in this interface.

   All functions return an <type>attr_value_t</type> consisting of a list with
   two elements. The first element is an error code of
   <type>debugger_error_t</type> type. The second element depends on the
   first. If the first element is <type>Debugger_No_Error</type>, meaning that
   the function went well, then the second element will contain the expected
   return value that is specified per function below. If the first element is
   another error code, then the second element will be a string describing the
   error that occurred.

   <ndx>matching_context!debug_query interface method</ndx>
   <fun>matching_contexts</fun> returns a list of context IDs matching the
   specified context-query, <arg>query</arg>. The returned list can be empty if
   there are no matching contexts.

   <ndx>get_context_group!debug_query interface method</ndx>
   <fun>get_context_group</fun> returns a context ID for the context marked
   as the group leader for the specified context, <arg>ctx_id</arg>. As example
   in a Linux tracker the process node will be the group leader for all threads
   belonging to that process. A context that lacks a specific group leader will
   return itself.

   <ndx>get_context_parent!debug_query interface method</ndx>
   <fun>get_context_parent</fun> returns an ID to the parent context for the
   specified context.

   Errors specific to this function: <ul>

   <li><em>Debugger_Not_Supported_For_Context</em> - The context does not have
   a parent.</li>

   </ul>

   <ndx>get_context_children!debug_query interface method</ndx>
   <fun>get_context_children</fun> returns a list of context IDs for the
   children of the specified context. This list can be empty if the context has
   no children.

   <ndx>query_for_context_group!debug_query interface method</ndx>
   <ndx>query_for_context_id!debug_query interface method</ndx>
   <ndx>query_for_context_tree!debug_query interface method</ndx>
   <fun>query_for_context_group</fun>, <fun>query_for_context_id</fun> and
   <fun>query_for_context_tree</fun> will return context-queries which can be
   used for matching all contexts in a group, the context that matches an ID or
   contexts in the tree that starts with the specified context. The latter
   context-query will match the context itself plus all its child contexts, the
   children of those contexts and so on. The result of these functions should
   be used with functions that take a context-query as argument.

   <ndx>context_name!debug_query interface method</ndx> <fun>context_name</fun>
   returns the name of the specified context.

   Errors specific to this function: <ul>

   <li><em>Debugger_Not_Supported_For_Context</em> - If the he context lacks a
   name.</li>

   </ul>

   <ndx>context_id_for_object!debug_query interface method</ndx>
   <fun>context_id_for_object</fun> returns a context ID for the specified
   Simics object, <arg>ctx_obj</arg>, if that has an associated context. In
   general, processors and some memory spaces usually have contexts associated
   with them.

   Errors specific to this function: <ul>

   <li><em>Debugger_No_Context_For_Object</em> - The object does not have any
   associated context.</li>

   </ul>

   <ndx>context_has_state!debug_query interface method</ndx>
   <fun>context_has_state</fun> returns a boolean that tells whether the
   specified context has state or not. That a context has state will mean that
   callbacks in the <iface>debug_notification</iface> will trigger for this
   context. The contexts that have state are usually leaf nodes in the OS
   Awareness node tree.

   <ndx>object_for_context!debug_query interface method</ndx>
   <fun>object_for_context</fun> is used to get a Simics object that matches
   the context for <arg>ctx_id</arg>, if such an object exists.

   Errors specific to this function: <ul>

   <li><em>Debugger_Missing_Object</em> - The context does not match a Simics
   object.</li>

   </ul>

   <ndx>get_active_processor!debug_query interface method</ndx>
   <fun>get_active_processor</fun> is used check if a context is active and in
   that case return the Simics processor object that the node is active on.

   Errors specific to this function: <ul>

   <li><em>Debugger_Context_Is_Not_Active</em> - Context is not active.</li>

   </ul>

   <note>For functions that take <arg>query</arg> as argument, having this set
   to nil will work the same way as for <tt>"*"</tt>. A bad context query will
   result in a <type>Debugger_Incorrect_Context_Query</type> error.</note>

   <insert-until text="// ADD INTERFACE debug_query_interface"/>

   <insert id="debugger_error_t"/>
   </add>

   <add id="debug_query_interface_exec_context">
   Global Context for all methods.
   </add>
*/
SIM_INTERFACE(debug_query) {
        attr_value_t (*matching_contexts)(conf_object_t *NOTNULL obj,
                                          const char *query);
        attr_value_t (*get_context_group)(conf_object_t *NOTNULL obj,
                                          const char *NOTNULL ctx_id);
        attr_value_t (*get_context_parent)(conf_object_t *NOTNULL obj,
                                           const char *NOTNULL ctx_id);
        attr_value_t (*get_context_children)(conf_object_t *NOTNULL obj,
                                             const char *NOTNULL ctx_id);
        attr_value_t (*query_for_context_group)(conf_object_t *NOTNULL obj,
                                                const char *ctx_id);
        attr_value_t (*query_for_context_id)(conf_object_t *NOTNULL obj,
                                             const char *NOTNULL ctx_id);
        attr_value_t (*query_for_context_tree)(conf_object_t *NOTNULL obj,
                                               const char *NOTNULL ctx_id);
        attr_value_t (*context_name)(conf_object_t *NOTNULL obj,
                                     const char *NOTNULL ctx_id);
        attr_value_t (*context_id_for_object)(conf_object_t *NOTNULL obj,
                                              conf_object_t *NOTNULL ctx_obj);
        attr_value_t (*object_for_context)(conf_object_t *NOTNULL obj,
                                           const char *NOTNULL ctx_id);
        attr_value_t (*context_has_state)(conf_object_t *NOTNULL obj,
                                          const char *NOTNULL ctx_id);
        attr_value_t (*get_active_processor)(conf_object_t *NOTNULL obj,
                                             const char *NOTNULL ctx_id);
};
#define DEBUG_QUERY_INTERFACE "debug_query"
// ADD INTERFACE debug_query_interface


/* <add id="debug_step_interface_t">
   <name>debug_step_interface_t</name>
   <ndx>debug_step_interface_t</ndx>

   This interface is used to perform stepping with the debugger, on a specified
   debug context.

   <insert id="debug_attr_value_return_type"/>

   <ndx>instruction_into!debug_step interface method</ndx>
   <ndx>instruction_over!debug_step interface method</ndx>
   <fun>instruction_into</fun> and <fun>instruction_over</fun> runs one step
   forward for the specified context. <fun>instruction_into</fun> will enter
   subroutine calls while <fun>instruction_over</fun> will step over subroutine
   calls.

   <ndx>into!debug_step interface method</ndx>
   <ndx>over!debug_step interface method</ndx>
   <fun>into</fun> and <fun>over</fun> will run one
   source line forward for the specified context. <fun>into</fun> will enter
   function calls while <fun>over</fun> will skip over functions.

   <ndx>out!debug_step interface method</ndx>
   <fun>out</fun> will run until the currently active function returns.

   All function take a context ID, <arg>ctx_id</arg>, as argument. This context
   ID is passed as an argument to callbacks for functions in the
   <iface>debug_notification</iface> interface. The context, that is passed to
   functions in this interface, has to have state (see
   <fun>context_has_state</fun> in the <iface>debug_query</iface> interface)
   otherwise a <type>Debugger_Context_Does_Not_Have_State</type> error will be
   returned.

   Calling functions in this interface can only be done when simulation is
   stopped, otherwise a <type>Debugger_Already_Running</type> error will be
   returned.

   For all functions in this interface, if another stop reason occurs before a
   function finishes the simulation will stop at that point instead.

   Stepping for a context that is not active will run until that context
   becomes active and then take the step.

   <insert-until text="// ADD INTERFACE debug_step_interface"/>
   </add>

   <add id="debug_step_interface_exec_context">
   Global Context for all methods.
   </add>
*/
SIM_INTERFACE(debug_step) {
        attr_value_t (*instruction_into)(conf_object_t *NOTNULL obj,
                                         const char *NOTNULL ctx_id);
        attr_value_t (*into)(conf_object_t *NOTNULL obj,
                             const char *NOTNULL ctx_id);
        attr_value_t (*instruction_over)(conf_object_t *NOTNULL obj,
                                         const char *NOTNULL ctx_id);
        attr_value_t (*over)(conf_object_t *NOTNULL obj,
                             const char *NOTNULL ctx_id);
        attr_value_t (*out)(conf_object_t *NOTNULL obj,
                            const char *NOTNULL ctx_id);
        attr_value_t (*reverse_instruction_into)(conf_object_t *NOTNULL obj,
                                                 const char *NOTNULL ctx_id);
        attr_value_t (*reverse_into)(conf_object_t *NOTNULL obj,
                                     const char *NOTNULL ctx_id);
        attr_value_t (*reverse_instruction_over)(conf_object_t *NOTNULL obj,
                                                 const char *NOTNULL ctx_id);
        attr_value_t (*reverse_over)(conf_object_t *NOTNULL obj,
                                     const char *NOTNULL ctx_id);
        attr_value_t (*reverse_out)(conf_object_t *NOTNULL obj,
                                    const char *NOTNULL ctx_id);
};
#define DEBUG_STEP_INTERFACE "debug_step"
// ADD INTERFACE debug_step_interface

/* <add id="debug_symbol_interface_t">
   <name>debug_symbol_interface_t</name>
   <ndx>debug_symbol_interface_t</ndx>

   This interface contains functions for retrieving various symbol information
   for a debug context, <arg>ctx_id</arg>. There are also a few functions for
   updating symbol information.

   Prior to using functions in this interface, symbol file(s) containing symbol
   information should have been added using the <iface>debug_setup</iface>
   interface, for a context-query that matches the context ID provided to the
   function here. Symbol files can also be added in other ways such as from a
   tracker or from Eclipse.

   Some functions, that do read values or stack, can also be used directly with
   symbol files opened with the <iface>debug_symbol_file</iface> interface.

   All functions in this interface take a context ID <arg>ctx_id</arg> as
   argument. This is the ID passed as an argument to
   <iface>debug_notification</iface> callbacks or returned from some
   <iface>debug_query</iface> interface functions.

   <insert id="debug_attr_value_return_type"/>

   All addresses used in this interface are virtual addresses.

   For functions that take <arg>frame</arg> as argument, providing
   <arg>frame</arg> = -1 means no frame. This can be used when finding symbols
   for functions or global variables. When using other <arg>frame</arg> than -1
   the context has to be active, otherwise no frame will exists and a
   <type>Debugger_Context_Is_Not_Active</type> error will be returned. If the
   specified frame cannot be found, then error code
   <type>Debugger_Frame_Outside_Of_Known_Stack</type> will be returned.

   Functions that handle stack can only be used with an active context,
   otherwise error code <type>Debugger_Context_Is_Not_Active</type> or
   <type>Debugger_Context_Does_Not_Have_State</type> will be returned depending
   on if the context has state or not.

   <ndx>address_source!debug_symbol interface method</ndx>
   <fun>address_source</fun> will provide callbacks, <arg>cb</arg>, with source
   information for the range specified by <arg>address</arg> and
   <arg>size</arg>. The range must not wrap around the 64-bit limit. Source
   information will be provided as code areas, which is the source information
   for a range of continuous addresses matching the same file and line. User
   data can be passed to the callback using the <arg>data</arg> argument and
   will be passed as the first argument in the callback. Each code area
   matching the range will get a callback with information about the code area
   in the <arg>code_area</arg> argument, which is a dictionary with the
   following elements:

   <ul>

   <li><arg>filename</arg> <type>(string)</type> - File name of the source file
   where the specified <arg>address</arg> is found.</li>

   <li><arg>start-line</arg>, <arg>end-line</arg> <type>(int64)</type> -
   Starting and ending lines in the source file for the <arg>address</arg>.
   <arg>end-line</arg> is inclusive so if the <arg>address</arg> just matches
   one source line <arg>start-line</arg> and <arg>end-line</arg> will be the
   same.</li>

   <li><arg>start-column</arg>, <arg>end-column</arg> <type>(int64)</type> -
   Starting and ending columns on the line for <arg>address</arg>. If column
   information is not available in debug information the value will be 0.
   <arg>end-column</arg> is inclusive so the column is included for
   <arg>address</arg>.</li>

   <li><arg>start-address</arg>, <arg>end-address</arg> <type>(uint64)</type> -
   Starting and ending addresses that correspond to the returned line and
   column information. <arg>end-address</arg> is inclusive so this matches the
   last byte of the last instruction of this code area. If the code area is
   empty then, then <arg>end-address</arg> will not be present. This can happen
   when asking for a line prior to the first executable line.</li>

   </ul>

   Errors specific to this function: <ul>

   <li><em>Debugger_Source_Not_Found</em> - There are no matching code
   areas.</li>

   <li><em>Debugger_Incorrect_Size</em> - Incorrect <arg>size</arg>
   provided. The size cannot be zero and it cannot make the end address exceed
   64 bits.</li>

   <li><em>Debugger_Lookup_Failure</em> Address lookup failure. Can occur if
   the address is not in the debug information or the lookup failed for some
   other reason. The error message explains more what went wrong.</li>

   </ul>

   <ndx>source_address!debug_symbol interface method</ndx>
   <fun>source_address</fun> will provide callbacks, with source information
   for the line specified by <arg>filename</arg>, <arg>line</arg> and
   <arg>column</arg>. The <arg>column</arg> argument will only be useful for
   binaries with column information in the debug information.

   One call to this function will provide as many callbacks as there are code
   areas matching that source information. The <arg>code_area</arg> provided to
   the callback is a dictionary with the same format as for the callback from
   <fun>address_source</fun>.

   Errors specific to this function: <ul>

   <li><em>Debugger_File_Not_Found</em> - There is no matching source file,
   <arg>filename</arg>, in the debug information.</li>

   <li><em>Debugger_Source_Not_Found</em> - The <arg>line</arg> or
   <arg>column</arg> is not found in the debug information.</li>

   <li><em>Debug_Lookup_Failure</em> - Some problem with the lookup other than
   that file, line or column was not found. Error described in error
   message.</li>

   </ul>

   For binaries <em>without</em> column information, a line is executable if
   the <arg>start-line</arg> in the callback matches the asked <arg>line</arg>.

   For binaries <em>with</em> column information one can say that the line is
   executable if any code area can be found with <arg>start-line</arg> matching
   the asked line. When asking for a <arg>line</arg> and <arg>column</arg> and
   the <arg>start-line</arg> matches a previous line, but the
   <arg>end-line</arg> matches the asked line, then one should ask for the
   <arg>column</arg> just after <arg>end-column</arg> to see if the
   <arg>start-line</arg> of that code area matches the asked line and if it
   does then the asked line is considered executable.

   <ndx>stack_depth!debug_symbol interface method</ndx>
   <fun>stack_depth</fun> returns the current stack depth.

   Errors specific to this function: <ul>

   <li><em>Debugger_Failed_To_Get_Stack_Frame</em> - Failed to get stack
   frames.</li>

   </ul>

   <ndx>stack_frames!debug_symbol interface method</ndx>
   <fun>stack_frames</fun> returns a list of dictionaries with information
   about each frame. The executing frame is at position zero in the list. The
   elements of the stack frame dictionary are:

   <ul>

   <li><arg>address</arg> <type>(uint64)</type> - For the executing frame this
   is the address of the program counter, for all other frames this is the
   address that the function will return to.</li>

   <li><arg>source-file</arg> <type>(string)</type> - The source file for the
   frame. Can be nil if no matching file is found.</li>

   <li><arg>source-line</arg> <type>(uint64)</type> - The source line in the
   file for the frame. Will be nil if <arg>source-file</arg> is nil.</li>

   <li><arg>function-name</arg> <type>(string)</type> - The function that is
   executing in the frame. Can be nil if no function is found.</li>

   </ul>

   Errors specific to this function: <ul>

   <li><type>Debugger_Failed_To_Get_Stack_Frame</type> - If there are problems
   getting stack depth or getting frame info.</li>

   <li><type>Debugger_Failed_To_Read</type> - If a register cannot be read for
   some frame.</li>

   <li><type>Debugger_Failed_To_Get_PC</type> - If the program counter
   definition cannot be found for the context.</li>

   </ul>

   <ndx>local_variables!debug_symbol interface method</ndx>
   <fun>local_variables</fun> returns a list of names for local variables in
   the specified <arg>frame</arg>.

   <ndx>local_arguments!debug_symbol interface method</ndx>
   <fun>local_arguments</fun> returns a list of names of arguments to the
   function in the specified <arg>frame</arg>.

   Both <fun>local_variables</fun> and <fun>local_arguments</fun> can return an
   empty list if no local variables or arguments are found. If something goes
   wrong while looking up variables or arguments they will return a
   <type>Debugger_Lookup_Failure</type> with more information in the error
   string.

   <ndx>expression_value!debug_symbol interface method</ndx>
   <fun>expression_value</fun> returns the value of an expression,
   <arg>expr</arg>, in the specified <arg>frame</arg>. See note about
   <arg>address_scope</arg> further down. The returned value will be of integer
   type if the expression is of integer type, including if it is an
   address. Floating-point types will be returned as such. If the expression is
   a structure, union, class or an array a list containing the elements of that
   type will be returned.

   Errors specific to this function: <ul>

   <li><em>Debugger_Failed_To_Evaluate_Expression</em> - The expression could
   not be evaluated, more information in the error message.</li>

   </ul>

   <ndx>expression_type!debug_symbol interface method</ndx>
   <fun>expression_type</fun> returns the type of an expression,
   <arg>expr</arg>, in the specified <arg>frame</arg>. See note about
   <arg>address_scope</arg> further down. For most types the return value will
   be a string containing the type, for example <type>'int'</type>,
   <type>'double'</type>, <type>'char'</type> or <type>'my_own_type_t</type>.

   For pointers the return value will be a list with <arg>'*'</arg> as the
   first argument followed by the type of the pointer, for example <type>['*',
   'void']</type> or <type>['*', ['*', 'char']]</type>.

   For qualifiers ('const', 'volatile' or 'restrict') these will be added as an
   element to the list, such as <tt>['const', 'char']</tt> or <tt>['volatile',
   ['*', 'int']]</tt>.

   Functions are returned with a <arg>'()'</arg> string as the first element of
   a list, followed by the return type, followed by a list of arguments to the
   function. Example: <tt>['*', ['()', 'int', ['int', ['*', ['*',
   'char']]]]]</tt> for a function of type <type>int (*)(int, char **)</type>.

   A <type>struct</type> or <type>union</type> will return a string
   <type>'struct'</type>, followed by the struct name, if available, then
   followed by a list of lists containing the struct member type and name
   <tt>['struct', 'my_struct', [['int', 'i'], ['float', 'f']]]</tt> as
   example. If the struct has been assigned a type with <fun>typedef</fun> then
   the output will be <type>'typedef'</type> followed by the name of the
   assigned type, then the list of members. An example: <tt>['typedef',
   'mytype_t', [['int', 'age'], [['*', 'char'], 'name']]]</tt>. For unions the
   string <type>'struct'</type> will be replace with the string
   <type>'union'</type>.

   A <type>bit field</type> will return a list containing a string
   <type>'bitfield'</type> followed by the basic type of the bit field,
   followed by the size of the bit field. An example: <tt>['bitfield', 'int',
   5]</tt> for a variable declared as <tt>int var:5</tt>.

   For array types <fun>expression_type</fun> will return a list, <tt>['[]',
   array size, array type]</tt>, where the array type is of the same format as
   other types for this function. An example: <tt>['[]', 10, 'char']</tt> for a
   variable declared as <tt>char x[10]</tt>;

   Enumerations will be displayed as a list: <tt>['enum', enum name,
   members]</tt>. The <arg>enum name</arg> is the declared name for the
   enumeration, if no such name exists this list field will be left out. The
   <arg>members</arg> field will contain a list of [name, value] lists for all
   the members of the enumeration. An example: <tt>['enum, 'my_enum',
   [['My_Val_0', 0], [My_Val_1, 1]]]</tt>.

   <note>There are some limitations for symbols from PE files that can depend
   on the version of <file>dbghelp.dll</file>.
   <ul>
   <li>Type defined types might get shown as the
   base type instead of the <type>'typedef'</type> type.</li>

   <li>Variables with a <type>const</type> qualifier might be shown without
   that qualifier.</li>

   <li>Members of anonymous structures within a base structure are shown as
   being part of the base structure.</li>
   </ul></note>

   Errors specific to this function: <ul>

   <li><em>Debugger_Failed_To_Evaluate_Expression</em> - Could not evaluate the
   expression, more information in the error message.</li>

   </ul>

   <ndx>type_info!debug_symbol interface method</ndx> <fun>type_info</fun>
   returns information about what base type a type that has been added with
   <fun>typedef</fun> has. See note about <arg>address_scope</arg> further
   down. The returned value will be a list on the same format as for
   <fun>expression_value</fun>.

   Errors specific to this function: <ul>

   <li><em>Debugger_Incorrect_Type</em> - If the <arg>type</arg> asked for is
   not a valid type.</li>

   <li><em>Debugger_Lookup_Failure</em> - Finding the symbol failed or the type
   of symbol cannot be retrieved. More information in the error string.</li>

   </ul>

   <ndx>type_to_string!debug_symbol interface method</ndx>
   <fun>type_to_string</fun> converts the return value from
   <fun>expression_type</fun> or <fun>type_info</fun> to a readable string and
   returns that.

   Errors specific to this function: <ul>

   <li><em>Debugger_Incorrect_Type</em> - The format of <arg>type</arg> is
   incorrect.</li>

   </ul>

   <ndx>symbol_address!debug_symbol interface method</ndx>
   <fun>symbol_address</fun> takes a <arg>frame</arg> and a <arg>symbol</arg>
   as arguments and returns a list of addresses that matches that symbol name.
   If a single symbol address is wanted near a provided instruction pointer
   then the <fun>expression_value</fun> function can be used with the
   expression set as <tt>&amp;&lt;symbol&gt;</tt>.

   Errors specific to this function: <ul>

   <li><em>Debugger_Lookup_Failure</em> - The address for the symbol cannot be
   found. This can be because the symbol was not found or if evaluation
   failed. More information will be found in the error message.</li>

   </ul>

   <ndx>address_string!debug_symbol interface method</ndx>
   <fun>address_string</fun> returns the string at the specified
   <arg>address</arg>. The <arg>maxlen</arg> is used to specify the maximum
   length of a string to read from memory. If the string at <arg>address</arg>
   exceeds the given length the truncated string will be returned, and a
   terminating null character will be added if needed. A <arg>maxlen</arg>
   value of zero means no maximum string length.

   Errors specific to this function: <ul>

   <li><em>Debugger_Failed_To_Read</em> - Failed to read memory at
   <arg>address</arg>.</li>

   </ul>

   <ndx>lvalue_write!debug_symbol interface method</ndx>
   <fun>lvalue_write</fun> writes a <arg>value</arg> to a <arg>symbol</arg> in
   the specified <arg>frame</arg>. The <arg>value</arg> can be of
   <type>integer</type> types or <type>floating-point</type> type if the type
   of the lvalue is of <type>floating-point</type> type. The returned value
   when the write goes well is nil.

   Errors specific to this function: <ul>

   <li><em>Debugger_Incorrect_Type</em> - The type of <arg>value</arg> is
   incorrect.</li>

   <li><em>Debugger_Failed_To_Evaluate_Expression</em> - The provided
   <arg>symbol</arg> could not be found or evaluated as an expression.</li>

   <li><em>Debugger_Failed_To_Write</em> - Failed to write to
   <arg>symbol</arg>.</li>

   </ul>

   <ndx>address_write!debug_symbol interface method</ndx>
   <fun>address_write</fun> writes an <type>attr_value_t</type> of
   <type>data</type> type, <arg>value</arg>, to the specified
   <arg>address</arg>. Returned value is nil when the write goes well.

   Errors specific to this function: <ul>

   <li><em>Debugger_Incorrect_Type</em> - The type of <arg>value</arg> is
   incorrect.</li>

   <li><em>Debugger_Failed_To_Write</em> - The memory at <arg>address</arg>
   cannot be written to.</li>

   </ul>

   <ndx>address_read!debug_symbol interface method</ndx>
   <fun>address_read</fun> reads <arg>size</arg> number of bytes from the
   specified <arg>address</arg>. The read data is returned as an
   <type>attr_value_t</type> of <type>data</type> type.

   Errors specific to this function: <ul>

   <li><em>Debugger_Failed_To_Read</em> - The <arg>address</arg> cannot be
   read.</li>

   </ul>

   <ndx>struct_members!debug_symbol interface method</ndx>
   <fun>struct_members</fun> returns a list of all members of a struct,
   <arg>struct_name</arg>. See note about <arg>address_scope</arg> further
   down. Each element of the returned list will be on the format <tt>[name,
   offset, size]</tt>. The <arg>size</arg> and <arg>offset</arg> elements are
   usually integers representing the offset into the struct and the size of the
   variable. If the struct member is a bit field then <arg>size</arg> will be a
   list on the format <tt>[base size, bits]</tt>, where <arg>base size</arg> is
   the number of bytes for the declared base type and <arg>bits</arg> is the
   number of bits the declared for the bit field. For bit fields
   <arg>offset</arg> will be a list on the format <tt>[byte offset, bit
   offset]</tt>, where <arg>byte offset</arg> is the offset of in bytes into
   the structure and <arg>bit offset</arg> is the offset in bits from the byte
   offset to where the bit field starts. If some member, size or offset cannot
   be retrieved then that element will be set as nil. This can for example
   occur if a struct member is an anonymous struct or union.

   Errors specific to this function: <ul>

   <li><em>Debugger_Incorrect_Type</em> - The provided symbol,
   <arg>struct_name</arg>, is not a structure.</li>

   <li><em>Debugger_Lookup_Failure</em> - Failed to find the symbol or its
   type.</li>

   </ul>

   <ndx>struct_field!debug_symbol interface method</ndx>
   <fun>struct_field</fun> returns a list of [offset, size] for a
   <arg>field</arg> of a given structure, <arg>struct_name</arg>. The
   <arg>offset</arg> and <arg>size</arg> are usually integers, but if the
   <arg>field</arg> is a bit field then the returned <arg>size</arg> and
   <arg>offset</arg> will be on the same format as for
   <fun>struct_members</fun>. See note about <arg>address_scope</arg> below.

   This function can return the same error codes, for the same reasons, as
   the <fun>struct_members</fun> function.

   The <arg>address_scope</arg> argument that is provided for several functions
   can be used to specify where to find the symbol if there are several matches
   for the provided symbol name or expression. This argument is used to provide
   an address that tells the function in which scope to search for the symbol.
   This is only taken in account when no frame (-1) is given as frame. This
   address can be an address that belongs to a loaded symbol file to prioritize
   finding symbols from that symbol file.

   <ndx>list_functions!debug_symbol interface method</ndx>
   <ndx>list_global_variables!debug_symbol interface method</ndx>
   <fun>list_functions</fun> and <fun>list_global_variables</fun> lists all
   function or global variable symbols that are known for the given
   context. The symbols shown are the ones that have been added with the
   <fun>add_symbol_file</fun> function of the <iface>debug_setup</iface>
   interface. The returned format is a list of dictionaries with the dictionary
   elements on the format:

   <ul>

   <li><arg>symbol</arg> <type>(string)</type> - The name of the symbol.</li>

   <li><arg>address</arg> <type>(uint64)</type> - The address in memory of the
   symbol.</li>

   <li><arg>size</arg> <type>(uint64)</type> - The size of the symbol in
   memory.</li>

   </ul>

   <ndx>list_source_files!debug_symbol interface method</ndx>
   <fun>list_source_files</fun> lists all source files provided by the debug
   information of the symbol files for the given context.

   <insert-until text="// ADD INTERFACE debug_symbol_interface"/>
   </add>

   <add id="debug_symbol_interface_exec_context">
   Global Context for all methods.
   </add>
*/

SIM_INTERFACE(debug_symbol) {
        attr_value_t (*address_source)(conf_object_t *NOTNULL obj,
                                       const char *NOTNULL ctx_id,
                                       uint64 address, uint64 size,
                                       void (*cb)(cbdata_call_t data,
                                                  attr_value_t code_area),
                                       cbdata_register_t data);
        attr_value_t (*source_address)(conf_object_t *NOTNULL obj,
                                       const char *NOTNULL ctx_id,
                                       const char *NOTNULL filename,
                                       uint32 line, uint32 column,
                                       void (*cb)(cbdata_call_t data,
                                                  attr_value_t code_area),
                                       cbdata_register_t data);
        attr_value_t (*address_symbol)(conf_object_t *NOTNULL obj,
                                       const char *NOTNULL ctx_id,
                                       uint64 address);
        attr_value_t (*stack_depth)(conf_object_t *NOTNULL obj,
                                    const char *NOTNULL ctx_id);
        attr_value_t (*stack_frames)(conf_object_t *NOTNULL obj,
                                     const char *NOTNULL ctx_id, int min,
                                     int max);
        attr_value_t (*local_variables)(conf_object_t *NOTNULL obj,
                                        const char *NOTNULL ctx_id, int frame);
        attr_value_t (*local_arguments)(conf_object_t *NOTNULL obj,
                                        const char *NOTNULL ctx_id, int frame);
        attr_value_t (*expression_value)(conf_object_t *NOTNULL obj,
                                         const char *NOTNULL ctx_id,
                                         int32 frame, uint64 address_scope,
                                         const char *NOTNULL expr);
        attr_value_t (*expression_type)(conf_object_t *NOTNULL obj,
                                        const char *NOTNULL ctx_id,
                                        int32 frame, uint64 address_scope,
                                        const char *NOTNULL expr);
        attr_value_t (*type_info)(conf_object_t *NOTNULL obj,
                                  const char *NOTNULL ctx_id,
                                  uint64 address_scope,
                                  const char *NOTNULL type);
        attr_value_t (*type_to_string)(conf_object_t *NOTNULL obj,
                                       attr_value_t type);
        attr_value_t (*symbol_address)(conf_object_t *NOTNULL obj,
                                       const char *NOTNULL ctx_id, int32 frame,
                                       const char *NOTNULL symbol);
        attr_value_t (*address_string)(conf_object_t *NOTNULL obj,
                                       const char *NOTNULL ctx_id,
                                       uint64 address, int maxlen);
        attr_value_t (*lvalue_write)(conf_object_t *NOTNULL obj,
                                     const char *NOTNULL ctx_id, int32 frame,
                                     const char *NOTNULL symbol,
                                     attr_value_t value);
        attr_value_t (*address_write)(conf_object_t *NOTNULL obj,
                                      const char *NOTNULL ctx_id,
                                      uint64 address, attr_value_t value);
        attr_value_t (*address_read)(conf_object_t *NOTNULL obj,
                                     const char *NOTNULL ctx_id, uint64 address,
                                     unsigned size);
        attr_value_t (*struct_members)(conf_object_t *NOTNULL obj,
                                       const char *NOTNULL ctx_id,
                                       uint64 address_scope,
                                       const char *NOTNULL struct_name);
        attr_value_t (*struct_field)(conf_object_t *NOTNULL obj,
                                     const char *NOTNULL ctx_id,
                                     uint64 address_scope,
                                     const char *NOTNULL struct_name,
                                     const char *NOTNULL field);
        attr_value_t (*list_functions)(conf_object_t *NOTNULL obj,
                                       const char *NOTNULL ctx_id);
        attr_value_t (*list_global_variables)(conf_object_t *NOTNULL obj,
                                              const char *NOTNULL ctx_id);
        attr_value_t (*list_source_files)(conf_object_t *NOTNULL obj,
                                          const char *NOTNULL ctx_id);
};
#define DEBUG_SYMBOL_INTERFACE "debug_symbol"
// ADD INTERFACE debug_symbol_interface


/* <add id="debug_symbol_file_interface_t">
   <name>debug_symbol_file_interface_t</name>
   <ndx>debug_symbol_file_interface_t</ndx>

   This interface has functions for operating directly on symbol files, instead
   of using contexts related to processes or processors. Files opened with this
   interface can be used together with functions in the
   <iface>debug_symbol</iface> interface that do not require any stack or
   location information. There are also functions for getting information about
   the symbol file, such as listing sections or segments.

   <insert id="debug_attr_value_return_type"/>

   All function but <fun>open_symbol_file</fun> has to act on a context id,
   <arg>ctx_id</arg>, that has been returned by <fun>open_symbol_file</fun>,
   otherwise a <type>Debugger_Not_Supported_For_Context</type> error will be
   returned.

   <ndx>open_symbol_file!debug_symbol_file interface method</ndx>
   <fun>open_symbol_file</fun> opens a symbol file, specified by the
   <arg>filename</arg> argument, as a context so that this can be used with
   many of the symbol lookup functions in the <iface>debug_symbol</iface>
   interface. The <arg>address</arg> is the offset for which to load the symbol
   file if <arg>absolute_address</arg> is false. If <arg>absolute_address</arg>
   is true, <arg>address</arg> will instead be the absolute address to load the
   symbol file on. The returned value will be a context id that can be used as
   argument where a <arg>ctx_id</arg> is required. Functions that take a stack
   frame can only be accessed with no stack (value -1) when used with a symbol
   file context. Files that read or write memory cannot be used.

   Errors specific to this function: <ul>

   <li><em>Debugger_Unrecognized_File_Format</em> - The file format of the file
   is not ELF or PE.</li>

   <li><em>Debugger_Unsupported_For_File_Format</em> - If the symbol file is of
   PE format then the <arg>address</arg> cannot be 0 as the internal handling
   does not support that.</li>

   </ul>

   <ndx>open_symbol_section!debug_symbol_file interface method</ndx>
   <fun>open_symbol_section</fun> opens a symbol file in the same way as
   <fun>open_symbol_file</fun>, but only adds symbols from the specified
   <arg>section</arg>. Other arguments and return value are as for
   <fun>open_symbol_file</fun>. The symbol section is closed using
   <fun>close_symbol_file</fun>. This method can only handle ELF binaries. In
   addition to errors reported by <fun>open_symbol_file</fun> this function can
   report <em>Debugger_Section_Not_Found</em> if the <arg>section</arg> cannot
   be found.

   <ndx>close_symbol_file!debug_symbol_file interface method</ndx>
   <fun>close_symbol_file</fun> closes a symbol file that was opened with
   <fun>open_symbol_file</fun>. The <arg>ctx_id</arg> should be the context id
   returned from that function. The returned value will just be nil when this
   goes well.

   <ndx>symbol_file_info!debug_symbol_file interface method</ndx>
   <fun>symbol_file_info</fun> returns a list containing a string describing
   the file format and a dictionary with information about the file. The
   <arg>ctx_id</arg> argument should be the context id returned from the
   <fun>open_symbol_file</fun>. The returned information depends on which file
   format the file has. For ELF files the string is "ELF" and the returned
   entries are:

   <ul>

   <li><arg>big-endian</arg> <type>(uint64)</type> - Set to 1 if the binary is
   built for big endian, otherwise 0.</li>

   <li><arg>elf-machine-id</arg> <type>(uint64)</type> - The ELF machine
   identifier number (<tt>e_machine</tt>) in the ELF header. This is usually
   prefixed with <tt>EM_</tt> in headers. For example <tt>EM_386 = 3</tt> and
   <tt>EM_X86_64 = 62.</tt> </li>

   </ul>

   For PE files the string is "PE" and the returned entries are:

   <ul>

   <li><arg>image-size</arg> <type>(uint64)</type> - The size of the PE image in
   memory.</li>

   <li><arg>image-base</arg> <type>(uint64)</type> - The recommended address for
   the image to be loaded at.</li>

   </ul>

   Both PE and ELF files will include the following entries:

   <ul>

   <li><arg>entry-address</arg> <type>(uint64)</type> - The program counter for
   where the execution in the file should begin.</li>

   <li><arg>file-size</arg> <type>(uint64)</type> - The file size in bytes.</li>

   <li><arg>machine</arg> <type>(string)</type> - The type of machine the binary
   is built for, for example "X86-64", "X86" or "ARM".</li>

   <li><arg>address-width</arg> <type>(uint64)</type> - The address width the
   file was built for, should be 32 or 64, but 0 specifies unknown.</li>

   </ul>


   <ndx>sections_info!debug_symbol_file interface method</ndx>
   <fun>sections_info</fun> provides information about the sections in the
   symbol file. The <arg>ctx_id</arg> must be an id for a file opened with
   <fun>open_symbol_file</fun> or <fun>open_symbol_section</fun> in this
   interface. The returned result is a list with two elements, the first a
   string specifying the format of the file, "ELF" or "PE", and the second a
   list of dictionaries where each dictionary contains information about the
   section. If an id from <fun>open_symbol_section</fun> is used then only the
   opened section will be included in the list. The following keys exist in the
   dictionary:

   <ul>

   <li><arg>name</arg> <type>(string)</type> - The name of the section. This key
   could potentially be left out if the sections does not have a name.</li>

   <li><arg>address</arg> <type>(uint64)</type> - The address in memory.</li>

   <li><arg>offset</arg> <type>(uint64)</type> - The offset in the file.</li>

   <li><arg>size</arg> <type>(uint64)</type> - The section size in the file
   image.</li>

   <li><arg>executable</arg> <type>(boolean)</type> - A boolean telling if the
   section is executable or not.</li>

   <li><arg>index</arg> <type>(uint64)</type> - Only available for ELF
   files. The section index.</li>

   <li><arg>flags</arg> <type>(uint64)</type> - Only available for ELF
   files. The value of <tt>sh_flags</tt> in the ELF section header.</li>

   <li><arg>characteristics</arg> <type>(uint64)</type> - Only available for PE
   files. The value of <tt>Characteristics</tt> in the PE section
   <tt>IMAGE_SECTION_HEADER</tt> structure.</li> </ul>

   <ndx>segments_info!debug_symbol_file interface method</ndx>
   <fun>segments_info</fun> provides information about the segments in the
   symbol file, this is only supported for ELF. The <arg>ctx_id</arg> must be
   an id for a file opened with <fun>open_symbol_file</fun> in this
   interface. The returned result is a list where each entry represent a
   segment. Each entry is in turn a dictionary with the following keys:

   <ul>

   <li><arg>address</arg> <type>(uint64)</type> - Virtual address in
   memory.</li>

   <li><arg>size</arg> <type>(uint64)</type> - The segment size in
   memory, specified in bytes.</li>

   <li><arg>offset</arg> <type>(uint64)</type> - Offset into the segment
   location on file.</li>

   <li><arg>flags</arg> <type>(uint64)</type> - Flags, depending on segment
   type, corresponds to <tt>p_flags</tt> in the ELF program header table.</li>

   <li><arg>type</arg> <type>(uint64)</type> - The type of the segment,
   corresponds to <tt>p_type</tt> in the ELF program header table.</li>

   <li><arg>physical-address</arg> <type>(uint64)</type> - Physical address in
   memory, if applicable.</li>

   <li><arg>file-size</arg> <type>(uint64)</type> - The size of the segment in
   file.</li>

   <li><arg>sections</arg> <type>([string,...])</type> - A list of sections
   included in the segment, presented by the section name.</li>

   </ul>

   <insert-until text="// ADD INTERFACE debug_symbol_file_interface"/>
   </add>

   <add id="debug_symbol_file_interface_exec_context">
   Global Context for all methods.
   </add>
 */

SIM_INTERFACE(debug_symbol_file) {
        attr_value_t (*open_symbol_file)(conf_object_t *NOTNULL obj,
                                         const char *NOTNULL filename,
                                         uint64 address, bool absolute_address);
        attr_value_t (*close_symbol_file)(conf_object_t *NOTNULL obj,
                                         const char *NOTNULL ctx_id);
        attr_value_t (*symbol_file_info)(conf_object_t *NOTNULL obj,
                                         const char *NOTNULL ctx_id);
        attr_value_t (*sections_info)(conf_object_t *NOTNULL obj,
                                      const char *NOTNULL ctx_id);
        attr_value_t (*segments_info)(conf_object_t *NOTNULL obj,
                                      const char *NOTNULL ctx_id);
        attr_value_t (*open_symbol_section)(conf_object_t *NOTNULL obj,
                                            const char *NOTNULL filename,
                                            const char *NOTNULL section,
                                            uint64 address,
                                            bool absolute_address);
};
#define DEBUG_SYMBOL_FILE_INTERFACE "debug_symbol_file"
// ADD INTERFACE debug_symbol_file_interface

#ifdef __cplusplus
}
#endif

#endif /* ! SIMICS_SIMULATOR_IFACE_DEBUGGER_H */
