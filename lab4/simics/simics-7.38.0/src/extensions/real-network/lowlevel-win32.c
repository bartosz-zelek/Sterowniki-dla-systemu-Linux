/*
  © 2020 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "real-network.h"
#include "lowlevel.h"

#ifdef _WIN32
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

/* Wrap GetAdaptersInfo() and IP_ADAPTER_INFO to make them a little nicer
   to work with. */
typedef IP_ADAPTER_INFO adapter_info_t;

static inline adapter_info_t *
get_next_adapter(adapter_info_t *adapter)
{
        return adapter->Next; 
}

static inline char *
get_adapter_guid(adapter_info_t *adapter)
{
        return adapter->AdapterName;
}

/* Return the adapter's IP address (host byte order) */
static inline uint32
get_adapter_ip(adapter_info_t *adapter)
{
        return CONVERT_BE32(inet_addr(
                                    adapter->IpAddressList.IpAddress.String));
}

/* Return the adapter's net mask (host byte order) */
static inline uint32
get_adapter_netmask(adapter_info_t *adapter)
{
        return CONVERT_BE32(inet_addr(adapter->IpAddressList.IpMask.String));
}

/* Wrapper to hide the quirks of GetAdaptersInfo(). Returns a linked list
   of adapter_info_t structs. */
static adapter_info_t *
get_adapter_list()
{
        ULONG len = sizeof(adapter_info_t);

        /* First, allocate a buffer the length of a adapter_info_t  */
        adapter_info_t *list = MM_MALLOC_SZ(len, adapter_info_t);
        DWORD result = GetAdaptersInfo(list, &len);

        /* If that was not enough, enlarge buffer */
        if (result == ERROR_BUFFER_OVERFLOW) {
                list = MM_REALLOC_SZ(list, len, adapter_info_t);
                result = GetAdaptersInfo(list, &len);
        }

        if (result != NO_ERROR) {
                MM_FREE(list);
                list = NULL;
        }

        return list;
}

/* Deallocate list returned by get_adapter_list(). */
static void
free_adapter_list(adapter_info_t *list)
{
        MM_FREE(list);
}


/* Given a list of adapters, return the one having the specified device
   GUID.  Returns NULL on failure. */
static adapter_info_t *
find_adapter_by_guid(conf_object_t *obj, adapter_info_t *list,
                     const char *guid)
{
        adapter_info_t *adapter;

        for (adapter = list; adapter != NULL;
             adapter = get_next_adapter(adapter)) {
                if (strcmp(get_adapter_guid(adapter), guid) == 0)
                        return adapter;
        }

        return NULL;
}

/* Obtain ip, netmask, and mac_address of a network interface
   given its GUID. */
int
win32_get_interface_info(conf_object_t *obj, const char *guid,
                         ip_addr_t *ip, ip_addr_t *mask)
{
        adapter_info_t *list = get_adapter_list();
        if (list == NULL)
                return -1;

        adapter_info_t *adapter = find_adapter_by_guid(obj, list, guid);
        if (adapter == NULL) {
                free_adapter_list(list);
                return -1;
        }

        ip_address_set_ipv4_host(ip, get_adapter_ip(adapter));
        ip_address_set_ipv4_host(mask, get_adapter_netmask(adapter));

        free_adapter_list(list);
        return 0;
}

#endif /* _WIN32 */
