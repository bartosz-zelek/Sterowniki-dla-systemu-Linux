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

#include <linux/if_tun.h>
#include <simics/util/os.h>

static const char tap_device[] = "/dev/net/tun";

bool
tap_is_open(rn_tap_t *rt)
{
        return rt->tap_fd >= 0;
}

/* called when the capture fd indicates data to read */
static void
wait_for_packet(void *param)
{
        rn_tap_t *rt = param;
        static uint8 buffer[MAX_CAPTURE_SIZE];
        int len;

        for (;;) {

                len = read(rt->tap_fd, buffer, sizeof buffer);
                /* TODO: improve error handling */
                if (len < 0) {
                        if (errno == EINTR)
                                continue;
                        return;
                }
                generic_receive_async(&rt->pl, len, buffer);
        }
}

void
lowlevel_send_eth(rn_tap_t *rt, dbuffer_t *frame)
{
        const uint8 *buf = dbuffer_read_all(frame);

        SIM_LOG_INFO(3, rt->obj, 0,
                     "Sending Ethernet frame from simulated net to '%s'",
                     sb_str(&rt->tap_iface));

        if (write(rt->tap_fd, buf, dbuffer_len(frame)) < 0) {
                SIM_LOG_ERROR(rt->obj, 0,
                              "Error sending frame to TAP interface '%s'.",
                              sb_str(&rt->tap_iface));
        }
}

bool
get_ip_address(rn_tap_t *rt)
{
        ip_addr_t host_ip, netmask;

        if (raw_get_ip_and_netmask(rt->obj, sb_str(&rt->tap_iface),
                                   &host_ip, &netmask) < 0)
                return false;

        ip_address_to_text(rt->host_ip_str, &host_ip);
        ip_address_to_text(rt->netmask_str, &netmask);
        return true;
}

static int
check_if_tap(const char *name, void *arg)
{
        bool status = false;

        int fd = os_open(tap_device, O_RDWR);
        if (fd < 0)
                return status;

        struct ifreq ifr;
        memset(&ifr, 0, sizeof ifr);
        strncpy(ifr.ifr_name, name, IF_NAMESIZE - 1);
        ifr.ifr_flags = IFF_TAP;

        if (!ioctl(fd, TUNSETIFF, &ifr)) {
                if (arg) {
                        attr_value_t *ret = arg;
                        int last = SIM_attr_list_size(*ret);
                        SIM_attr_list_resize(ret, last + 1);
                        SIM_attr_list_set_item(ret, last,
                                               SIM_make_attr_string(name));
                }
                status = true;
        }
        close(fd);
        return status;
}

attr_value_t
get_tap_devices()
{
        attr_value_t ret = SIM_alloc_attr_list(0);
        for_all_interfaces(check_if_tap, &ret);
        return ret;
}

/* Returns false on failure after having called SIM_log_{error|info}. */
bool
connect_tap(rn_tap_t *rt)
{
        struct ifreq ifr;

        int fd = os_open(tap_device, O_RDWR);
        if (fd < 0) {
                syscall_failure(rt->obj, "open", errno);
                return false;
        }

        memset(&ifr, 0, sizeof ifr);

        if (!check_if_tap(sb_str(&rt->tap_iface), NULL)) {
                SIM_LOG_INFO(1, rt->obj, 0,
                             "Failed opening TAP interface '%s'",
                             sb_str(&rt->tap_iface));
                close(fd);
                return false;
        }
        strncpy(ifr.ifr_name, sb_str(&rt->tap_iface), IF_NAMESIZE - 1);

        /* Set TAP; we do not want any header */
        ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

        if (ioctl(fd, TUNSETIFF, &ifr) < 0) {
                syscall_failure(rt->obj, "TUNSETIFF", errno);
                close(fd);
                return false;
        }

        sb_set(&rt->tap_iface, ifr.ifr_name);
        SIM_LOG_INFO(1, rt->obj, 0,
                     "Connecting to TAP device '%s'",
                     ifr.ifr_name);

        /* make sure the capture descriptor is nonblocking and is
           closed on exec */
        (void)fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
        (void)fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC);

        SIM_notify_on_descriptor(fd, Sim_NM_Read, rt->run_in_thread,
                                 wait_for_packet, rt);

        ioctl(fd, TUNSETNOCSUM, 1);

        memset(&ifr, 0, sizeof(ifr));
        strncpy(ifr.ifr_name, sb_str(&rt->tap_iface), IFNAMSIZ - 1);

        /* Need a real socket for this ioctl */
        int s = socket(PF_INET, SOCK_DGRAM, 0);
        if (!os_socket_isvalid(s)) {
                if (errno == EAFNOSUPPORT)
                        s = socket(PF_INET6, SOCK_DGRAM, 0);
                if (!os_socket_isvalid(s)) {
                        syscall_failure(rt->obj, "socket", errno);
                        close(fd);
                        return false;
                }
        }

        if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
                syscall_failure(rt->obj, "SIOCGIFINDEX", errno);
                close(s);
                close(fd);
                return false;
        }

        close(s);

        rt->tap_fd = fd;
        return true;
}

void
disconnect_tap(rn_tap_t *rt)
{
        if (rt->tap_fd >= 0) {

                /* remove notification for descriptor */
                SIM_notify_on_descriptor(rt->tap_fd, Sim_NM_Read,
                                         rt->run_in_thread, NULL, NULL);
                close(rt->tap_fd);
                rt->tap_fd = -1;
        }

        /* no IP address configured anymore */
        rt->host_ip_str[0] = 0;
        rt->netmask_str[0] = 0;
}
