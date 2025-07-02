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

#include <iphlpapi.h>

#include <simics/util/os.h>

bool
tap_is_open(rn_tap_t *rt)
{
        return tap_win32_is_opened(rt->dev);
}

/* Start an overlapped read. The function is called
   in Threaded Context: limited Simics API usage. */
static int
win32_read_async(rn_tap_t *rn)
{
        tap_device_t *dev = rn->dev;
        DWORD result;
        HANDLE handle = tap_win32_get_handle(dev);
        OVERLAPPED *ol = &dev->overlapped.read;
        DWORD bytes_read;

        for (;;) {
                ResetEvent(ol->hEvent);

                result = ReadFile(handle,
                                  dev->buf, MAX_CAPTURE_SIZE,
                                  &bytes_read, ol);

                if (result == 0) {
                        if (GetLastError() == ERROR_IO_PENDING) {
                                return 0;
                        } else {
                                char *oserr = os_describe_last_error();
                                SIM_printf("Failed to initialize async read:"
                                           " %s.\n", oserr);
                                MM_FREE(oserr);
                                return -1;
                        }
                } else {
                        generic_receive_async(&rn->pl, bytes_read, dev->buf);
                }
        }
}

/* Callback for SIM_notify_on_object(), i.e. this function gets called when
   input is available on the TAP device handle. The function is called in
   Threaded Context: limited Simics API usage. */
static void
wait_for_packet(void *param)
{
        rn_tap_t *rn = param;
        OVERLAPPED *ol = &rn->dev->overlapped.read;

        if (!rn->read_init_done) {
                /* bootstrap read operation */
                win32_read_async(rn);
                rn->read_init_done = true;
        } else {
	        /* handle event for async I/O */
	        DWORD len;
		HANDLE handle = tap_win32_get_handle(rn->dev);
                BOOL result = GetOverlappedResult(handle, ol, &len, FALSE);

                if (result) {
                        generic_receive_async(&rn->pl, len, rn->dev->buf);
                        win32_read_async(rn);      /* restart read operation */
                } else {
                        char *oserr = os_describe_last_error();
		        SIM_printf("Error getting async I/O result: %s.\n",
                                   oserr);
                        MM_FREE(oserr);
		        Sleep(100);		   /* throttle */
		}
	}
}

void
lowlevel_send_eth(rn_tap_t *rn, dbuffer_t *frame)
{
        const uint8 *buf = dbuffer_read_all(frame);
        BOOL result;
        HANDLE handle = tap_win32_get_handle(rn->dev);
        OVERLAPPED *ol = &rn->dev->overlapped.write;
        DWORD bytes_written;
        DWORD error;

        SIM_LOG_INFO(3, rn->obj, 0,
                     "Sending Ethernet frame (%zd bytes) to TAP device",
                     dbuffer_len(frame));

        ResetEvent(ol->hEvent);

        result = WriteFile(handle, buf, dbuffer_len(frame),
                           &bytes_written, ol);

        if (result) {
                SIM_LOG_INFO(3, rn->obj, 0,
                             "Wrote %d bytes to TAP interface '%s'.",
                             (int)bytes_written, rn->dev->name);
                return;
        }

        error = GetLastError();

        switch (error) {
        case ERROR_IO_PENDING:
                /* This is the common case, since operation is asynchronous */

                SIM_LOG_INFO(3, rn->obj, 0,
                             "Write pending; waiting for async result...");

                result = GetOverlappedResult(handle, ol, &bytes_written, TRUE);

                if (result == 0) {
                        char *oserr = os_describe_last_error();
                        SIM_LOG_ERROR(rn->obj, 0,
                                      "Error sending frame to TAP"
                                      " interface '%s': %s",
                                      rn->dev->name, oserr);
                        MM_FREE(oserr);
                        break;
                }

                SIM_LOG_INFO(3, rn->obj, 0,
                             "Wrote %d bytes to TAP interface '%s'.",
                             (int)bytes_written, rn->dev->name);
                break;

        default:
                char *oserr = os_describe_last_error();
                SIM_LOG_ERROR(rn->obj, 0,
                              "Failed to write frame to TAP device: %s",
                              oserr);
                MM_FREE(oserr);
                break;
        }
}

bool
get_ip_address(rn_tap_t *rt)
{
        ip_addr_t host_ip, netmask;

        if (win32_get_interface_info(rt->obj, rt->dev->guid, 
                                     &host_ip, &netmask) < 0) {
                return false;
        }

        ip_address_to_text(rt->host_ip_str, &host_ip);
        ip_address_to_text(rt->netmask_str, &netmask);
        return true;
}

/* Returns false on failure after having logged the error */
bool
connect_tap(rn_tap_t *rt)
{
        rt->dev = tap_win32_open(rt->obj, sb_str(&rt->tap_iface));

        if (rt->dev == NULL) {
                SIM_LOG_INFO(1, rt->obj, 0,
                             "Failed to open TAP device");
                return false;
        }

        if (tap_win32_set_status(rt->dev, TRUE)) {
                SIM_LOG_INFO(2, rt->obj, 0, "Enabled TAP device");
        }

        /* Set an artificial event, causing the notifier to be called ASAP to
           bootstrap the overlapped read chain */
        OVERLAPPED *ol = &rt->dev->overlapped.read;
        SetEvent(ol->hEvent);
        SIM_notify_on_object(ol->hEvent, TRUE, wait_for_packet, rt);

        return true;
}


void
disconnect_tap(rn_tap_t *rt)
{
        if (rt->dev != NULL) {
                OVERLAPPED *ol = &rt->dev->overlapped.read;
                SIM_notify_on_object(ol->hEvent, TRUE, 0, NULL);
                tap_win32_set_status(rt->dev, FALSE);
                tap_win32_close(rt->dev);
                rt->dev = NULL;
        }

        /* no IP address configured anymore */
        rt->host_ip_str[0] = 0;
        rt->netmask_str[0] = 0;
}
