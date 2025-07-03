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

#include "ptys.h"
#include "host-serial.h"

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <pthread.h>

#include <simics/base/log.h>
#include <simics/util/os.h>
#include <simics/simulator/callbacks.h>
#include <simics/simulator/hap-consumer.h>

#include "text-console.h"
#include "driver.h"
#include "screen.h"
#include "pty-inline.h"

typedef VECT(uint8) obuf_t;

typedef struct pty {
        // Pointer to parent structure (for pty name)
        host_serial_data_t *hs;
        // PTY handle (equivalent of Linux file descriptor)
        HANDLE handle;
        // Required by Windows asynchronous I/O operations.
        OVERLAPPED overlapped;
        // Storage for character read by asynchronous I/O.
        uint8 ch;
        // Windows event object, for SIM_notify_object.
        HANDLE hevent;

        // Lock protecting fields below
        pthread_mutex_t lock;

        // Output function scheduled
        bool has_worker;

        // Buffer used for new output
        int cur_buf;
        obuf_t obuf[2];

        // Exit synchronization. The condition is raised when 'has_worker'
        // is cleared when 'exiting' is set.
        bool exiting;
        pthread_cond_t exit_cond;
} pty_t;

static conf_object_t *
pty_obj(pty_t *pty) { return to_obj(pty->hs->tc); }

static screen_t *
pty_sc(pty_t *pty) { return pty->hs->tc->screen; }

bool
host_serial_connected(text_console_t *tc)
{
        pty_t *pty = tc->host_serial_data->pty_data;
        return pty->handle != INVALID_HANDLE_VALUE;
}

