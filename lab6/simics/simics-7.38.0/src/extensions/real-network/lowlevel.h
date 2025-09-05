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

#ifndef REAL_NETWORK_LOWLEVEL_H
#define REAL_NETWORK_LOWLEVEL_H

/* This is to get some sundry network interface definitions from Linux */
#define _GNU_SOURCE 1

#include <simics/module-host-config.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wchar.h>

#ifdef _WIN32
  #include <winsock2.h>
  #include <windows.h>
  #include <ws2tcpip.h>
  #include <wincrypt.h>
  #include <iphlpapi.h>
#endif

#include <simics/simulator-api.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#ifndef _WIN32
 #include <sys/ioctl.h>
 #include <sys/wait.h>
#endif

#include "real-network.h"

#define MAX_CAPTURE_SIZE 16384  /* should be enough even for jumboframes */

#if defined(__cplusplus)
extern "C" {
#endif

/* interface exported by raw lowlevel */
int raw_get_ip_and_netmask(conf_object_t *obj, const char *iface,
                           ip_addr_t *ip, ip_addr_t *net);

#ifndef _WIN32
void for_all_interfaces(int (*callback)(const char *name, void *arg),
                        void *arg);

attr_value_t get_tap_devices();
#endif

void syscall_failure(conf_object_t *obj, const char *msg, int err);

#ifdef _WIN32

#define NETWORK_CONNECTION_KEY \
	"SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}"
#define ADAPTER_KEY "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}"
#define MAX_KEYLEN	256
#define MAX_DATALEN	2048

int win32_get_interface_info(conf_object_t *obj, const char *guid,
                             ip_addr_t *ip, ip_addr_t *mask);

struct tap_device {
        conf_object_t *obj;                   /* for logging */
        char *guid;
        char *name;
        int version_major;
        int version_minor;
        HANDLE handle;             /* The handle if the device is opened */
        struct {
                OVERLAPPED read;
                OVERLAPPED write;
        } overlapped;
        char buf[MAX_CAPTURE_SIZE];
        bool is_wrtap;           /* Indicates a Wind River TAP device */
        int32 file_index;        /* WR TAP device name: "wrtap%d", file_index*/
};

typedef struct {
        VECT(tap_device_t *) devices;
} tap_device_list_t;

tap_device_t *tap_win32_open(conf_object_t *obj, char *name);
void tap_win32_close(tap_device_t *tap_device);
BOOL tap_win32_is_opened(tap_device_t *tap_device);
HANDLE tap_win32_get_handle(tap_device_t *tap_device);
BOOL tap_win32_set_status(tap_device_t *tap_device, BOOL status);
attr_value_t tap_win32_get_network_devices();

#endif /* defined(_WIN32) */

bool connect_tap(rn_tap_t *rt);
void disconnect_tap(rn_tap_t *rt);
bool tap_is_open(rn_tap_t *rt);
bool get_ip_address(rn_tap_t *rt);

#if defined(__cplusplus)
}
#endif

#endif /* not REAL_NETWORK_LOWLEVEL_H */
