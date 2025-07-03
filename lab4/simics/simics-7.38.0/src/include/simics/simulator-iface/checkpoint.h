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

#ifndef SIMICS_SIMULATOR_IFACE_CHECKPOINT_H
#define SIMICS_SIMULATOR_IFACE_CHECKPOINT_H

#include <simics/base/types.h>
#include <simics/pywrap.h>
#include <simics/simulator/configuration.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="checkpoint_interface_t">
   The <fun>save</fun> function in this interface is called when a checkpoint
   is saved, right before the attributes of an object is read. If defined,
   it should prepare the object for checkpointing, saving any state to
   <param>path</param> that is not directly included in the attributes.
   Default behavior is image to be compressed craff and config to be compressed
   too. Errors are signalled through exceptions.

   The <fun>save_v2</fun> function is same as <fun>save</fun> but take an extra
   parameter with the type of save_flags_t to control format of files in the
   checkpoint other than defaults.

   The <param>path</param> argument may be the empty string, which indicates
   that the checkpoint bundle directory is the same as the current working
   directory while the checkpoint is being saved.

   The <fun>finish</fun> function is called after the checkpoint has been
   saved, for all objects that <fun>save</fun> was called for. If
   <arg>success</arg> is nonzero, the checkpoint was saved successfully;
   otherwise there was a failure. This permits the object to clean up temporary
   data structures and files in either case. In particular, any files written
   to <param>path</param> in the <fun>save</fun> method must be removed in
   <fun>finish</fun> if <param>success</param> is zero.

   The function <fun>has_persistent_data</fun>, if implemented, should return
   0 if the object only has volatile attributes, 1 otherwise. This overrides
   <const>Sim_Attr_Persistent</const> on individual attributes.

   <insert-until text="// ADD INTERFACE checkpoint_interface"/>
   </add>
   <add id="checkpoint_interface_exec_context">
   Global Context for all methods.
   </add> */
SIM_INTERFACE(checkpoint) {
        void (*save)(conf_object_t *obj, const char *NOTNULL path);
        void (*finish)(conf_object_t *obj, int success);
        int (*has_persistent_data)(conf_object_t *obj);
        void (*save_v2)(conf_object_t *obj, const char *NOTNULL path,
                        save_flags_t flags);
};

#define CHECKPOINT_INTERFACE "checkpoint"
// ADD INTERFACE checkpoint_interface

#if defined(__cplusplus)
}
#endif

#endif
