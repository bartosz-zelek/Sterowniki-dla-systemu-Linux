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

#ifndef SIMICS_INTERNAL_PYTHON_H
#define SIMICS_INTERNAL_PYTHON_H

#include <simics/base/attr-value.h>
#include <simics/base/memory.h>
#include <simics/base/sim-exception.h>
#include <simics/util/hashtab.h>
#include <simics/base/cbdata.h>
#include <simics/arch/arm.h>
#include <simics/arch/mips.h>
#include <simics/arch/ppc.h>
#include <simics/arch/x86-memop.h>
#include <simics/devs/pci.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* thread support */

/* These two macros are meant to be used to allow other threads to run when the
   Python lock is taken: first release, then retake the lock */
#define SAVE_AND_RELEASE_PYTHON_LOCK()                                  \
        PyThreadState *__current_state =                                \
                (PyThreadState *)VT_save_and_release_python_lock()

#define OBTAIN_AND_RESTORE_PYTHON_LOCK()                        \
        VT_obtain_and_restore_python_lock(__current_state)

#define CALL_WITHOUT_PYTHON_LOCK(thing) do {    \
        SAVE_AND_RELEASE_PYTHON_LOCK();         \
        thing;                                  \
        OBTAIN_AND_RESTORE_PYTHON_LOCK();       \
} while (0)

/* These two macros are meant to be used to take, then release the global
   Python lock */
#define OBTAIN_PYTHON_LOCK() \
        bool __prev_state = VT_obtain_python_lock()

#define RELEASE_PYTHON_LOCK() \
        VT_release_python_lock(__prev_state)

#ifndef PYWRAP
EXPORTED void VT_push_exc_ppg_entry(void *key);
EXPORTED bool VT_pop_exc_ppg_entry(void *key, const char *function);
EXPORTED void VT_report_uncaught_exceptions(void);


EXPORTED bool VT_obtain_python_lock(void);
EXPORTED void VT_release_python_lock(bool old_lock_state);

EXPORTED void *VT_save_and_release_python_lock(void);
EXPORTED void VT_obtain_and_restore_python_lock(void *saved);

/* these constants must be kept in sync with
   scripts/build/analyze-trampolines.py */
#define TRAMPOLINE_FUNCTION_PLACEHOLDER         \
        ((uintptr_t)(sizeof (uintptr_t) == 4    \
                     ? 0xdeadbeef               \
                     : 0xdeadbeefc001babe))
#define TRAMPOLINE_DATA_PLACEHOLDER             \
        ((uintptr_t)(sizeof (uintptr_t) == 4    \
                     ? 0xc5c5c5c5               \
                     : 0xc5c5c5c55c5c5c5c))

typedef struct trampoline_data {
        void (*templ)(void);    /* trampoline template */
        size_t size;          /* size of the trampoline template */
        struct {
#if defined HOST_X86_64
                /* offsets of hi/lo 32 bits of the C function address */
                size_t function_hi, function_lo;
                /* offsets of hi/lo 32 bits of the data address */
                size_t data_hi, data_lo;
#else
 #error Unimplemented host type
#endif
        } offsets;
} trampoline_data_t;

EXPORTED SIM_PYOBJECT *VT_python_wrap_conf_object(conf_object_t *data);
EXPORTED conf_object_t *VT_python_unwrap_conf_object(SIM_PYOBJECT *pyobj);

EXPORTED bool VT_get_py_opaque_conf_object(void **dst, SIM_PYOBJECT *src);

EXPORTED SIM_PYOBJECT *VT_python_wrap_conf_class(conf_class_t *data);
EXPORTED bool VT_get_conf_class(SIM_PYOBJECT *from, conf_class_t **to);

EXPORTED bool VT_get_py_opaque_transaction(void **dst, SIM_PYOBJECT *src);
EXPORTED SIM_PYOBJECT *VT_python_wrap_transaction(const transaction_t *src);

EXPORTED bool VT_get_py_opaque_generic_transaction(void **dst, SIM_PYOBJECT *src);
EXPORTED SIM_PYOBJECT *VT_python_wrap_generic_transaction(
        const generic_transaction_t *src);

EXPORTED bool VT_get_py_opaque_x86_transaction_upcast(
        void **dst, SIM_PYOBJECT *src);
EXPORTED SIM_PYOBJECT *VT_python_wrap_x86_transaction_upcast(
        const x86_memory_transaction_t *src);

EXPORTED bool VT_get_py_opaque_ppc_transaction_upcast(
        void **dst, SIM_PYOBJECT *src);
EXPORTED SIM_PYOBJECT *VT_python_wrap_ppc_transaction_upcast(
        const ppc_memory_transaction_t *src);

EXPORTED bool VT_get_py_opaque_pci_transaction_upcast(
        void **dst, SIM_PYOBJECT *src);
EXPORTED SIM_PYOBJECT *VT_python_wrap_pci_transaction_upcast(
        const pci_memory_transaction_t *src);

EXPORTED bool VT_get_py_opaque_mips_transaction_upcast(
        void **dst, SIM_PYOBJECT *src);
EXPORTED SIM_PYOBJECT *VT_python_wrap_mips_transaction_upcast(
        const mips_memory_transaction_t *src);

EXPORTED bool VT_get_py_opaque_arm_transaction_upcast(
        void **dst, SIM_PYOBJECT *src);
EXPORTED SIM_PYOBJECT *VT_python_wrap_arm_transaction_upcast(
        const arm_memory_transaction_t *src);

EXPORTED bool VT_get_exception_type_t(SIM_PYOBJECT *from, exception_type_t *to);

