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

#ifndef SIMICS_BASE_CONF_OBJECT_H
#define SIMICS_BASE_CONF_OBJECT_H

#include <simics/base/types.h>
#include <simics/base/sobject.h>
#include <simics/base/attr-value.h>
#include <simics/util/vect.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum {
        Sim_Attr_Required         = 0,
        Sim_Attr_Optional         = 1,

        Sim_Attr_Pseudo           = 4,

        Sim_Attr_Integer_Indexed  = 0x1000,
        Sim_Attr_String_Indexed   = 0x2000,
        Sim_Attr_List_Indexed     = 0x4000,
        
        Sim_Attr_Persistent       = 0x20000,

        /* The members below are for internal use only. */
        Sim_Attr_Flag_Mask        = 0xff,

        Sim_Init_Phase_Shift      = 8,
        Sim_Init_Phase_0          = 0 << Sim_Init_Phase_Shift,
        Sim_Init_Phase_1          = 1 << Sim_Init_Phase_Shift,
        Sim_Init_Phase_Bits       = 2,
        Sim_Init_Phase_Mask       = (1 << Sim_Init_Phase_Bits) - 1,

        Sim_Init_Phase_Pre1       =
                            (-1 & Sim_Init_Phase_Mask) << Sim_Init_Phase_Shift,
        Sim_Attr_Class            = 0x8000,

        /* To prevent attribute from being visible in documentation,
           use Sim_Attr_Internal.
        */
        Sim_Attr_Internal         = 0x10000,
        Sim_Attr_CB_Data          = 0x200000,
        Sim_Attr_Legacy           = 0x400000,
        Sim_Attr_Weak_Ref         = 0x800000,

        Sim_Attr_Read_Only        = 0x40000,
        Sim_Attr_Write_Only       = 0x80000
} attr_attr_t;

/* <add id="device api types">
   <name index="true">set_error_t</name>
   <insert id="set_error_t DOC"/>
   </add> */

/* <add id="set_error_t DOC">
   <ndx>set_error_t</ndx>
   <name index="true">set_error_t</name>
   <doc>
   <doc-item name="NAME">set_error_t</doc-item>
   <doc-item name="SYNOPSIS"><smaller><insert id="conf set_error_t"/>
   </smaller></doc-item>
   <doc-item name="DESCRIPTION">
   The <fun>SIM_set_attribute()</fun> family of functions and the set functions
   registered with the <fun>SIM_register_attribute()</fun> family of
   functions return a <type>set_error_t</type> value to report success or
   failure.

   <b>Sim_Set_Ok</b><br/>
   The attribute was successfully set.

   <b>Sim_Set_Object_Not_Found</b><br/>
   The string value does not match any object name. Deprecated, use attributes
   of object type instead of string attributes referring to object names.

   <b>Sim_Set_Interface_Not_Found</b><br/>
   The object value does not implement an interface required by the attribute.

   <b>Sim_Set_Illegal_Value</b><br/>
   The value is of a legal type for the attribute, but outside the legal range.

   <b>Sim_Set_Illegal_Type</b><br/>
   The value is of an illegal type for the attribute.

   <b>Sim_Set_Attribute_Not_Found</b><br/>
   The object has no attribute with the specified name. Should only be returned
   by <fun>SIM_set_attribute()</fun> family of functions, not by attribute set
   functions.

   <b>Sim_Set_Not_Writable</b><br/>
   The attribute is read-only.

   <b>Sim_Set_Error_Types</b><br/>
   This is the number of valid error values and should not be used as
   an error code.

   </doc-item>
   </doc>
   </add> */

/* <add-type id="conf set_error_t"></add-type> */
typedef enum {
        Sim_Set_Ok,
        Sim_Set_Object_Not_Found,
        Sim_Set_Interface_Not_Found,
        Sim_Set_Illegal_Value,
        Sim_Set_Illegal_Type,
        Sim_Set_Illegal_Index,
        Sim_Set_Attribute_Not_Found,
        Sim_Set_Not_Writable,

        Sim_Set_Error_Types     /* number of error types */
} set_error_t;

