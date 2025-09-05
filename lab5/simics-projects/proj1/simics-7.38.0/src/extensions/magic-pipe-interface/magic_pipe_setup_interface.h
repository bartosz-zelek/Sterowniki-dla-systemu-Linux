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

#ifndef MAGIC_PIPE_SETUP_INTERFACE_H
#define MAGIC_PIPE_SETUP_INTERFACE_H

#include <simics/base/conf-object.h>
#include <simics/pywrap.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Buffer read/write event callback.

   Arguments:
        cpu     the active cpu core of the event.
        buf     the buffer data handle.
        magic   the magic number of the buffer.

   The buffer handle is used in most calls to this interface to identify the
   buffer. */
typedef void (*pipe_func_t)(conf_object_t *cpu, uintptr_t buf, uint64 magic);

/* <add id="magic_pipe_setup_interface_t"> The magic pipe setup interface is
  used to establish connections between an application running in the simulated
  target system and a Simics extension executing on the simulator host.

  Magic numbers are used to identify and isolate connections, called
  pipes. Typically a well-known value is used to perform a handshake and then
  assign a new and unique value, which is used in all successive correspondence
  between the end-points.

  The Simics extension is responsible for assigning these new and unique magic
  numbers, and to subscribe to them. This interface provides the necessary
  facilities to either get a new random number or reserve a range for the
  service to distribute on its own.

  An example of a common communication flow can be divided into two phases,
  first the initial handshake phase where some information about each other is
  exchanged, followed by the duty phase where the target system application is
  communicating back and forth with the host system Simics extension to fulfill
  their purpose.

  The handshake is initiated from the target application, which sends a
  handshake request to the host system extension containing some information
  about itself. The host system extension receives the request and replies with
  a new magic number and some information about itself. The new magic number is
  to be used in all further communication between the parties, to isolate the
  communications pipe from other users of the magic pipe.

  In the duty phase the common communication flow may look like this. The
  target system application starts by allocating a pipe buffer from the magic
  pipe library. Then writes its data to the buffer and sends it to the host
  system extension. The extension handles the data and reuses the same buffer
  to write something back to the application. This means that the buffer size
  is fixed and limits the amount of data that can be returned. Because of this
  it is common for the application to allocate more space than needed for its
  sent data. Once the application returns from its send call, the same buffer
  it allocated earlier is filled with data coming from the extension. This data
  is handled by the application and then the buffer is freed. This duty cycle
  is then repeated as many times as needed.

  Each magic number may have more than one subscribers, therefore reading and
  writing is divided into two phases, where all readers are allowed access
  first. Then comes the writer phase and the subscribers are called in the
  order they registered. This also means that later writer subscribers are
  limited to writing only the remaining amount of data to the buffer.

  There is no on or off setting for a pipe to enable or disable the
  communication. The only option is to unregister from the magic number to
  suspend the communication and then to register again to resume.

  The registered subscribers should unregister when they are no longer
  interested in receiving any data. This will also allow the magic pipe to stop
  listening to haps when there is no one to receive them. The magic pipe will
  automatically resume listening once there are subscribers again.

    <note>This interface is an experimental feature. It is excluded from the
  standard support program, and is subject to change or removal without
  notice.</note>

    <insert-until text="// ADD INTERFACE magic_pipe_setup_interface"/>
  </add>
  <add id="magic_pipe_setup_interface_exec_context">
  Cell Context for all methods.
  </add> */
SIM_INTERFACE(magic_pipe_setup) {
        /* Register a subscriber for a new magic number, which is returned by
           this function. The number is guaranteed to be unused and unreserved.

           The reader and writer call-backs will be called in turn for each
           message with the new magic number. Unless they are NULL or None. */
        uint64 (*register_new_pipe)(
                conf_object_t *obj, conf_object_t *cpu, conf_object_t *user,
                pipe_func_t reader, pipe_func_t writer);

        /* Register a subscriber for a range of reserved magic numbers.

           The min_magic argument must be greater than zero and max_magic equal
           to or greater than that. */
        void (*register_pipe_range)(conf_object_t *obj, conf_object_t *user,
                                    uint64 min_magic, uint64 max_magic,
                                    pipe_func_t rd, pipe_func_t wr);

        /* Register a subscriber for a reserved magic number.

           The reader and writer call-backs will be called in turn for each
           message with the reserved magic number, unless NULL or None.

           The magic number zero is reserved for a catch-all handler,
           where any message that is unsubscribed will trigger the
           call-backs. */
        void (*register_reserved_pipe)(
                conf_object_t *obj, conf_object_t *user, uint64 magic,
                pipe_func_t reader, pipe_func_t writer);

        /* Unregister the subscription of a magic number for this user. This
           will unregister both the reader and writer callback functions. If
           the user registered a whole range, then any number in the range will
           do, to unsubscribe to the whole range. */
        void (*unregister_pipe)(conf_object_t *obj, conf_object_t *user,
                                uint64 magic);

        /* Get a list of the subscribers for a magic number. The list can be
           filtered to include only readers or writers or both. If neither is
           specified only reservations are listed. */
        attr_value_t (*get_pipe_subscribers)(conf_object_t *obj, uint64 magic,
                                             bool readers, bool writers);

        /* Get a list of the used or reserved magic numbers. Each entry is a
           list of 5 items: minimum magic number, maximum magic number,
           registered subscriber object, reader callback present and writer
           callback present.

           EXAMPLE:
           [[0, 0, "fault_handler", TRUE, FALSE],
            [1, 1, "handshake", TRUE, TRUE],
            [0x10, 0x20, "my_magics", FALSE, FALSE],
            [0x4711, 0x4711, "cool_user", TRUE, TRUE]]

            The exact same information is available in the map attribute of the
            magic-pipe object. Most of the same information is also printed by
            the status command. */
        attr_value_t (*get_magic_map)(conf_object_t *obj);
};

#define MAGIC_PIPE_SETUP_INTERFACE "magic_pipe_setup"
// ADD INTERFACE magic_pipe_setup_interface

#ifdef __cplusplus
}
#endif

#endif /* MAGIC_PIPE_SETUP_INTERFACE_H */
