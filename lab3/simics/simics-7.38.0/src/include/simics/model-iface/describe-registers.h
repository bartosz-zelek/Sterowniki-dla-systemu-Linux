/*
  © 2011 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_MODEL_IFACE_DESCRIBE_REGISTERS_H
#define SIMICS_MODEL_IFACE_DESCRIBE_REGISTERS_H

#include <simics/simulator-api.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add-type id="description_type_t def"><ndx>description_type_t</ndx></add-type> */
typedef enum {
        Description_Type_Group,

        Description_Type_Int_Reg,
        Description_Type_Float_Reg,
        Description_Type_Fields_Reg,

        Description_Type_Int_Field,
        Description_Type_Float_Field,
} description_type_t;

/* <add-type id="reg_role_t def"><ndx>reg_role_t</ndx></add-type> */
typedef enum {
        Reg_Role_None, /* No special role for the register. */
        Reg_Role_Program_Counter /* The register is the program counter. */
} reg_role_t;

/* <add-type id="reg_bitorder_t def"><ndx>reg_bitorder_t</ndx></add-type> */
typedef enum {
        Reg_Bitorder_Little_Endian,
        Reg_Bitorder_Big_Endian
} reg_bitorder_t;

/* Describes a named value. */
/* <add-type id="named_value_t def"><ndx>named_value_t</ndx></add-type> */
typedef struct {
        const char *name;
        const char *description;
        const bytes_t value; /* Little endian byte order */
} named_value_t;

/* <add-type id="description_t def"><ndx>description_t</ndx></add-type> */
typedef struct {
        /* Common fields */
        description_type_t type;
        const char *name;
        const char *description;

        /* Register and field fields */
        int16 dwarf_id;            /* id used by dwarf for this register
                                      or -1 if no such id is defined. This
                                      is ABI specific, but the CPU will
                                      give the ids for the most common ABI
                                      for that architecture. */
        reg_bitorder_t bitorder;   /* Bitorder convention used in the
                                      documentation for this register or
                                      field. */
        reg_role_t role;           /* Role of this register in the ABI/HW. */
        bool memory_mapped;        /* True if the register is memory mapped. */
        uint64 offset;             /* Offset into the bank for memory mapped
                                      registers. */
        bool catchable;            /* True if Core_Control_Register_Write and
                                      Core_Control_Register_Read are triggered
                                      when this register is written or read. */
        int msb, lsb;              /* Most and least significant bit of the
                                      register or field. Always given in le
                                      bitorder. For groups msb == -1 and
                                      lsb == 0. */
        int regsize;               /* Number of bits in the register, or the
                                      register this field is a part of. */
        int reg_id;                /* For registers and fields the id to pass
                                      to the get and set methods to access the
                                      register's value. Fields have the same
                                      reg_id as the register they are a part
                                      of. Not valid for groups.*/
} description_t;

/*
  <add id="describe_registers_interface_t">

  This interface is used by the Simics debugger to get certain information from
  a processor.

  The <fun>first_child</fun> function returns the first description in the
  sequence of child descriptions of parent or NULL if parent has no
  children. Groups can have both registers and groups as children, registers
  can only have fields as children and fields cannot have any children. If
  parent is NULL, return the first description in the sequence of top-level
  descriptions.

  Use <fun>next_description</fun> to deallocate the previous description and
  return the next description in the sequence or NULL if there are no more
  descriptions in the current sequence.

  The <fun>free_description</fun> function is used to free the description
  without returning the next one in the sequence.

  The <fun>first_named_value</fun> function returns the first named value in
  the sequence of named values for parent or NULL if there are no named values
  for parent. Only fields and registers can have named values.

  Use <fun>next_named_value</fun> to deallocate the previous named value and
  return the next named value or NULL if there are no more named values in this
  sequence.

  Use <fun>free_named_value</fun> to free the named value without returning the
  next one in the sequence.

  The <fun>get</fun> and <fun>set</fun> functions are used to get and set the
  value of the register. To set the value pass in a bytes_t for the value. The
  value passed in must be long enough to contain the full value of the
  register. If the bytes_t is too long it will be truncated. To get the value
  pass in a buffer_t which is long enough to contain the register's value. The
  value is encoded in little endian byte order.

  <insert id="description_type_t def"/>
  <insert id="reg_role_t def"/>
  <insert id="reg_bitorder_t def"/>
  <insert id="named_value_t def"/>
  <insert id="description_t def"/>
  <insert-until text="// ADD INTERFACE describe_registers_interface_t"/>
  </add>
  <add id="describe_registers_interface_exec_context">
  Cell Context for all methods.
  </add>
*/

SIM_INTERFACE(describe_registers) {
        const description_t *(*first_child)(
                conf_object_t *NOTNULL obj, const description_t *parent);
        const description_t *(*next_description)(
                conf_object_t *NOTNULL obj, const description_t *prev);
        void (*free_description)(conf_object_t *NOTNULL obj,
                                 const description_t *desc);
        const named_value_t *(*first_named_value)(
                conf_object_t *NOTNULL obj, const description_t *parent);
        const named_value_t *(*next_named_value)(
                conf_object_t *NOTNULL obj, const named_value_t *prev);
        void (*free_named_value)(conf_object_t *NOTNULL obj,
                                 const named_value_t *nv);
        void (*get)(conf_object_t *NOTNULL obj, int reg_id, buffer_t dest);
        void (*set)(conf_object_t *NOTNULL obj, int reg_id, bytes_t value);
};

#define DESCRIBE_REGISTERS_INTERFACE "describe_registers"
// ADD INTERFACE describe_registers_interface_t

#if defined(__cplusplus)
}
#endif

#endif