/* <add-type id="conf class_info_t">
   </add-type>
 */
typedef enum {
        Sim_Class_Kind_Vanilla = 0, /* object is saved at checkpoints */
        Sim_Class_Kind_Session = 1, /* object is saved as part of a
                                     * session only */
        Sim_Class_Kind_Pseudo = 2,  /* object is never saved */

        Sim_Class_Kind_Extension = 3, /* extension class
                                         (see SIM_extend_class) */
} class_kind_t;

/* <add id="device api types">
   <name>class_data_t</name>
   <insert id="class_data_t DOC"/>
   </add> */

/* <add id="class_data_t DOC">
   <name>class_data_t, class_kind_t</name>
   <ndx>class_data_t</ndx>

<doc>
  <doc-item name="NAME">class_data_t, class_kind_t</doc-item>
  <doc-item name="SYNOPSIS">
  <smaller>
  <insert id="conf class_data_t"/>
  </smaller></doc-item>

  <doc-item name="DESCRIPTION"> 

  The <type>class_data_t</type> type is used when a new class is registered.
  Uninitialized fields should be set to zero before the structure is passed to
  <fun>SIM_register_class</fun>.

  When a new object is created, memory for the <type>conf_object_t</type> is
  allocated by Simics unless <var>alloc_object</var> has been provided by the
  class. Classes written in C may implement <var>alloc_object</var> to have a
  single allocation and must then place <type>conf_object_t</type> first in the
  object data structure. This has the advantage of allowing casts directly from
  a <type>conf_object_t *</type> to a pointer to the user structure instead of
  using <var>SIM_object_data</var>.

  When the <type>conf_object_t</type> has been allocated, the
  <var>init_object</var> function is called. If the object instance needs
  additional storage, it may allocate its own memory and return a pointer to
  it from <var>init_object</var>. This pointer can later be obtained using
  <fun>SIM_object_data</fun>. The return value from <var>init_object</var>
  has to be non-null to signal a successful object initialization. If a null
  value is returned, no configuration object will be created and an error will
  be reported. For classes that implement their own <var>alloc_object</var>,
  there is no need to allocate additional storage in the <var>init_object</var>
  function and they can simply return the <type>conf_object_t</type> pointer
  from <var>init_object</var>.

  <var>alloc_object</var> and <var>init_object</var> both receive a
  <var>data</var> parameter; they are currently not used, and should be
  ignored.

  The optional <var>finalize_instance</var> function is called when all
  attributes have been initialized in the object, and in all other objects
  that are created at the same time.

  The <var>pre_delete_instance</var> and <var>delete_instance</var> fields can
  be set to let the objects of this class support deletion:

  <ul>
    <li>
      <var>pre_delete_instance</var> will be called in the first phase of the
      object deletion, during which objects are expected to clean-up their
      external links to other objects (breakpoints, hap
      callbacks, file or network resources, ...). They may also trigger the
      deletion of other objects. <var>pre_delete_instance</var> is only called
      for objects that have reached at least <var>finalize_instance</var>
      during initialization.
    </li>

    <li>
      <var>delete_instance</var> will be called in the second phase of the
      object deletion: objects are expected to deallocate the memory they use
      including the object data structure. They may not communicate with other
      objects as these may already have been destroyed. The return value from
      <var>delete_instance</var> is ignored for compatibility.
      <var>delete_instance</var> is always called unless
      <var>alloc_object</var> is not defined, or if it returned NULL.
    </li>
   </ul>

   The delete functions may be called by Simics before an object is fully
   configured. That is, without any call to <var>finalize_instance</var> and
   possibly before all the attribute set methods have been called. This may
   happen when the object is part of a configuration that fails to load. The
   <fun>SIM_object_is_configured</fun> function can be used to determine if
   <var>finalize_instance</var> has run or not.

   The <var>description</var> string is used to describe the class in several
   sentences. It is used in the help commands and reference manuals. The
   <var>class_desc</var> string is a short class description beginning with
   lower case, without any trailing dot, and at most 50 characters long. It is
   used in help commands and for example in the GUI.

   <note>The old class functions are legacy. New code should use
   <tt>class_info_t</tt> and <fun>SIM_create_class</fun>.</note>.

   </doc-item>

   </doc>
   </add> */
