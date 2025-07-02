/*
  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_MODEL_IFACE_MMC_H
#define SIMICS_MODEL_IFACE_MMC_H

#include <simics/device-api.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="mmc_interface_t">
   <insert-until text="// ADD INTERFACE mmc_interface"/>

   Interface that should be implemented by all MMC/SD/SDHC/SDIO card models.

   send_command: sends a 5-byte command to the card (1-byte command index and
                 4 bytes command arguments).
                 Caller provides the response length. Card fills in actual
                 response data. The response data is 0, 6 or 17 bytes,
                 in big-endian (see the MMC/SD specification for details).
                 Return value: number of response bytes, -1 if the command
                 wasn't accepted (e.g. command is not supported or illegal in
                 current state, or command is not supported or illegal for
                 current card type).

   read_data: reads data. Caller provides the length.
              Return value: the card fills in the provided buffer, and returns
              the number of bytes actually read, which might be less than the
              buffer length in case of error.

   write_data: writes data. Caller provides in both length and data.
               Return value: number of bytes actually written, which might be
               less than the provided data length in case of error.

   </add>
   <add id="mmc_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
#define MMC_INTERFACE "mmc"
SIM_INTERFACE(mmc) {
#if !defined(PYWRAP)
        int (*send_command)(conf_object_t *obj, uint8 cmd, uint32 args,
                            buffer_t response);
        int (*read_data)(conf_object_t *obj, buffer_t data);
#endif
        int (*write_data)(conf_object_t *obj, bytes_t data);
};
// ADD INTERFACE mmc_interface

#if defined(__cplusplus)
}
#endif

#endif /* SIMICS_MODEL_IFACE_MMC_H */
