/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef MAGIC_PIPE_READER_INTERFACE_H
#define MAGIC_PIPE_READER_INTERFACE_H

#include <simics/base/conf-object.h>
#include <simics/pywrap.h>

#ifdef __cplusplus
extern "C" {
#endif

/* <add id="magic_pipe_reader_interface_t">

  An interface for reading received data from a magic pipe application running
  in the target system. This interface is called by a simics extension running
  in the host system communicating with a target application via a magic
  pipe. The communication is identified by a magic number. These numbers are
  acquired or reserved in the magic_pipe_setup_interface.

  The magic pipe library on the target system allocates a page-locked pipe
  buffer for the target application, which uses the buffer to communicate data
  to and from the host system. That buffer is fragmented when read by the host
  system and therefore copied into a new unfragmented buffer, which is used for
  all read accesses in this interface. All C-code readewr callback functions
  are allowed direct access to this memory area, while Python reader callback
  functions require another copy for ownership reasons.

  This interface does not modify the pipe buffer data in any way, and the
  callers are not allowed to do that neither. It is therefore safe for several
  readers to subscribe to the same data.

    <note>This interface is an experimental feature. It is excluded from
  the standard support program, and is subject to change or removal
  without notice.</note>

    <insert-until text="// ADD INTERFACE magic_pipe_reader_interface"/>
  </add>
  <add id="magic_pipe_reader_interface_exec_context">
    Cell Context for all methods.
  </add> */
SIM_INTERFACE(magic_pipe_reader) {
        /* Query whether the byte-order of the simulated target system differs
           from that of the simulator host system. */
        bool (*is_byte_swap_needed)(conf_object_t *obj, uintptr_t buf);

        /* Query the amount of used pipe buffer space. This value is always
           less than the allocated buffer size because it does not count the
           internal pipe buffer header. */
        size_t (*read_buffer_size)(conf_object_t *obj, uintptr_t buf);

#ifndef PYWRAP
        /* Get direct read-only access to the incoming pipe buffer data, at the
           desired offset. The function returns a pointer to, and the remaining
           used size from, the specified offset.

           This function gives a direct pointer into internal memory and
           therefore cannot be used by Python code. */
        bytes_t (*read_data_direct)(conf_object_t *obj, uintptr_t buf,
                                    size_t offs);
#endif

        /* Get a copy the pipe buffer data, at the specified offset with the
           specified length. If the length argument is zero, then the length of
           the remaining space from the offset is assumed.

           This function will allocate a new data buffer to hold the desired
           amount of data and return it to the caller, who is responsible to
           deallocating it once it has served its purpose. */
        bytes_t (*read_data_copy)(conf_object_t *obj, uintptr_t buf,
                                  size_t offs, size_t len);
};

#define MAGIC_PIPE_READER_INTERFACE "magic_pipe_reader"
// ADD INTERFACE magic_pipe_reader_interface

#ifdef __cplusplus
}
#endif

#endif /* MAGIC_PIPE_READER_INTERFACE_H */