/* <add-type id="conf class_data_t">
   </add-type>
 */
typedef struct class_data {
        conf_object_t *(*alloc_object)(lang_void *data);
        lang_void *(*init_object)(conf_object_t *obj, lang_void *data);
        void (*finalize_instance)(conf_object_t *obj);

        void (*pre_delete_instance)(conf_object_t *obj);
        int (*delete_instance)(conf_object_t *obj);

        const char           *description;
        const char           *class_desc;
        class_kind_t          kind;
} class_data_t;

/* <add id="device api types">
   <name>class_info_t</name>
   <insert id="class_info_t DOC"/>
   </add> */

/* <add id="class_info_t DOC">
   <name>class_info_t, class_kind_t</name>
   <ndx>class_info_t</ndx><ndx>class_kind_t</ndx>

<doc>
  <doc-item name="NAME">class_info_t, class_kind_t</doc-item>
  <doc-item name="SYNOPSIS">
  <smaller>
  <insert id="conf class_info_t"/>
  </smaller></doc-item>

  <doc-item name="DESCRIPTION">

  The <type>class_info_t</type> type is used when a new class is registered.
  Uninitialized fields should be set to zero before the structure is passed to
  <fun>SIM_create_class</fun>.

  The <var>alloc</var> method is responsible for allocating memory for the
  object itself, i.e. the <type>conf_object_t</type> structure. If no
  <var>alloc</var> method is provided, Simics will use a default one, which
  uses <fun>MM_MALLOC</fun>. Classes written in C may implement
  <var>alloc</var> to have a single allocation and must then place
  <type>conf_object_t</type> first in the object data structure. This has the
  advantage of allowing casts directly from a <type>conf_object_t *</type> to a
  pointer to the user structure instead of using
  <var>SIM_object_data</var>. The <var>alloc</var> method can fail, e.g. if
  memory allocation fails, and signals this by returning NULL.

  After <var>alloc</var> has run on all objects being created, the
  <var>init</var> function is called, if defined. This method should do any
  class specific initialization, such as initializing internal data
  structures. The <var>init</var> method may also use <fun>SIM_get_object</fun>
  to obtain pointers to other objects, and it can use
  <fun>SIM_set_attribute_default</fun> on its descendants, but it may not call
  interfaces on other objects, or post events. If the object instance needs
  additional storage, it may allocate its own memory and return a pointer to it
  from <var>init</var>. This pointer can later be obtained using
  <fun>SIM_object_data</fun>. However, for classes that implement their own
  <var>alloc</var>, there is no need for that, since it can be done by
  co-allocating the <type>conf_object_t</type> struct in a larger data
  structure, and simply return the <type>conf_object_t</type> pointer from
  <var>init</var>. The <var>init</var> method is allowed to fail, and it
  signals this by returning NULL.

  The <var>finalize</var> method, if defined, is called when all attributes
  have been initialized in the object, and in all other objects that are
  created at the same time. This method is supposed to do any
  object initialization that require attribute values. Communication with other
  objects, e.g. via interfaces, should ideally be deferred until the
  <var>objects_finalized</var> method, but is permitted if
  <fun>SIM_require_object</fun> is first called.

  The <var>objects_finalized</var> method, if defined, is called after
  <var>finalize</var> has been called on all objects, so in this method the
  configuration is ready, and communication with other objects is permitted
  without restrictions.

  The <var>deinit</var> and <var>dealloc</var> methods are called during object
  destruction. The <var>deinit</var> method, if defined, is called first on all
  objects being deleted, and is supposed to do the inverse of the
  <var>init</var> method. The <var>dealloc</var> method is supposed to free the
  <type>conf_object_t</type> itself, i.e. it should be the inverse of
  <var>alloc</var>. It is not defined, a default dealloc method is used, which
  uses <fun>MM_FREE</fun>.

  The delete functions may be called by Simics before an object is fully
  configured. That is, without any call to <var>finalize</var> and possibly
  before all the attribute set methods have been called. This may happen when
  the object is part of a configuration that fails to load. The
  <fun>SIM_object_is_configured</fun> function can be used to determine if
  <var>finalize</var> has run or not.

  All functions are called in hierarchical order, starting from the root, so
  each object can assume that in each case, a function has already been called
  on all its ancestors. This can be used to e.g. set attribute default values
  on descendants in the <var>init</var> method.

  If the initialization fails, i.e. if <var>init</var> fails, or if any
  attribute setter fails, then the configuration creation is rolled back. For
  those objects where init succeeded (or no init was defined), the
  <var>deinit</var> function will be called, and on all created objects
  (i.e. not ones where <var>alloc</var> failed) the <var>dealloc</var> method
  is called.

  The <var>description</var> string is used to describe the class in several
  sentences. It is used in the help commands and reference manuals. The
  <var>short_desc</var> string is a short class description beginning with
  lower case, without any trailing dot, and at most 50 characters long. It is
  used in help commands and for example in the GUI.

 </doc-item>

   </doc>
   </add> */
