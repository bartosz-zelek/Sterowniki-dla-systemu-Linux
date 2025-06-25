/*
  © 2012 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifdef _WIN32

#include "real-network.h"
#include "lowlevel.h"

#include <windows.h>
#include <winioctl.h>

#include <simics/util/os.h>

#define TAP_CONTROL_CODE(request) CTL_CODE(FILE_DEVICE_UNKNOWN, request, \
                                           METHOD_BUFFERED, FILE_ANY_ACCESS)

#define TAP_IOCTL_GET_VERSION           TAP_CONTROL_CODE(2)
#define TAP_IOCTL_SET_MEDIA_STATUS      TAP_CONTROL_CODE(6)

#define USERMODEDEVICEDIR "\\\\.\\Global\\"
#define TAPSUFFIX         ".tap"

typedef enum {
        DT_NORMAL = 0,
        DT_OVPN_TAP,
        DT_WR_TAP
} device_type_t;

typedef struct {
        char *name;
        char *guid;
        device_type_t type;
        int32 file_index;    /* Used if type is DT_WR_TAP */
} host_device_t;

typedef struct {
        VECT(host_device_t*) devices;
} host_device_list_t;

static tap_device_t *
alloc_tap_device(conf_object_t *obj)
{
        tap_device_t *dev = MM_ZALLOC(1, tap_device_t);
        dev->obj = obj;
        dev->handle = INVALID_HANDLE_VALUE;

        dev->overlapped.read.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        dev->overlapped.write.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        return dev;
}

static void
dealloc_tap_device(tap_device_t *dev)
{
        CloseHandle(dev->overlapped.read.hEvent);
        CloseHandle(dev->overlapped.write.hEvent);
        MM_FREE(dev->name);
        MM_FREE(dev->guid);
}

/* Return a host_device_t * if the device denoted by the specified GUID exists
   on the host system, else NULL. */
static host_device_t *
get_device(conf_object_t *obj, char *device_guid)
{
        strbuf_t sb = SB_INIT;
        host_device_t *ret_dev = NULL;
        HKEY card_key = NULL, unit_key = NULL;

        LONG result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, ADAPTER_KEY,
                                   0, KEY_READ, &card_key);
        if (result != ERROR_SUCCESS) {
                if (obj) {
                        char *oserr = os_describe_last_error();
                        SIM_LOG_ERROR(obj, 0,
                                      "Failed to open registry key \"%s\": %s",
                                      ADAPTER_KEY, oserr);
                        MM_FREE(oserr);
                }
                return NULL;
        }

        for (int i = 0; TRUE; i++) {
                char name[MAX_KEYLEN];
                DWORD len = sizeof(name);
                result = RegEnumKeyEx(card_key, i, name, &len,
                                      NULL, NULL, NULL, NULL);
                if (result == ERROR_NO_MORE_ITEMS) {
                        ret_dev = NULL;
                        break;
                }

                if (result != ERROR_SUCCESS) {
                        if (obj) {
                                char *oserr = os_describe_last_error();
                                SIM_LOG_ERROR(obj, 0,
                                              "Failed to query subkeys: %s",
                                              oserr);
                                MM_FREE(oserr);
                        }
                        ret_dev = NULL;
                        goto out;
                }

                sb_fmt(&sb, "%s\\%s", ADAPTER_KEY, name);
                if (obj) {
                        SIM_LOG_INFO(4, obj, 0,
                                     "Opening registry key: HKLM\\%s",
                                     sb_str(&sb));
                }

                result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sb_str(&sb),
                                      0, KEY_READ, &unit_key);
                if (result != ERROR_SUCCESS)
                        continue;

                char comp_id[MAX_DATALEN];
                len = sizeof(comp_id);
                DWORD data_type;
                result = RegQueryValueEx(unit_key, "ComponentId", NULL,
					 &data_type, (uint8 *)comp_id, &len);
                if (result != ERROR_SUCCESS || data_type != REG_SZ)
                        continue;
                if (obj) {
                        SIM_LOG_INFO(4, obj, 0, "ComponentId(%s) = %s", name,
                                     comp_id);
                }

                char net_cfg_id[MAX_DATALEN];
                len = sizeof(net_cfg_id);
                result = RegQueryValueEx(unit_key, "NetCfgInstanceId", NULL,
					 &data_type, (uint8 *)net_cfg_id,
					 &len);
                if (result != ERROR_SUCCESS || data_type != REG_SZ)
                        continue;
                if (obj) {
                        SIM_LOG_INFO(4, obj, 0, "NetCfgInstanceId(%s) = %s",
                                     name, net_cfg_id);
                }
                
                /* Is this a recognized TAP device? */
                bool is_tap = (strncmp(comp_id, "tap", 3) == 0
                               || strncmp(comp_id, "isa\\wrs_tap", 11) == 0
                               || strncmp(comp_id, "wrtap", 5) == 0);

                if (strcmp(net_cfg_id, device_guid) == 0) {
                        int32 file_index;
                        device_type_t dtype = DT_NORMAL;

                        /* Is this a Wind River TAP device? */
                        if (strncmp(comp_id, "isa\\wrs_tap", 11) == 0
                            || strncmp(comp_id, "wrtap", 5) == 0) {
                                dtype = DT_WR_TAP;
                                len = sizeof(file_index);
                                result = RegQueryValueEx(unit_key, "FileIndex",
                                                         NULL, &data_type,
                                                         (uint8 *)&file_index,
                                                         &len);
                                if (result != ERROR_SUCCESS
                                    || data_type != REG_DWORD)
                                        continue;
                        } else if (is_tap) {
                                dtype = DT_OVPN_TAP;
                        }

                        ret_dev = MM_MALLOC(1, host_device_t);
                        ret_dev->type = dtype;
                        ret_dev->file_index = file_index;
                        goto out;
                }
         }

  out:
        RegCloseKey(unit_key);
        RegCloseKey(card_key);
        sb_free(&sb);
        return ret_dev;
}

