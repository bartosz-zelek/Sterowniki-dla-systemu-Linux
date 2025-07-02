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

#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED
#define _DEFAULT_SOURCE

#include <stdio.h>

/* To compile on older systems */
#ifndef __USE_MISC
#define __USE_MISC
#endif

#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>

/* To obtain cfmakeraw */
#ifdef __linux
#define __USE_BSD
#endif

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pty.h>

#include <simics/base/log.h>
#include <simics/util/os.h>
#include <simics/simulator/callbacks.h>

#include "ptys.h"
#include "host-serial.h"
#include "text-console.h"
#include "driver.h"
#include "screen.h"
#include "pty-inline.h"
#include <pthread.h>

typedef VECT(uint8) obuf_t;

typedef struct pty {
        // Pointer to parent structure (for PTY name.)
        host_serial_data_t *hs;
        // Descriptor used for communication, or -1.
        int fd;
        int slave;

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
        return pty->fd != -1;
}


/// Open and setup a new PTY. Assigns file descriptor and PTY name in
// host-serial data structure. Does not change object state on error.
static bool
open_new_pty(pty_t *pty)
{
        int fd;
        int slave;
        char ptyname[512];

        struct termios new;
        cfmakeraw(&new);

        int err = openpty(&fd, &slave, ptyname, &new, NULL);
        if (err) {
                SIM_LOG_ERROR(pty_obj(pty), 0,
                              "No free ptys found: %s", strerror(errno));
                return false;
        }
        pty->hs->pty_name = MM_STRDUP(ptyname);
        pty->fd = fd;
        pty->slave = slave;

        return true;
}

// Open and setup the specified PTY. Assigns file descriptor and PTY name in
// host-serial data structure. Does not change object state on error.
static bool
open_existing_pty(pty_t *pty, const char *name)
{
        int fd = os_open(name, O_RDWR | O_NDELAY);
        if (fd < 0) {
                SIM_LOG_ERROR(pty_obj(pty), Text_Console_Log_Pty,
                              "Failed opening %s: \"%s\". Make sure this"
                              " device exists and is unused.",
                              name, strerror(errno));
                return false;
        }

        struct termios new;
        if (tcgetattr(fd, &new) < 0) {
                char *oserr = os_describe_last_error();
                SIM_LOG_ERROR(pty_obj(pty), Text_Console_Log_Pty,
                              "tcgetattr failed: %s", oserr);
                MM_FREE(oserr);
        }

        cfmakeraw(&new);
        
        if (tcsetattr(fd, TCSANOW, &new) < 0) {
                char *oserr = os_describe_last_error();
                SIM_LOG_ERROR(pty_obj(pty), Text_Console_Log_Pty,
                              "tcsetattr failed: %s", oserr);
                MM_FREE(oserr);
                close(fd);
                return false;
        }
        
        pty->hs->pty_name = MM_STRDUP(name);
        pty->fd = fd;
        return true;
}

// SIM_notify_on_descriptor callback for PTY, called when console input
// is available.
static void
pty_on_read(void *arg)
{
        pty_t *pty = arg;

        // Read input and send to the console.
        uint8 ch;        
        if (read(pty->fd, &ch, 1) != 1)
                return;        
        console_input(pty->hs->tc->driver_data, ch);
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
                        ssize_t written = write(pty->fd, dst, n);
                        if (written == -1) {
                                pthread_mutex_lock(&pty->lock);
                                bool exiting = pty->exiting;
                                pthread_mutex_unlock(&pty->lock);
                                if (!exiting) {
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


// Linux PTY version of host-serial console output.
void
host_serial_write(host_serial_data_t *hs, uint8 value)
{
        pty_t *pty = hs->pty_data;

        if (pty->fd >= 0) {
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
        if (pty->fd >= 0) {
                pthread_mutex_lock(&pty->lock);
                pty->exiting = true;
                bool wait = pty->has_worker;
                pthread_mutex_unlock(&pty->lock);

                SIM_notify_on_descriptor(pty->fd, Sim_NM_Read, 0, 0, NULL);

                // The following handles the case when the pty is not
                // connected to anything and the internal buffer has
                // been filled.
                (void)fcntl(pty->fd, F_SETFL,
                            fcntl(pty->fd, F_GETFL) | O_NONBLOCK);
                if (pty->slave != -1)
                        close(pty->slave);

                if (wait) {
                        pthread_mutex_lock(&pty->lock);
                        while (pty->has_worker)
                                pthread_cond_wait(&pty->exit_cond, &pty->lock);
                        pty->exiting = false;
                        pthread_mutex_unlock(&pty->lock);
                }
                close(pty->fd);
                pty->fd = -1;
                pty->slave = -1;
                MM_FREE(pty->hs->pty_name);
                pty->hs->pty_name = NULL;
        }
}

// Linux PTY implementation for host_serial interface.
bool
host_serial_setup(conf_object_t *obj, const char *name)
{
        pty_t *pty = pty_data(obj);
        pty_shutdown(pty);

        if (name != NULL && !open_existing_pty(pty, name)) {
                SIM_LOG_ERROR(obj, Text_Console_Log_Pty,
                              "Failed to open %s", name);
                return false;
        } else if (name == NULL && !open_new_pty(pty)) {
                SIM_LOG_ERROR(obj, Text_Console_Log_Pty,
                              "Failed to open new pty");
                return false;
        } else {
                SIM_LOG_INFO(1, obj, Text_Console_Log_Pty, 
                             "Device opened: %s", pty->hs->pty_name);
                SIM_notify_on_descriptor(pty->fd, Sim_NM_Read, 0,
                                         pty_on_read, pty);
                send_current_screen(pty);
                return true;
        }
}

// Linux PTY implementation for host_serial interface.
void
host_serial_shutdown(conf_object_t *obj)
{
        pty_shutdown(pty_data(obj));
}

pty_t *
init_pty(host_serial_data_t *hs)
{
        pty_t *pty = MM_ZALLOC(1, pty_t);
        pty->fd = -1;
        pty->hs = hs;
        pthread_mutex_init(&pty->lock, NULL);
        pthread_cond_init(&pty->exit_cond, NULL);
        return pty;
}

void
destroy_pty(pty_t *pty)
{
        pty_shutdown(pty);
        pthread_cond_destroy(&pty->exit_cond);
        pthread_mutex_destroy(&pty->lock);
        MM_FREE(pty);
}