/* <add-type id="conf class_info_t">
   </add-type>
 */
typedef struct class_info {
        conf_object_t *(*alloc)(conf_class_t *cls);
        lang_void *(*init)(conf_object_t *obj);
        void (*finalize)(conf_object_t *obj);
        void (*objects_finalized)(conf_object_t *obj);

        void (*deinit)(conf_object_t *obj);
        void (*dealloc)(conf_object_t *obj);

        const char           *description;
        const char           *short_desc;
        class_kind_t          kind;
} class_info_t;

#if !defined PYWRAP && !defined GULP_API_HELP
/* <add id="device api types">
   <name index="true">conf_object_t</name>
   <insert id="conf_object_t DOC"/>
   </add> */

/* <add id="conf_object_t DOC">
   <ndx>conf_object_t</ndx>
   <name index="true">conf_object_t</name>
   <doc>
   <doc-item name="NAME">conf_object_t</doc-item>
   <doc-item name="DESCRIPTION">
   All objects in the Simics simulator have an associated 
   <type>conf_object_t</type> struct.
   Pointers to <type>conf_object_t</type> are used
   in the Simics simulator API to refer to a
   specific object in the current Simics session.
   <type>conf_object_t</type> is an opaque data structure whose members
   should only be accessed using the Simics API.
   </doc-item>
   </doc>
   </add> */

/* Opaque struct - do not access members directly! */
struct conf_object {
        sobject_t sobj;
        void *instance_data;
        struct log_info *log;
        struct confdata *conf;
        struct extension_data *extension_data;
        struct thread_domain *thread_domain;
        void *reserved[3];
};

#endif /* not PYWRAP and not GULP_API_HELP */

/* Register new class. Returns NULL on failure. */
EXPORTED conf_class_t *SIM_register_class(
        const char *NOTNULL name,
        const class_data_t *NOTNULL class_data);
EXPORTED void SIM_register_class_alias(const char *NOTNULL alias,
                                       const char *NOTNULL name);

EXPORTED conf_class_t *SIM_create_class(
        const char *NOTNULL name, const class_info_t *NOTNULL class_info);

EXPORTED void SIM_extend_class(conf_class_t *NOTNULL cls,
                               conf_class_t *NOTNULL ext);

EXPORTED conf_class_t *SIM_copy_class(const char *NOTNULL name,
                                      const conf_class_t *NOTNULL src_cls,
                                      const char *desc);

EXPORTED const char *SIM_get_class_name(
        const conf_class_t *NOTNULL class_data);