/* Return a list of existing network devices. */
static host_device_list_t
enumerate_devices(conf_object_t *obj)
{
        strbuf_t sb = SB_INIT;
        host_device_list_t device_list;

        VINIT(device_list.devices);

        HKEY control_key;
        LONG result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, NETWORK_CONNECTION_KEY,
                                   0, KEY_READ, &control_key);
        if (result != ERROR_SUCCESS) {
                if (obj) {
                        char *oserr = os_describe_last_error();
                        SIM_LOG_ERROR(obj, 0,
                                      "Failed to open registry key \"%s\": %s",
                                      NETWORK_CONNECTION_KEY, oserr);
                        MM_FREE(oserr);
                }
                return device_list;
        }

        for (int i = 0; TRUE; i++) {
                char data[MAX_DATALEN];

                char device_guid[MAX_KEYLEN];
                DWORD len = sizeof(device_guid);
                result = RegEnumKeyEx(control_key, i, device_guid, &len,
                                      NULL, NULL, NULL, NULL);
                if (result == ERROR_NO_MORE_ITEMS)
                        break;

                if (result != ERROR_SUCCESS) {
                        if (obj) {
                                char *oserr = os_describe_last_error();
                                SIM_LOG_ERROR(obj, 0,
                                              "Failed to enumerate values: %s",
                                              oserr);
                                MM_FREE(oserr);
                        }
                        break;
                }

                sb_fmt(&sb, "%s\\%s\\Connection",
                       NETWORK_CONNECTION_KEY, device_guid);

                HKEY connection_key;
                result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sb_str(&sb),
                                      0, KEY_READ, &connection_key);
                if (result != ERROR_SUCCESS)
                        continue;

                len = sizeof(data);
                DWORD name_type;
                result = RegQueryValueEx(connection_key, "Name", NULL,
					 &name_type, (uint8 *)data, &len);
                if (result != ERROR_SUCCESS || name_type != REG_SZ) {
                        if (obj) {
                                char *oserr = os_describe_last_error();
                                SIM_LOG_ERROR(obj, 0,
                                              "Failed to query values: %s",
                                              oserr);
                                MM_FREE(oserr);
                        }
                        continue;
                }

                host_device_t *dev = get_device(obj, device_guid);
                if (dev) {
                        if (obj) {
                                SIM_LOG_INFO(3, obj, 0,
                                             "Found TAP device: %s", data);
                        }
                        dev->name = MM_STRDUP(data);
                        dev->guid = MM_STRDUP(device_guid);
                        VADD(device_list.devices, dev);
                }
        }

        sb_free(&sb);
        return device_list;
}

/* Return a list of existing TAP network devices. */
static tap_device_list_t
enumerate_tap_devices(conf_object_t *obj)
{
        host_device_list_t host_devices = enumerate_devices(obj);

        tap_device_list_t tap_devices;
        VINIT(tap_devices.devices);

        host_device_t **iter;
        VFOREACH(host_devices.devices, iter) {
                host_device_t *hdev = *iter;
                if (hdev->type != DT_OVPN_TAP && hdev->type != DT_WR_TAP) {
                        MM_FREE(hdev->name);
                        MM_FREE(hdev->guid);
                        continue;
                }
                tap_device_t *tdev = alloc_tap_device(obj);
                tdev->name = hdev->name;
                tdev->guid = hdev->guid;
                if (hdev->type == DT_WR_TAP) {
                        tdev->is_wrtap = true;
                        tdev->file_index = hdev->file_index;
                }
                VADD(tap_devices.devices, tdev);
        }

        while (VLEN(host_devices.devices) > 0) {
                host_device_t *hdev = VPOP(host_devices.devices);
                MM_FREE(hdev);
        }
        VFREE(host_devices.devices);

        return tap_devices;
}

/* Deallocate the list returned by enumerate_devices(). */
static void
free_tap_device_list(tap_device_list_t device_list)
{
        for (int i = 0; i < VLEN(device_list.devices); i++)
                dealloc_tap_device(VGET(device_list.devices, i));

        VFREE(device_list.devices);
}