// Open specified named pipe or COM port and setup host-serial data structure.
// Does not change object state on error.
static bool
open_existing_pty(pty_t *pty, const char *name)
{
        // Try open COM port
        strbuf_t buf = sb_newf("\\\\.\\%s", name);
        HANDLE handle = CreateFile(
                sb_str(&buf), GENERIC_READ | GENERIC_WRITE,
                0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
        if (handle == INVALID_HANDLE_VALUE) {
                // Try open named pipe
                sb_fmt(&buf, "\\\\.\\pipe\\%s", name);
                handle = CreateFile(
                        sb_str(&buf), GENERIC_READ | GENERIC_WRITE,
                        0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
        }

        if (handle == INVALID_HANDLE_VALUE) {
                char *oserr = os_describe_last_error();
                SIM_LOG_ERROR(pty_obj(pty), 0,
                              "Failed opening pipe or COM port %s: \"%s\"."
                              " Make sure it exists and is unused.",
                              sb_str(&buf), oserr);
                MM_FREE(oserr);
                sb_free(&buf);
                return false;
        }

        sb_free(&buf);
        pty->handle = handle;
        pty->hs->pty_name = MM_STRDUP(name);
        return true;
}

// Does a synchronous write operation.
static ssize_t
pty_write(pty_t *pty, const uint8 *buf, ssize_t len)
{
        ASSERT(pty->handle != INVALID_HANDLE_VALUE);
        // PTY opened with FILE_FLAG_OVERLAPPED, hence requires
        // an OVERLAPPED structure.
        OVERLAPPED overlapped;
        overlapped.Offset = 0;
        overlapped.OffsetHigh = 0;
        overlapped.hEvent = 0;

        DWORD num_bytes;
        if (WriteFile(pty->handle, buf, len,
                      &num_bytes, &overlapped) == FALSE) {
                // Wait for write operation to complete.
                GetOverlappedResult(pty->handle, &overlapped,
                                    &num_bytes, TRUE);
        }
        return num_bytes;
}

static void
write_in_thread(void *data)
{
        pty_t *pty = data;
        pthread_mutex_lock(&pty->lock);
        for (;;) {
                obuf_t *b = &pty->obuf[pty->cur_buf];
                if (VEMPTY(*b)) {
                        pty->has_worker = false;
                        if (pty->exiting)
                                pthread_cond_signal(&pty->exit_cond);
                        break;
                }
                // Switch active buffer to make sure new data is not added
                // to the buffer being serviced.
                pty->cur_buf = !pty->cur_buf;

                pthread_mutex_unlock(&pty->lock);
                ssize_t n = VLEN(*b);
                uint8 *dst = VVEC(*b);
                while (n > 0) {
                        ssize_t written = pty_write(pty, dst, n);
                        if (written == -1) {
                                if (!pty->exiting) {
                                        SIM_LOG_ERROR(pty_obj(pty),
                                                      Text_Console_Log_Pty,
                                                      "Could not write to pty");
                                }
                                break;
                        }
                        dst += written;
                        n -= written;
                }
                VCLEAR(*b);
                pthread_mutex_lock(&pty->lock);
        }
        pthread_mutex_unlock(&pty->lock);
}

// Windows version of host-serial console output.
void
host_serial_write(host_serial_data_t *hs, uint8 value)
{
        pty_t *pty = hs->pty_data;

        if (pty->handle != INVALID_HANDLE_VALUE) {
                SIM_LOG_INFO(4, pty_obj(pty), Text_Console_Log_Pty,
                             "sending 0x%x to pty", (int)value);

                pthread_mutex_lock(&pty->lock);
                bool worker_active = pty->has_worker;
                pty->has_worker = true;
                obuf_t *b = &pty->obuf[pty->cur_buf];
                VADD(*b, value);
                pthread_mutex_unlock(&pty->lock);

                if (!worker_active)
                        SIM_run_in_thread(write_in_thread, pty);
        }
}

static void
read_pty(pty_t *pty)
{
        // Start another asynchronous read
        pty->overlapped.Offset = 0;
        pty->overlapped.OffsetHigh = 0;
        pty->overlapped.hEvent = pty->hevent;
        ReadFile(pty->handle, &pty->ch, 1, NULL, &pty->overlapped);
}

// SIM_notify_object callback for PTY, called when console input
// is available.
static void
pty_on_read(void *arg)
{
        pty_t *pty = arg;
        DWORD num_bytes;

        // Check read operation status, without wait.
        GetOverlappedResult(pty->handle, &pty->overlapped, &num_bytes, FALSE);

        if (num_bytes) {
                ASSERT(num_bytes == 1);
                console_input(pty->hs->tc->driver_data, pty->ch);
        }

        // Start next read
        read_pty(pty);
}

static void
send_current_screen(pty_t *pty)
{
        strbuf_t buf = SB_INIT;
        get_screen_string(pty_sc(pty), &buf);

        pthread_mutex_lock(&pty->lock);
        bool worker_active = pty->has_worker;
        pty->has_worker = true;
        obuf_t *b = &pty->obuf[pty->cur_buf];
        size_t size = VLEN(*b);
        VGROW(*b, sb_len(&buf));
        memcpy(VVEC(*b) + size, sb_str(&buf), sb_len(&buf));
        pthread_mutex_unlock(&pty->lock);

        if (!worker_active)
                SIM_run_in_thread(write_in_thread, pty);

        sb_free(&buf);
}

static void
pty_shutdown(pty_t *pty)
{
        if (pty->handle != INVALID_HANDLE_VALUE) {
                // Cancel asynchronous read operation
                if (!CancelIo(pty->handle)) {
                        char *oserr = os_describe_last_error();
                        SIM_LOG_ERROR(pty_obj(pty), Text_Console_Log_Pty,
                                      "Failed to cancel I/O operation: %s\n",
                                      oserr);
                        MM_FREE(oserr);
                }
                
                pthread_mutex_lock(&pty->lock);
                pty->exiting = true;
                bool wait = pty->has_worker;
                pthread_mutex_unlock(&pty->lock);

                SIM_notify_on_object(pty->hevent, 0, 0, NULL);
                
                if (!CloseHandle(pty->hevent)) {
                        char *oserr = os_describe_last_error();
                        SIM_LOG_ERROR(pty_obj(pty), Text_Console_Log_Pty,
                                      "Failed to close I/O handle: %s\n",
                                      oserr);
                        MM_FREE(oserr);
                }
                if (!CloseHandle(pty->handle)) {
                        char *oserr = os_describe_last_error();
                        SIM_LOG_ERROR(pty_obj(pty), Text_Console_Log_Pty,
                                      "Failed to close File handle: %s\n",
                                      oserr);
                        MM_FREE(oserr);
                }

                if (wait) {
                        pthread_mutex_lock(&pty->lock);
                        while (pty->has_worker)
                                pthread_cond_wait(&pty->exit_cond, &pty->lock);
                        pty->exiting = false;
                        pthread_mutex_unlock(&pty->lock);
                }

                MM_FREE(pty->hs->pty_name);
                pty->hs->pty_name = NULL;
                pty->handle = INVALID_HANDLE_VALUE;
        }
}

// HAP callback for cleaning up system resources, in case we exit Simics before
// deleting console object.
static void
hap_core_at_exit_cb(void *arg, conf_object_t *_obj)
{
        pty_t *pty = arg;
        pty_shutdown(pty);
}

// Windows implementation for host_serial interface.
bool
host_serial_setup(conf_object_t *obj, const char *name)
{
        pty_t *pty = pty_data(obj);
        if (name != NULL) {
                if (!open_existing_pty(pty, name)) {
                        SIM_LOG_INFO(1, obj, Text_Console_Log_Pty,
                                     "Failed to open %s", name);
                        return false;
                }

                pty->hevent = CreateEvent(NULL, TRUE, FALSE, NULL);
                SIM_notify_on_object(pty->hevent, 0, pty_on_read, pty);

                // Start read operation
                read_pty(pty);

                // Make sure PTY is cleaned up
                SIM_hap_add_callback("Core_At_Exit",
                                     (obj_hap_func_t)hap_core_at_exit_cb, pty);

                SIM_LOG_INFO(1, obj, Text_Console_Log_Pty,
                             "Opened %s", name);
                send_current_screen(pty);
                return true;
        } else {
                SIM_LOG_ERROR(obj, Text_Console_Log_Pty,
                              "Must specify PTY");
                return false;
        }
}

// Windows implementation for host_serial interface.
void
host_serial_shutdown(conf_object_t *obj)
{
        pty_t *pty = pty_data(obj);
        pty_shutdown(pty);
}

pty_t *
init_pty(host_serial_data_t *hs)
{
        pty_t *pty = MM_ZALLOC(1, pty_t);
        pty->handle = INVALID_HANDLE_VALUE;
        pty->hs = hs;
        pthread_mutex_init(&pty->lock, NULL);
        pthread_cond_init(&pty->exit_cond, NULL);
        return pty;
}

void
destroy_pty(pty_t *pty)
{
        // Do not run atexit after object is deleted.
        SIM_hap_delete_callback("Core_At_Exit",
                                (obj_hap_func_t)hap_core_at_exit_cb, pty);
        pty_shutdown(pty);
        pthread_cond_destroy(&pty->exit_cond);
        pthread_mutex_destroy(&pty->lock);
        MM_FREE(pty);
}