EXPORTED void VT_set_constructor_data(
        conf_class_t *NOTNULL cls, lang_void *data);

#ifndef PYWRAP
EXPORTED void SIM_set_class_data(
        conf_class_t *NOTNULL cls, lang_void *data);

EXPORTED lang_void *SIM_get_class_data(conf_class_t *NOTNULL cls);
#endif

EXPORTED void SIM_require_object(conf_object_t *NOTNULL obj);

/* <add-fun id="simics api configuration">
   <short>get object class</short>
   Returns the class of an object.

   <di name="EXECUTION CONTEXT">Cell Context</di>

   <doc-item name="SEE ALSO"><fun>SIM_get_class</fun></doc-item>
   </add-fun> */
FORCE_INLINE conf_class_t *
SIM_object_class(const conf_object_t *NOTNULL obj)
{ return (conf_class_t *)sobject_class(&obj->sobj); }

EXPORTED const char *SIM_object_name(const conf_object_t *NOTNULL obj);
EXPORTED const char *SIM_object_id(const conf_object_t *NOTNULL obj);

EXPORTED bool SIM_object_is_configured(const conf_object_t *NOTNULL obj);
EXPORTED void SIM_set_object_configured(conf_object_t *NOTNULL obj);

#ifndef PYWRAP
EXPORTED lang_void *SIM_object_data(conf_object_t *NOTNULL obj);
EXPORTED lang_void *SIM_extension_data(conf_object_t *NOTNULL obj,
                                           conf_class_t *cls);
#endif

EXPORTED conf_object_t *SIM_port_object_parent(conf_object_t *NOTNULL obj);
EXPORTED conf_object_t *SIM_object_parent(conf_object_t *NOTNULL obj);

EXPORTED conf_object_t *SIM_object_descendant(conf_object_t *obj,
                                              const char *NOTNULL relname);
EXPORTED SIM_PYOBJECT *CORE_object_iterator(conf_object_t *obj);
EXPORTED SIM_PYOBJECT *CORE_shallow_object_iterator(conf_object_t *obj,
                                                    bool expand_arrays);
EXPORTED SIM_PYOBJECT *CORE_object_iterator_for_class(
        conf_class_t *NOTNULL cls);
EXPORTED SIM_PYOBJECT *CORE_object_iterator_for_interface(attr_value_t ifaces);

#ifndef PYWRAP
typedef struct {
        /* Opaque fields. Do not access directly. */
        conf_object_t *node;
        unsigned depth;
        unsigned kind;
} inner_object_iter_t;

typedef struct {
        inner_object_iter_t inner;
        struct {
                conf_class_t *cls;
                unsigned idx;
        } c;
        struct {
#ifndef GULP_API_HELP
                /* Static maximum number of interfaces, to avoid the need for
                   the iterator to own memory. */
                interface_key_t keys[17];
#endif
                unsigned idx;
        } iface;
} object_iter_t;

/* Make sure the api-help command displays SIM functions */
#ifdef GULP_API_HELP
EXPORTED object_iter_t SIM_object_iterator(conf_object_t *obj);
EXPORTED object_iter_t SIM_shallow_object_iterator(conf_object_t *obj);
#else
EXPORTED object_iter_t VT_object_iterator(conf_object_t *obj);
EXPORTED object_iter_t VT_shallow_object_iterator(conf_object_t *obj);
#endif

EXPORTED conf_object_t *SIM_object_iterator_next(object_iter_t *iter);

/* Make sure the old versions of the SIM functions are exported
   in libsimics-common */
#ifndef GULP_EXTERN_DECL
#define SIM_object_iterator VT_object_iterator
#define SIM_shallow_object_iterator VT_shallow_object_iterator
#else
EXPORTED inner_object_iter_t SIM_object_iterator(conf_object_t *obj);
EXPORTED inner_object_iter_t SIM_shallow_object_iterator(conf_object_t *obj);
#endif

