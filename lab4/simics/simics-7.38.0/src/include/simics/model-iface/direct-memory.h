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

#ifndef SIMICS_MODEL_IFACE_DIRECT_MEMORY_H
#define SIMICS_MODEL_IFACE_DIRECT_MEMORY_H

#include <simics/base/types.h>
#include <simics/processor/types.h>
#include <simics/base/direct-memory.h>
#include <simics/base/transaction.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
   <add id="direct_memory_interface_t">
   <ndx>direct_memory_handle_t</ndx>
   <ndx>direct_memory_ack_id_t</ndx>
   <ndx>direct_memory_t</ndx>

   The <iface>direct_memory</iface> interface is implemented by objects that
   model memory, such as RAM and ROM objects. These are called direct-memory
   objects. A user of the interface is called a memory user and is
   typically a processor that wants to do fast accesses to memory. The
   direct-memory object corresponding to a particular physical address
   can be obtained using the <fun>lookup</fun> method of the
   <iface>direct_memory_lookup</iface> interface.
   See the documentation for the
   <iface>direct_memory_lookup</iface> interface for more information.

   A memory user using the <iface>direct_memory</iface> interface 
   must implement the <iface>direct_memory_update</iface> interface.

   The <fun>get_handle</fun> method is used by a memory user
   to create or retrieve a handle to the memory region starting
   at offset <param>offs</param> with size <param>size</param>.
   The handle is typically used later on to request access permissions
   and to retrieve a direct pointer to the region.
   The handle returned by <fun>get_handle</fun> is private to the
   memory user specified in the <param>requester</param> parameter.

   If <fun>get_handle</fun> is invoked multiple times for the same
   range, and with identical <param>requester</param> and
   <param>subsystem</param> arguments, then the same handle will
   be returned each time, assuming the original handle is still valid.
   Note that the original handle is only returned if the range
   matches exactly. A single memory user can obtain multiple
   distinct handles for the same memory range by using different values for the
   <param>subsystem</param> parameter.

   For RAM and ROM, <param>offs</param> and <param>size</param> must
   specify a region which does not intersect a naturally aligned
   8192 byte boundary, or the request will fail with a NULL return
   value. Other direct-memory objects might have different requirements.   

   The <fun>request</fun> method is used to request a host pointer to simulated
   memory. This pointer can be used to carry out fast memory operations without
   having to involve the Simics API. The <arg>handle</arg> argument is the
   handle obtained using <fun>get_handle</fun>.

   Both the <arg>permission</arg> argument and the <arg>inhibit</arg> argument
   are <type>access_t</type> type bit fields. The <arg>permission</arg>
   argument is used to specify what kind of memory operations the memory user
   will perform. For example, if a memory user wants to read memory, the
   <arg>permission</arg> argument must include the Sim_Access_Read value.  The
   <arg>inhibit</arg> argument specifies what other memory users are not
   allowed to do. For example, if <arg>inhibit</arg> is set to Sim_Access_Write
   other memory users are not allowed to write to the memory range.  This
   protection mechanism can be used to create caches of simulated memory,
   request exclusive permissions to a memory range in order to carry out atomic
   operations, and similar.  When a memory user is requesting permission to a
   memory range that another memory user has protected with conflicting inhibit
   bits, the direct-memory object will inform the other memory user of
   the lost permissions and protection through the
   <iface>direct_memory_update</iface> interface.  A user can lose both the
   permission and protection for a memory range in this way. When this happens,
   a memory user may re-request permissions and inhibit protection.

   Note: if a memory user has multiple handles which overlaps, then
   each handle is considered to be a distinct memory user. For example,
   if a memory user holds two handles, and requests write inhibit on
   one of them, then write permission will be revoked from the second
   handle (if such permission had been granted).

   The <fun>request</fun> method returns a <type>direct_memory_t</type> value
   with information about the retrieved permissions and inhibit bits. These
   bits can be a super set of the bits that actually were requested. The
   returned data pointer can be used to access the memory range. Accesses are
   valid from the data pointer and up to the end of the range, i.e., addresses
   up to data pointer + <arg>size</arg> - 1, where size is the size valid for
   the handle. A call to <fun>request</fun> always succeeds and the
   corresponding memory range is valid until the permissions or the handle are
   revoked by the <iface>direct_memory_update</iface> interface.  Note that the
   data pointer may change each time <fun>request</fun> is called (with the
   same handle) since Simics may move simulated memory. If the pointer
   changes, then the old pointer must not be used.

   With <fun>set_user_data</fun>, a memory user can associate 
   a user-defined pointer with a specific handle. The pointer can
   be retrieved using the <fun>get_user_data</fun> method, which takes
   a <param>handle</param> as an argument.

   A memory user can use the <fun>release</fun> function to notify the
   direct-memory object when it is no longer interested in the memory
   region corresponding to <param>handle</param>. The handle is invalid
   and must not be used for anything after being released.
   
   The <fun>ack</fun> method is used by a memory user to inform the 
   direct-memory object that it has given up the corresponding permission and
   inhibit rights for a memory range when called by a method in the
   <iface>direct_memory_update</iface> interface.

   Permissions can be revoked from all memory users by invoking the
   <fun>revoke</fun> method. The <param>permission</param> parameter
   specifies the permissions which will be revoked from all memory users.
   Similarly, <param>inhibit</param> specifies the inhibit privileges which
   will be revoked. For instance, calling <fun>revoke</fun> with
   <param>permission</param>
   set to <const>Sim_Access_Write</const> will ensure that nobody has
   write permissions to the direct-memory object.

   <insert id="direct_memory_handle_t"/>
   <insert id="direct_memory_t"/>
   <insert id="direct_memory_ack_id_t"/>
   <insert-until text="// ADD INTERFACE direct_memory_interface_t"/>
   </add>
   <add id="direct_memory_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(direct_memory) {
        direct_memory_handle_t (*get_handle)(conf_object_t *NOTNULL obj,
                                             conf_object_t *NOTNULL requester,
                                             uint64 subsystem,
                                             uint64 offs,
                                             unsigned size);
        direct_memory_t (*request)(conf_object_t *NOTNULL obj,
                                   direct_memory_handle_t handle,
                                   access_t permission,
                                   access_t inhibit);
        void (*revoke)(conf_object_t *NOTNULL obj,
                       access_t access,
                       access_t permission,
                       access_t inhibit);