EXPORTED void VT_handle_python_exception(SIM_PYOBJECT *return_source,
                                         const char *signature);
EXPORTED void VT_raise_python_exception(
        sim_exception_t exc_type, const char *str);

EXPORTED SIM_PYOBJECT *VT_attr_value_to_python_obj(
        attr_value_t *value, const char **err_msg);
EXPORTED int VT_python_obj_to_attr_value_with_error(
        SIM_PYOBJECT *obj, attr_value_t *ret);
EXPORTED void VT_byte_string_to_pyobject(
        byte_string_t bstr, SIM_PYOBJECT **po);
EXPORTED int VT_pyobject_to_byte_string(SIM_PYOBJECT *po, byte_string_t *bstr);

EXPORTED bool VT_pyobject_to_buffer_t(buffer_t *b, SIM_PYOBJECT *obj);
EXPORTED SIM_PYOBJECT *VT_buffer_t_to_pyobject(buffer_t b);
EXPORTED void VT_buffer_t_pyobject_invalidate(SIM_PYOBJECT *obj);

typedef struct py_wrap_c_func py_wrap_c_func_t;

/* converter functions from/to a struct field type */
typedef struct {
        const char *name;                    /* of type */

        /* conversion from Python: return 0 on success, -1 on error */
        int (*from_py)(void *generic_dst, SIM_PYOBJECT *pysrc);

        /* conversion to Python: shouldn't fail */
        SIM_PYOBJECT *(*to_py)(void *generic_src, conf_object_t *object);

        /* optional function to free C data; must not fail */
        void (*free_fn)(void *generic_data);

        /* wrap data, used for converting function pointer types */
        py_wrap_c_func_t *fn_wrap_data;
} field_type_t;

/* struct member information */
typedef struct struct_member {
        const char *name;
        /* set this member to a Python value: 0 on success, -1 on error */
        int (*set)(void *c_struct, const struct struct_member *member,
                   SIM_PYOBJECT *pysrc);

        /* retrieve this member as a Python value; object is only used
           for interfaces */
        SIM_PYOBJECT *(*get)(void *c_struct, conf_object_t *object,
                             const struct struct_member *member);

        /* for non-bitfields, these are passed to the generic getter/setter
           methods; they are zero/null for bitfields: */
        size_t ofs;                          /* member byte offset in struct */
        const field_type_t *type;
} struct_member_t;

typedef struct opaque_type opaque_type_t;

EXPORTED const cbdata_type_t *VT_python_cbdata_type(void);
EXPORTED SIM_PYOBJECT *VT_cbdata_python_type(void);

#ifdef Py_PYTHON_H
/* struct to hold data about each type of generated trampolines (one
   per signature) */
struct py_wrap_c_func {
        /* Python definition */
        PyMethodDef meth;
        /* C function that converts arguments and calls Python */
        void (*python_caller)(void);
        /* information about generated trampolines */
        const struct trampoline_data *trampoline_data;
        /* non-zero if this is a "method" function that can bind to an
           object */
        bool is_method;

        /* Number of arguments the function takes, including the first "object"
           argument if it is a method. */
        unsigned char arity;

        /* Python wrapper, indexed by a hash of the C function and
           bound object */
        ht_int_table_t *c_to_py_table;
        /* PyDictObject with PyCObject(C wrapper), indexed by Python object */
        PyObject *py_to_c_dict;
};

/* py_opaque_type_t is used to wrap C structures whose contents cannot
 * be accessed by Python without getter/setter functions */
typedef struct {
        PyObject_HEAD
        bool malloced;           /* true if struct should be freed at
                                        destruction */
        bool free_fields;        /* true if fields should be freed at
                                        destruction */
        bool is_mutable;         /* true if data is allowed to change */
        void *data;
        conf_object_t *object;       /* used for interfaces only */
        opaque_type_t *type;
} py_opaque_type_t;

struct opaque_type {
        PyTypeObject *pytype;
        const char *name;
        PyType_Spec pyspec;
        unsigned size;
        const struct_member_t *members;
        int nmembers;
        bool is_interface;
        PyObject *(*get_dict)(void *c_struct, struct opaque_type *ot);
};

typedef struct {
        PyObject_HEAD
        void (*func)(void);          /* actual signature may vary */
        conf_object_t *object;
} py_method_t;

typedef struct {
        PyObject_HEAD
        cbdata_t cbdata;
} python_cbdata_t;

EXPORTED SIM_PYOBJECT *VT_init_python_wrappings(void);

#endif  /* ! Py_PYTHON_H */

/* Used by module loading system */
EXPORTED int VT_register_python_interface(const char *iface_name,
                                          opaque_type_t *ot);
EXPORTED opaque_type_t *VT_get_python_interface(const char *iface_name);

/* Functions needed by module Python wrappings */
EXPORTED opaque_type_t *VT_lookup_python_opaque_type(const char *name);
EXPORTED void VT_register_opaque_python_type(opaque_type_t *type,
                                             SIM_PYTYPEOBJECT *pytype);
EXPORTED SIM_PYTYPEOBJECT *VT_get_opaque_python_type(opaque_type_t *ot);

EXPORTED SIM_PYOBJECT *VT_get_py_c_wrap(
        const void *func, conf_object_t *object, py_wrap_c_func_t *data);
EXPORTED int VT_get_c_py_wrap(
        const void **dst, SIM_PYOBJECT *src, py_wrap_c_func_t *data,
        bool ignore_arity);

#endif  /* ! PYWRAP */

#if defined(__cplusplus)
}
#endif

#endif