/* Return a list of names of devices recognized as supported TAP devices */
attr_value_t
tap_win32_get_network_devices()
{
        tap_device_list_t dev_list = enumerate_tap_devices(NULL);
        attr_value_t attr_device_list = SIM_alloc_attr_list(
                VLEN(dev_list.devices));

        VFORI(dev_list.devices, i) {
                tap_device_t *dev = VGET(dev_list.devices, i);
                SIM_attr_list_set_item(&attr_device_list, i,
                                       SIM_make_attr_string(dev->name));
                MM_FREE(dev->name);
                MM_FREE(dev->guid);
                MM_FREE(dev);
        }

        VFREE(dev_list.devices);
        return attr_device_list;
}

/* Open a TAP device given its name and guid, as specified in the
   tap_device_t structure.  Returns 0 if successful, and non-zero on
   failure. */
static int
open_tap_device(tap_device_t *dev)
{
        int ret = -1;
        strbuf_t sb = SB_INIT;
        if (dev->is_wrtap)
                sb_fmt(&sb, "%swrtap%d", USERMODEDEVICEDIR, dev->file_index);
        else
                sb_fmt(&sb, "%s%s%s", USERMODEDEVICEDIR, dev->guid, TAPSUFFIX);

        dev->handle = CreateFile(sb_str(&sb),
                                 GENERIC_READ | GENERIC_WRITE, 0, NULL,
                                 OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

        if (dev->handle == INVALID_HANDLE_VALUE) {
                SIM_LOG_INFO(2, dev->obj, 0,
                             "Failed opening TAP device %s (%s)",
                             dev->name, sb_str(&sb));
        } else {
                SIM_LOG_INFO(2, dev->obj, 0,
                             "Opened TAP device %s", dev->name);
                ret = 0;
        }
        sb_free(&sb);
        return ret;
}

/* Open a TAP device. If preferred_name is NULL (or empty), the first TAP
   device found will be used. If preferred_name is set to a non-empty
   string, it should be set to the name of a TAP device to use. The name of
   a TAP device is the string shown beside the icon of the device in the
   "Network Connections" control panel window. Returns a pointer to a
   tap_device_t structure, which should be deallocated when closing the device
   by calling tap_win32_close(). */
tap_device_t *
tap_win32_open(conf_object_t *obj, char *preferred_name)
{
        SIM_LOG_INFO(3, obj, 0, "Listing installed TAP devices.");

        tap_device_list_t device_list = enumerate_tap_devices(obj);

        if (VLEN(device_list.devices) == 0) {
                SIM_LOG_ERROR(obj, 0, "No TAP devices installed.");
                free_tap_device_list(device_list);
                return NULL;
        }

        int dev_idx = -1;
        if (preferred_name && strlen(preferred_name) > 0) {
                for (int i = 0; i < VLEN(device_list.devices); i++) {
                        char *name = VGET(device_list.devices, i)->name;
                        if (strcmp(name, preferred_name) == 0) {
                                dev_idx = i;
                                break;
                        }
                }

                if (dev_idx == -1) {
                        SIM_LOG_ERROR(obj, 0, "No such TAP device: %s",
                                      preferred_name);
                        free_tap_device_list(device_list);
                        return NULL;
                }
        } else {
                SIM_LOG_INFO(1, obj, 0,
                             "No TAP device specified, using default");
                dev_idx = 0;
        }

        tap_device_t *dev = VGET(device_list.devices, dev_idx);

        /* remove the selected device and deallocate the rest */
        VREMOVE(device_list.devices, dev_idx);
        free_tap_device_list(device_list);

        if (open_tap_device(dev) == 0) {
                return dev;
        } else {
                char *oserr = os_describe_last_error();
                SIM_LOG_ERROR(obj, 0, "Failed to open TAP device: %s", oserr);
                MM_FREE(oserr);
                dealloc_tap_device(dev);
                return NULL;
        }

        return dev;
}

/* Close the TAP device, removes any associated notifications, and
   deallocates name/guid strings. */
void
tap_win32_close(tap_device_t *dev)
{
        MM_FREE(dev->name);
        MM_FREE(dev->guid);

        if (dev->handle != INVALID_HANDLE_VALUE) {
                SIM_notify_on_object(dev->handle, 1, NULL, NULL);
                CloseHandle(dev->handle);
                dev->handle = INVALID_HANDLE_VALUE;
        }
        MM_FREE(dev);
}

HANDLE
tap_win32_get_handle(tap_device_t *dev)
{
        if (dev == NULL)
                return INVALID_HANDLE_VALUE;
        else
                return dev->handle;
}

BOOL
tap_win32_is_opened(tap_device_t *dev)
{
        if (dev == NULL)
                return FALSE;
        else
                return dev->handle != INVALID_HANDLE_VALUE;
}

/* Enable/disable the TAP device. */
BOOL
tap_win32_set_status(tap_device_t *dev, BOOL status)
{
        unsigned long len = 0;
        return DeviceIoControl(dev->handle, TAP_IOCTL_SET_MEDIA_STATUS,
                               &status, sizeof(status),
                               &status, sizeof(status), &len, NULL);
}

#endif /* defined(_WIN32) */