#ifndef PYWRAP
        void *(*get_user_data)(conf_object_t *NOTNULL obj,
                                    direct_memory_handle_t handle);
        void (*set_user_data)(conf_object_t *NOTNULL obj,
                              direct_memory_handle_t handle,
                              void *user_data);
#endif
        void (*release)(conf_object_t *NOTNULL obj,
                        direct_memory_handle_t handle);
        void (*ack)(conf_object_t *NOTNULL obj,
                    direct_memory_ack_id_t id);
};

#define DIRECT_MEMORY_INTERFACE "direct_memory"
// ADD INTERFACE direct_memory_interface_t

/*  
   <add id="direct_memory_lookup_interface_t">
   <ndx>direct_memory_lookup_t</ndx>

   The <iface>direct_memory_lookup</iface> interface is implemented by Simics
   memory-spaces. The interface is used by simulator objects that want to do
   fast accesses to memory and/or want to build up a cached representation of
   memory. These objects are referred to as memory users, e.g., processors.
   Fast accesses are done via host pointers to simulated memory. The
   <iface>direct_memory_lookup</iface> interface is used in conjunction with
   the <iface>direct_memory</iface> interface which is implemented by objects
   that own the actual data storage, e.g., RAM/ROM objects. These objects are
   called direct-memory objects.

   To access data, a memory-user object first calls the <fun>lookup</fun>
   method on the memory space <param>obj</param>. The
   <param>requester</param> is the memory-user doing the lookup.
   The <fun>lookup</fun> method traces the range specified by
   <param>addr</param> and <param>size</param>
   through memory spaces and translators until a direct-memory object
   is reached. The direct-memory object is returned in the
   <param>target</param> field and the offset into this object
   corresponding to <param>addr</param> is returned in the
   <param>offs</param> field.

   The call to <fun>lookup</fun> fails if the specified range does
   not map continuously to a direct-memory object. A lookup failure
   is indicated by returning NULL in the <arg>target</arg> field.

   The <arg>access</arg> argument is a bit field of at least one
   <type>access_t</type> value specifying what kind of accesses the memory user
   is interested in. All specified access types must reach the same
   direct-memory object and range for the lookup to succeed. If the memory
   space, for example, redirects reads and writes to different memory ranges or
   direct-memory objects, a lookup would fail if <arg>access</arg>
   specified both read and write. Note that the actual access permissions
   needed to access the real data must be requested from
   the direct-memory object using the <fun>request</fun> method of
   the <iface>direct_memory</iface> interface. The <arg>access</arg>
   argument is only used to locate the direct-memory object.

   The return field <var>access</var> contains at least the access
   bits requested used in the lookup request, but may as an optimization
   contain a superset, indicating that the lookup result is valid for this
   superset. However, there is no guarantee that this optimization
   takes place.

   Once a direct-memory object has been found, the <iface>direct_memory</iface>
   interface can be used to obtain a direct pointer to the contents
   of the direct-memory object.

   The <var>tracers</var> and <var>breakpoints</var> fields in the
   return value contain information about installed tracers and breakpoints
   that intersect the range. Examples of tracers are timing models
   and snoop objects. In order to trigger breakpoints and invoke any tracers,
   the memory user should perform memory operations using the
   <iface>memory_space</iface> interface. Only breakpoints and tracers that
   overlap (binary and) with the provided <arg>access</arg> argument need to be
   considered.

   <insert id="direct_memory_lookup_t"/>
   <insert-until text="// ADD INTERFACE direct_memory_lookup_interface_t"/>

   The <iface>direct_memory_lookup</iface> and <iface>direct_memory</iface>
   interfaces replace the <iface>memory_page</iface> interface of Simics 4.8.
   </add>

   <add id="direct_memory_lookup_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(direct_memory_lookup) {
        direct_memory_lookup_t (*lookup)(conf_object_t *NOTNULL obj,
                                         conf_object_t *requester,
                                         physical_address_t addr,
                                         unsigned size,
                                         access_t access);
};

#define DIRECT_MEMORY_LOOKUP_INTERFACE "direct_memory_lookup"
// ADD INTERFACE direct_memory_lookup_interface_t

/*
   <add id="direct_memory_lookup_v2_interface_t">
   <ndx>direct_memory_lookup_v2_t</ndx>

   The <iface>direct_memory_lookup_v2</iface> interface is implemented by
   Simics memory-spaces. Its functionality is identical to the
   <iface>direct_memory_lookup</iface> interface except that it takes a
   <type>transaction_t</type> argument instead of a requester object and
   size. It must be used instead of <iface>direct_memory_lookup</iface> in
   cases when memory mappings may depend on atoms of the transaction in
   addition to the physical address.

   <insert-until text="// ADD INTERFACE direct_memory_lookup_v2_interface_t"/>
   </add>

   <add id="direct_memory_lookup_v2_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(direct_memory_lookup_v2) {
        direct_memory_lookup_t (*lookup)(conf_object_t *NOTNULL obj,
                                         transaction_t *NOTNULL transaction,
                                         physical_address_t addr,
                                         access_t access);
};

#define DIRECT_MEMORY_LOOKUP_V2_INTERFACE "direct_memory_lookup_v2"
// ADD INTERFACE direct_memory_lookup_v2_interface_t

/*
   <add id="direct_memory_update_interface_t">

   The <iface>direct_memory_update</iface> interface must be implemented by
   memory-user objects that use the <iface>direct_memory</iface> interface.

   The <iface>direct_memory_update</iface> interface replaces the
   <iface>memory_page_update</iface> interface from Simics 4.8.

   Accesses to memory are controlled by a handle that the memory-user object
   requests by calling the <fun>get_handle</fun> method of the
   <iface>direct_memory</iface> interface. The object implementing the
   <iface>direct_memory</iface> interface through which the handle was
   established is passed to the functions in
   <iface>direct_memory_update</iface> as <arg>target</arg>.

   If the <fun>release</fun> method is called, the corresponding
   <arg>handle</arg> and all the permissions and inhibit protections are
   recalled. The memory-user object must stop using the handle and
   associated data pointers and then call the acknowledge method
   <fun>ack</fun> in the <iface>direct_memory</iface> interface from
   which the handle was granted.

   A call to the <fun>update_permission</fun> method revokes earlier requested
   rights for a handle. The <arg>lost_access</arg> argument recalls rights to
   use the handle for the given access bits. This means that the handle needs
   to be re-fetched (by a call to the <fun>lookup</fun> method of the
   <iface>direct_memory_lookup</iface> interface followed by a call to the
   <fun>get_handle</fun> method of the <iface>direct_memory</iface> interface)
   before the handle can be used again for the particular access. This typically
   happens if a new breakpoint is inserted or a remap of the memory system is
   done. In case of a remap it is possible that the same handle will never be
   returned again which means that any user data associated with the handle
   should be reclaimed. 

   The <arg>lost_permission</arg> and the <arg>lost_inhibit</arg> arguments
   describe which permission rights and inhibit protection that are
   revoked. However, in contrast to the <arg>lost_access</arg>, the
   <arg>handle</arg> is still valid and can be used to re-request permission
   rights and inhibit protection.

   A call to the <fun>conflicting_access</fun> method informs a memory-user
   object that a conflicting memory operation will be performed. Hence, if the
   memory-user object have some protected representation of memory (such as
   decoded instructions in an internal cache), that representation of memory
   has to be flushed (or written back to memory in case of dirty data). Note
   however that the memory-user object does not lose any permission rights or
   any inhibit protection. 

   There is no mechanism for locking simulated memory in host memory.

   All methods in this interface receive a <type>direct_memory_ack_id_t</type>
   value as an argument. The <fun>ack</fun> method of the
   <iface>direct_memory</iface> interface must be called with this
   <arg>id</arg> as an argument when the corresponding operation has been
   carried out. The <fun>ack</fun> method may be called after the
   <iface>direct_memory_update</iface> interface function has returned, which
   allows for queueing of update requests. This may be valuable if multiple
   simulator threads are used.

   An exception to the allowed queueing of update requests is for update
   requests that are received while calling <fun>request</fun> in the
   <iface>direct_memory</iface> interface. Such requests must be handled
   immediately with <fun>ack</fun> being called before the return of the
   <iface>direct_memory_update</iface> interface function. This requirement
   avoids a dead-lock that would otherwise happen if <fun>request</fun> would
   wait for <fun>ack</fun> before returning, but <fun>ack</fun> is
   queued to be handled at some time after <fun>request</fun> has returned.

   <insert-until text="// ADD INTERFACE direct_memory_update_interface_t"/>
   </add>
   <add id="direct_memory_update_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(direct_memory_update) {
        void (*release)(conf_object_t *NOTNULL obj,
                        conf_object_t *NOTNULL target,
                        direct_memory_handle_t handle,
                        direct_memory_ack_id_t id);
        void (*update_permission)(conf_object_t *NOTNULL obj,
                                  conf_object_t *NOTNULL target,
                                  direct_memory_handle_t handle,
                                  access_t lost_access,
                                  access_t lost_permission,
                                  access_t lost_inhibit,
                                  direct_memory_ack_id_t id);
        void (*conflicting_access)(conf_object_t *NOTNULL obj,
                                   conf_object_t *NOTNULL target,
                                   direct_memory_handle_t handle,
                                   access_t conflicting_permission,
                                   direct_memory_ack_id_t id);
};

#define DIRECT_MEMORY_UPDATE_INTERFACE "direct_memory_update"
// ADD INTERFACE direct_memory_update_interface_t


/* <add id="direct_memory_flush_interface_t">
     The <iface>direct_memory_flush</iface> interface is implemented by objects
     that model memory, such as RAM and ROM objects, and is used for flushing
     granted rights and for managing access rights.

     The <fun>revoke</fun> method revokes granted access, permissions and
     inhibit rights from memory-user regions intersecting
     [<param>base</param>, <param>base</param> + <param>size</param>).

     The <fun>set_access_bits</fun> method grants access rights
     <param>access</param> for the region [<param>base</param>,
     <param>base</param> + <param>size</param>) to the memory user
     <param>requester</param>. If <param>requester</param> is NULL, then
     access rights are granted to all memory users. If the set succeeds, true is
     returned, otherwise false.

     <insert-until text="// ADD INTERFACE direct_memory_flush_interface_t"/>
   </add>

   <add id="direct_memory_flush_interface_exec_context">
     Cell Context for all methods.
   </add> */