EXPORTED object_iter_t SIM_object_iterator_for_class(conf_class_t *NOTNULL cls);
EXPORTED object_iter_t SIM_object_iterator_for_interface(attr_value_t ifaces);
#endif

/* <add-type id="conf get_attr_t">
   </add-type>
 */
typedef attr_value_t (*get_attr_t)(lang_void *ptr,
                                   conf_object_t *obj,
                                   attr_value_t *idx);

/* <add-type id="conf get_class_attr_t">
   </add-type>
 */
typedef attr_value_t (*get_class_attr_t)(lang_void *ptr,
                                         conf_class_t *c,
                                         attr_value_t *idx);

/* <add-type id="conf set_attr_t">  </add-type> */
typedef set_error_t (*set_attr_t)(lang_void *ptr,
                                  conf_object_t *obj,
                                  attr_value_t *val,
                                  attr_value_t *idx);

/* <add-type id="conf set_class_attr_t">
   </add-type>
 */
typedef set_error_t (*set_class_attr_t)(lang_void *ptr,
                                        conf_class_t *c,
                                        attr_value_t *val,
                                        attr_value_t *idx);

/* Register a typed attribute (with an optionally typed index). */
EXPORTED int SIM_register_typed_attribute(
        conf_class_t *NOTNULL cls, const char *NOTNULL name,
        get_attr_t get_attr, lang_void *user_data_get,
        set_attr_t set_attr, lang_void *user_data_set,
        attr_attr_t attr, const char *type, const char *idx_type,
        const char *desc);

EXPORTED int SIM_register_typed_class_attribute(
        conf_class_t *NOTNULL cls, const char *NOTNULL name,
        get_class_attr_t get_attr, lang_void *user_data_get,
        set_class_attr_t set_attr, lang_void *user_data_set,
        attr_attr_t attr, const char *type, const char *idx_type,
        const char *desc);

EXPORTED void SIM_register_attribute(
        conf_class_t *NOTNULL cls, const char *NOTNULL name,
        attr_value_t (*get_attr)(conf_object_t *),
        set_error_t (*set_attr)(conf_object_t *, attr_value_t *),
        attr_attr_t attr, const char *NOTNULL type, const char *desc);

EXPORTED void SIM_register_class_attribute(
        conf_class_t *NOTNULL cls, const char *NOTNULL name,
        attr_value_t (*get_attr)(conf_class_t *),
        set_error_t (*set_attr)(conf_class_t *, attr_value_t *),
        attr_attr_t attr, const char *NOTNULL type, const char *desc);

EXPORTED void SIM_register_attribute_with_user_data(
        conf_class_t *NOTNULL cls, const char *NOTNULL name,
        attr_value_t (*get_attr)(conf_object_t *, lang_void *),
        lang_void *user_data_get,
        set_error_t (*set_attr)(conf_object_t *, attr_value_t *, lang_void *),
        lang_void *user_data_set,
        attr_attr_t attr, const char *NOTNULL type, const char *desc);

EXPORTED void SIM_register_class_attribute_with_user_data(
        conf_class_t *NOTNULL cls, const char *NOTNULL name,
        attr_value_t (*get_attr)(conf_class_t *, lang_void *),
        lang_void *user_data_get,
        set_error_t (*set_attr)(conf_class_t *, attr_value_t *, lang_void *),
        lang_void *user_data_set,
        attr_attr_t attr, const char *NOTNULL type, const char *desc);

EXPORTED void SIM_ensure_partial_attr_order(
        conf_class_t *NOTNULL cls,
        const char *NOTNULL attr1, const char *NOTNULL attr2);

EXPORTED void SIM_attribute_error(const char *NOTNULL msg);
#ifndef PYWRAP
EXPORTED void SIM_c_attribute_error(const char *NOTNULL msg, ...)
        PRINTF_FORMAT(1, 2);

EXPORTED void VT_set_cpu_offset(const void *arg, bool add);
#endif

typedef void interface_t;
typedef void class_interface_t;

/* Return error code on failure (0 == ok). */

