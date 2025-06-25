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

#ifndef SIMICS_MODEL_IFACE_REGISTER_VIEW_H
#define SIMICS_MODEL_IFACE_REGISTER_VIEW_H

#include <simics/base/types.h>
#include <simics/base/attr-value.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
  This interface is an experimental feature. It is excluded from
  the standard support program, and is subject to change or removal
  without notice.

  Global Context for all methods.
*/

SIM_INTERFACE(register_view) {
        const char *(*description)(conf_object_t *NOTNULL obj);
        bool (*big_endian_bitorder)(conf_object_t *NOTNULL obj);
        unsigned (*number_of_registers)(conf_object_t *NOTNULL obj);

        /* Returns the static information about a register in the list
           [NAME, DESC, SIZE, OFFSET, BITFIELDS, BIG_ENDIAN_BYTE_ORDER].
           All but the two last
           element in the list are required and the client should
           tolerate if more than 6 elements are presented in the list.
           NAME must be a valid identifier. DESC is a short description
           of the register. SIZE is the number of bytes of the register,
           could only be 1, 2, 4, or 8. OFFSET is the offset into the bank
           and can be Nil denoting non-memory-mapped registers.
           BITFIELDS is optional and if present, is a list where each
           element itself being a list of [NAME, DESC, LSB, MSB] where
           NAME and DESC is the identifier and description of the field
           and LSB and MSB are the least significant and the most significant
           bit in little-endian bitorder. MSB is optional and if omitted,
           is assumed to be the same as LSB, i.e., the bitfield specifies
           a single bit.
           BIG_ENDIAN_BYTE_ORDER is an optional boolean value, assumed to be
           False if omitted. It controls the byte order of the register's
           memory-mapped representation; the byte at OFFSET is the
           most significant byte if BIG_ENDIAN_BYTE_ORDER is True, and
           the least significant byte otherwise.
        */
        attr_value_t (*register_info)(conf_object_t *NOTNULL obj, unsigned reg);

        /* Get the current value of a register, must be side-effect free. */
        uint64 (*get_register_value)(conf_object_t *NOTNULL obj, unsigned reg);

        /* Set the current value of a register, must be side-effect free except
           from updating the value. */
        void (*set_register_value)(conf_object_t *NOTNULL obj, unsigned reg,
                                   uint64 val);
};

#define REGISTER_VIEW_INTERFACE "register_view"

SIM_INTERFACE(register_view_catalog) {
        /* [s*], the name of each register using register_view's numbering */
        attr_value_t (*register_names)(conf_object_t *NOTNULL obj);
        /* [i*], the offset of each register using register_view's numbering */
        attr_value_t (*register_offsets)(conf_object_t *NOTNULL obj);
};

#define REGISTER_VIEW_CATALOG_INTERFACE "register_view_catalog"

#if defined(__cplusplus)
}
#endif

#endif