SIM_INTERFACE(direct_memory_flush) {
        void (*revoke)(conf_object_t *NOTNULL obj,
                       uint64 base, uint64 size,
                       access_t access, access_t perm, access_t inhibit);
        bool (*set_access_bits)(conf_object_t *NOTNULL obj,
                                conf_object_t *requester,
                                uint64 base, uint64 size,
                                access_t access);
};
#define DIRECT_MEMORY_FLUSH_INTERFACE "direct_memory_flush"
// ADD INTERFACE direct_memory_flush_interface_t


/* <add id="direct_memory_tags_interface_t">
   <ndx>direct_memory_tags_t</ndx>

   The <iface>direct_memory_tags</iface> interface is implemented by objects
   that model RAM memory with support for auxiliary RAM bits.

   The <fun>get_tags_data</fun> method returns a 
   <type>direct_memory_tags_t</type> value which contains 
   a direct pointer to the memory used to store the tags bits.
   The <arg>handle</arg> argument should be a handle for a region of memory
   previously obtained from the <fun>get_handle</fun> method of
   the <iface>direct_memory</iface> interface. 

   NOTE: The memory region specified indirectly by the <arg>handle</arg>
   argument will be enlarged, if necessary, to have a natural 128-byte
   alignment. This is done to ensure that the tags mapping is unambiguous.

   The returned data pointer points to tag bits for the region
   specified by <arg>handle</arg>. The least significant bit of the
   first byte corresponds to the first 16 bytes of the (aligned) region.

   The <arg>len</arg> field is set to the number of bytes holding
   tags data that may be accessed and it equals the size of the
   (aligned) region divided by 128.

   The augmented memory bits may be read or modified using the
   returned pointer, provided that the user has corresponding
   read or write permissions to the region specified by <arg>handle</arg>.

   The returned pointer will remain valid until the corresponding
   permissions to the direct memory region are lost, which usually
   happens through a call to the <fun>update_permission</fun> method of the
   <iface>direct_memory_update</iface> interface. The returned pointer
   must also be considered invalid when additional permissions
   for the region are requested using the <fun>request</fun> method of
   the <iface>direct_memory</iface> interface. This is necessary since
   e.g. a write request could trigger copy-on-write behavior and
   reallocation of the underlying storage.
   
   <insert id="direct_memory_tags_t"/>
   <insert-until text="// ADD INTERFACE direct_memory_tags_interface_t"/>
   </add>

   <add id="direct_memory_tags_interface_exec_context">
      Cell Context for all methods.
   </add> */

SIM_INTERFACE(direct_memory_tags) {
        direct_memory_tags_t (*get_tags_data)(conf_object_t *NOTNULL obj,
                                              direct_memory_handle_t handle);
};
#define DIRECT_MEMORY_TAGS_INTERFACE "direct_memory_tags"        
// ADD INTERFACE direct_memory_tags_interface_t

        
#if defined(__cplusplus)
}
#endif
        
#endif
