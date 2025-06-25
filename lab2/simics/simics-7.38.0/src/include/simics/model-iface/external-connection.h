/*
  external connection interfaces

  © 2021 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_MODEL_IFACE_EXTERNAL_CONNECTION_H
#define SIMICS_MODEL_IFACE_EXTERNAL_CONNECTION_H

#include <simics/base/conf-object.h>
#include <simics/base/types.h>
#include <simics/pywrap.h>
#include <simics/simulator/callbacks.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
 * TODO: these constants should likely migrate to simics/simulator/callbacks.h.
 */
typedef enum {
        Sim_EM_Global,          /* run in global context */
        Sim_EM_Thread           /* run in standalone thread */
} exec_mode_t;

/* Internal interface implemented by external connection servers. */
SIM_INTERFACE(external_connection_ctl) {
        /* Call this with a NULL cookie to indicate non-acceptance.
           If blocking_read is true then this connection will 
           use blocking reads, i.e. calls to read will not return until the
           complete buffer has been filled. */
        void (*accept)(conf_object_t *obj, uint64 id, lang_void *cookie,
                       bool blocking_read);
        /* Return number of bytes read, -1 on error,
           or -2 if there is no data and the read would have blocked
           (only possible if non-blocking reads are used). */
        ssize_t (*read)(conf_object_t *obj, lang_void *cookie, buffer_t buffer);
        /* Return number of bytes written, or -1 on error. */
        ssize_t (*write)(conf_object_t *obj, lang_void *cookie, bytes_t bytes);
        void (*write_async)(conf_object_t *obj, lang_void *cookie,
                            bytes_t bytes);
        /* Indicate if an error was detected during read/write. */
        bool (*has_error)(conf_object_t *obj, lang_void *cookie);
        void (*notify)(conf_object_t *obj, lang_void *cookie,
                       notify_mode_t notify_mode, exec_mode_t exec_mode,
                       bool active);
        void (*close)(conf_object_t *obj, lang_void *cookie);
};
#define EXTERNAL_CONNECTION_CTL_INTERFACE "external_connection_ctl"

/* Internal interface implemented by objects communicating
   with external connection servers. */
SIM_INTERFACE(external_connection_events) {
        void (*on_accept)(conf_object_t *obj, conf_object_t *server, uint64 id);
        void (*on_input)(conf_object_t *obj, lang_void *cookie);
        void (*can_write)(conf_object_t *obj, lang_void *cookie);
};
#define EXTERNAL_CONNECTION_EVENTS_INTERFACE "external_connection_events"

/* Internal interface implemented by tcp server. */
SIM_INTERFACE(tcp_connection_info) {
        uint16 (*remote_port)(conf_object_t *NOTNULL obj, lang_void *cookie);
        bytes_t (*remote_ip)(conf_object_t *NOTNULL obj, lang_void *cookie);
};
#define TCP_CONNECTION_INFO_INTERFACE "tcp_connection_info"

#if defined(__cplusplus)
}
#endif

#endif /* ! SIMICS_MODEL_IFACE_EXTERNAL_CONNECTION_H */
