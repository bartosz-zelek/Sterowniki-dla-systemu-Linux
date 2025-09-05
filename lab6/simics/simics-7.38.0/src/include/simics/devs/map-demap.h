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

#ifndef SIMICS_DEVS_MAP_DEMAP_H
#define SIMICS_DEVS_MAP_DEMAP_H

#include <simics/base/types.h>
#include <simics/base/memory.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="map_demap_interface_t">
   Interface used to dynamically add and remove mappings in a memory space.
   The first argument to all functions, <param>space</param> is the memory
   space to map an object in.

   The <fun>map_simple</fun> function adds a device/port pair to the
   memory map. The added device (port) must implement either the
   <iface>ram</iface>, <iface>rom</iface>, <iface>io_memory</iface>,
   <iface>port_space</iface>, <iface>translator</iface>,
   <iface>transaction_translator</iface>, <iface>transaction</iface> or
   <iface>memory_space</iface> interfaces.
   
   The <fun>map_bridge</fun> function adds a device/port pair to the
   memory map to act as a bridge that translates memory accesses
   before passing them on to another object. The added device (port)
   must implement either the <iface>translate</iface> interface or the
   <iface>bridge</iface> interface, and the <param>target</param> and
   <param>target_port</param> parameter specifies the object behind
   the bridge. The target is typically another memory space.
   
   The <fun>unmap</fun> removes an object from the memory map. The
   <param>dev</param> and <param>port</param> parameters are the same
   as those given to the <fun>map_simple</fun> and
   <fun>map_bridge</fun> functions.
   
   The <fun>unmap_address</fun> removes all mappings that map
   <param>object</param> and start at the <param>base</param> address.
   
   The <fun>add_map</fun>, <fun>remove_map</fun>,
   <fun>add_default</fun> and <fun>remove_default</fun> functions are
   deprecated and should not be used.
   Both <tt>base</tt> and <tt>length</tt> of the <tt>map_info_t</tt> structure
   should be 0 to map a <tt>default_target</tt>.

   All functions in the <iface>map_demap</iface> return 1 on success and 0 on
   failure.

   More information about the different kinds of memory space mappings is
   available in Simics Model Builder User's Guide.

   <insert-until text="// ADD INTERFACE map_demap_interface"/>
   </add>
   <add id="map_demap_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(map_demap) {
        /* old-style */
        int (*add_map)(conf_object_t *NOTNULL space,
                       conf_object_t *NOTNULL dev,
                       conf_object_t *target,
                       map_info_t map_info);
        int (*remove_map)(conf_object_t *NOTNULL space,
                          conf_object_t *NOTNULL dev,
                          int function);
        int (*add_default)(conf_object_t *NOTNULL space,
                           conf_object_t *NOTNULL dev,
                           conf_object_t *target,
                           map_info_t map_info);
        void (*remove_default)(conf_object_t *NOTNULL space);

        /* new-style */
        int (*map_simple)(conf_object_t *NOTNULL space,
                          conf_object_t *NOTNULL dev,
                          const char *dev_port,
                          map_info_t map_info);
        int (*map_bridge)(conf_object_t *NOTNULL space,
                          conf_object_t *NOTNULL dev,
                          const char *dev_port,
                          conf_object_t *target,
                          const char *target_port,
                          map_info_t map_info);
        int (*unmap)(conf_object_t *NOTNULL space,
                     conf_object_t *NOTNULL dev,
                     const char *dev_port);
        int (*unmap_address)(conf_object_t *NOTNULL space,
                             conf_object_t *NOTNULL dev,
                             physical_address_t base,
                             const char *dev_port);
};

#define MAP_DEMAP_INTERFACE "map_demap"
// ADD INTERFACE map_demap_interface

#if defined(__cplusplus)
}
#endif

#endif
