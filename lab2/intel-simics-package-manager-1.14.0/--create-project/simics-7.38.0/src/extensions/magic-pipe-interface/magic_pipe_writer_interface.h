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

#ifndef MAGIC_PIPE_WRITER_INTERFACE_H
#define MAGIC_PIPE_WRITER_INTERFACE_H

#include <simics/base/conf-object.h>
#include <simics/pywrap.h>

#ifdef __cplusplus
extern "C" {
#endif

/* <add id="magic_pipe_writer_interface_t">

  An interface for writing transmit data to a magic pipe application running in
  the target system. This interface is called by a simics extension running in
  the host system which communicates with a target application through a magic
  pipe. The communication is identified by a magic number. These numbers are
  acquired or reserved in the magic_pipe_setup_interface.

  The write functions operate on an unfragmented host system buffer. All C-code
  writer callback functions are allowed direct access to the unfragmented host
  buffer, while Python callback functions require the data to be copied from a
  Python owned data buffer.

  The target system pipe buffer is not modified until all writer callbacks for
  its magic number have returned. Then the pipe buffer header is updated and
  the whole unfragmented host buffer is copied into the target pipe buffer
  fragments.

    <note>This interface is an experimental feature. It is excluded from
  the standard support program, and is subject to change or removal
  without notice.</note>

    <insert-until text="// ADD INTERFACE magic_pipe_writer_interface"/>
  </add>
  <add id="magic_pipe_writer_interface_exec_context">
    Cell Context for all methods.
  </add> */
SIM_INTERFACE(magic_pipe_writer) {
        /* Query whether the simulated target system has a different byte-order
           than the simulator host system. */
        bool (*is_byte_swap_needed)(conf_object_t *obj, uintptr_t buf);

        /* Query how much unused space is available in the pipe buffer.  This
           value is decreased by each call to either write_data_add or
           write_data_copy. */
        size_t (*write_buffer_left)(conf_object_t *obj, uintptr_t buf);

        /* Query the allocated pipe buffer size. This value includes both the
           pipe buffer header and payload data. */
        size_t (*write_buffer_size)(conf_object_t *obj, uintptr_t buf);

#ifndef PYWRAP
        /* Get direct write access to the outgoing pipe buffer data. This call
           returns a pointer to the write position in the pipe buffer and its
           remaining unused size.

           If the caller writes to the pipe buffer, then the write_data_add
           function must be called also to update the amount of used data in
           the pipe buffer and advance the write position.

           The write position is also advanced by calls to the write_data_copy
           function. In case neither write_data_add nor write_data_copy
           function is called, this function will return the exact same pointer
           address and size all the time.

           This function gives a direct pointer into internal memory and
           therefore cannot be used by Python code. */
        buffer_t (*write_data_direct)(conf_object_t *obj, uintptr_t buf);

        /* Increase the amount of used data in the pipe buffer. When a caller
           to the write_data_direct function has written to the pipe buffer,
           the caller must also call this function to declare the amount of
           data written to the fragment. This will cause the write position to
           be moved to the next available space in the pipe buffer.

           If the length argument exceeds the available unused space. It is
           assumed that all the remaining space is used. */
        void (*write_data_add)(conf_object_t *obj, uintptr_t buf, size_t len);
#endif

        /* Append the data from the caller buffer to the outgoing pipe
           buffer. This function will copy as much of the data contents from
           the supplied buffer argument to the end of the pipe buffer as fits.

           The function will return the amount of data from the caller buffer
           that does not fit in the pipe buffer. If the return value is zero,
           then all data was copied. Otherwise the copied data was truncated
           and the remaining uncopied size is returned.

           Be sure to use the write_buffer_left function to determine the
           amount of remaining space, unless truncated data is desired and
           properly handled.

           This call will automatically advance the write position to the next
           unused space. */
        size_t (*write_data_copy)(conf_object_t *obj, uintptr_t buf,
                                  bytes_t data);

        /* Change the magic number in the pipe buffer. This is used to assign a
           new magic number to the target magic pipe application. Typically
           this is done only on the first exchange with a new target
           application, to give it a unique identifier, which is then
           subscribed to by the simics extension, and used throughout all
           further communication.

           The magic number to pick is typically returned by the
           register_new_pipe function in the setup interface. However, the
           simics extension may choose to reserve a range of magic numbers and
           provide its own scheme for assigning these to new target
           applications. */
        void (*write_buffer_magic)(conf_object_t *obj, uintptr_t buf,
                                   uint64 magic);
};

#define MAGIC_PIPE_WRITER_INTERFACE "magic_pipe_writer"
// ADD INTERFACE magic_pipe_writer_interface

#ifdef __cplusplus
}
#endif

#endif /* MAGIC_PIPE_WRITER_INTERFACE_H */
