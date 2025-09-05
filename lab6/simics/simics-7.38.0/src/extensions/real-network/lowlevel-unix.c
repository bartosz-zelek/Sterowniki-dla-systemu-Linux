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

#include "lowlevel.h"
#include <simics/util/os.h>

/* logs an error in SIM_log_error */
void
syscall_failure(conf_object_t *obj, const char *msg, int err)
{
        SIM_LOG_ERROR(obj, 0, "%s: %s", msg, strerror(err));
}

void
for_all_interfaces(int (*callback)(const char *name, void *arg), void *arg)
{
        struct if_nameindex *if_array, *iff;
        struct ifreq ifrflags;
        int fd;

        fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (!os_socket_isvalid(fd)) {
                if (errno == EAFNOSUPPORT)
                        fd = socket(AF_INET6, SOCK_DGRAM, 0);
                if (!os_socket_isvalid(fd))
                        return;
        }

        if_array = if_nameindex();
        if (!if_array) {
                close(fd);
                return;
        }

        for (iff = if_array; iff->if_index != 0 && iff->if_name; iff++) {
                // if_name is pointer to null terminated string, any size
                // ifr_name is pointer to fixed size char buffer
                size_t namelen = strlen(iff->if_name) + 1;  // incl NULL
                ASSERT(namelen <= sizeof(ifrflags.ifr_name));
                memcpy(ifrflags.ifr_name, iff->if_name, namelen);
                if (ioctl(fd, SIOCGIFFLAGS, &ifrflags) < 0) {
                        if (errno == ENXIO)
                                continue;
                }

                /* skip not up, or if loopback */
                if (!(ifrflags.ifr_flags & IFF_UP) ||
                    ifrflags.ifr_flags & IFF_LOOPBACK)
                        continue;

                (void)callback(iff->if_name, arg);
        }

        if_freenameindex(if_array);
        close(fd);
}

int
raw_get_ip_and_netmask(conf_object_t *obj, const char *iface,
                       ip_addr_t *ip, ip_addr_t *net)
{
        struct ifreq ifr;
        int err = -1;

        int fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (!os_socket_isvalid(fd))
                goto error;

        memset(&ifr, 0, sizeof ifr);
        strncpy(ifr.ifr_name, iface, IF_NAMESIZE - 1);

        if (ioctl(fd, SIOCGIFADDR, &ifr) < 0)
                goto error;

        struct sockaddr_in sin;
        memcpy(&sin, &ifr.ifr_addr, sizeof sin);
        ip_addr_t tmp_ip = ip_address_from_in_addr(sin.sin_addr);

        if (ioctl(fd, SIOCGIFNETMASK, &ifr) < 0)
                goto error;
        memcpy(&sin, &ifr.ifr_addr, sizeof sin);

        /* only modify ip,net on success */
        ip_address_assign(ip, &tmp_ip);
        *net = ip_address_from_in_addr(sin.sin_addr);

        err = 0;
 error:
        if (fd >= 0)
                close(fd);
        return err;
}