EXPORTED int SIM_register_interface(conf_class_t *NOTNULL cls,
                                    const char *NOTNULL name,
                                    const interface_t *NOTNULL iface);

EXPORTED int SIM_register_port_interface(conf_class_t *NOTNULL cls,
                                         const char *NOTNULL name,
                                         const interface_t *NOTNULL iface,
                                         const char *NOTNULL portname,
                                         const char *desc);

EXPORTED void SIM_register_compatible_interfaces(conf_class_t *NOTNULL cls,
                                                 const char *NOTNULL name);

EXPORTED void SIM_register_port(conf_class_t *NOTNULL cls,
                                const char *NOTNULL name,
                                conf_class_t *NOTNULL port_cls,
                                const char *desc);

EXPORTED conf_class_t *SIM_register_simple_port(conf_class_t *NOTNULL cls,
                                                const char *NOTNULL name,
                                                const char *desc);

/* Get interface from object (returns NULL if interface not
   implemented by object). */
EXPORTED const interface_t *SIM_get_interface(
        const conf_object_t *NOTNULL obj,
        const char *NOTNULL name);
EXPORTED const interface_t *SIM_c_get_interface(
        const conf_object_t *NOTNULL obj,
        const char *NOTNULL name);
EXPORTED const class_interface_t *SIM_get_class_interface(
        const conf_class_t *NOTNULL cls,
        const char *NOTNULL name);
EXPORTED const class_interface_t *SIM_c_get_class_interface(
        const conf_class_t *NOTNULL cls,
        const char *NOTNULL name);

EXPORTED const interface_t *SIM_get_port_interface(
        const conf_object_t *NOTNULL obj,
        const char *NOTNULL name,
        const char *portname);
EXPORTED const interface_t *SIM_c_get_port_interface(
        const conf_object_t *NOTNULL obj,
        const char *NOTNULL name,
        const char *portname);
EXPORTED const class_interface_t *SIM_get_class_port_interface(
        const conf_class_t *NOTNULL cls,
        const char *NOTNULL name,
        const char *portname);
EXPORTED const class_interface_t *SIM_c_get_class_port_interface(
        const conf_class_t *NOTNULL cls,
        const char *NOTNULL name,
        const char *portname);

typedef VECT(const interface_t *) interface_array_t;

EXPORTED int VT_register_port_array_interface(
        conf_class_t *NOTNULL cls,
        const char *NOTNULL name,
        const interface_array_t *NOTNULL iface_list,
        const char *NOTNULL portname,
        const char *desc);

EXPORTED bool SIM_marked_for_deletion(const conf_object_t *NOTNULL obj);

/* type-safe replacement of SIM_register_interface */
#define SIM_REGISTER_INTERFACE(cls, name, iface)        \
do {                                                    \
        const name ## _interface_t *_if = (iface);      \
        SIM_register_interface(cls, #name, _if);        \
} while (0)

/* type-safe replacement of SIM_register_port_interface */
#define SIM_REGISTER_PORT_INTERFACE(cls, name, iface, portname, desc)   \
        do {                                                            \
                const name ## _interface_t *_if = (iface);              \
                        SIM_register_port_interface(                    \
                                cls, #name, _if, portname, desc);       \
        } while (0)

/* type-safe replacement of SIM_c_get_interface */
#define SIM_C_GET_INTERFACE(obj, name)                        \
   ((const name ## _interface_t *)SIM_c_get_interface(obj, #name))

/* type-safe replacement of SIM_c_get_port_interface */
#define SIM_C_GET_PORT_INTERFACE(obj, name, port)                \
        ((const name ## _interface_t *)SIM_c_get_port_interface( \
                obj, #name, port))

/* type-safe replacement of SIM_c_get_class_interface */
#define SIM_C_GET_CLASS_INTERFACE(cls, name)                            \
   ((const name ## _interface_t *)SIM_c_get_class_interface(cls, #name))

#if defined(__cplusplus)
}
#endif

#endif
