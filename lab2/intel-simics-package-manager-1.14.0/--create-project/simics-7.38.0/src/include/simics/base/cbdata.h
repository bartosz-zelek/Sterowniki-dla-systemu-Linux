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

#ifndef SIMICS_BASE_CBDATA_H
#define SIMICS_BASE_CBDATA_H

#include <simics/base-types.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="device api types">
   <name index="true">cbdata_t, cbdata_call_t, cbdata_register_t,
   cbdata_type_t</name>
   <insert id="cbdata_t DOC"/>
   </add> */

/* <add id="cbdata_t DOC">
   <ndx>cbdata_t</ndx>
   <ndx>cbdata_call_t</ndx>
   <ndx>cbdata_register_t</ndx>
   <ndx>cbdata_type_t</ndx>
   <name index="true">cbdata_t, cbdata_call_t, cbdata_register_t,
   cbdata_type_t</name>
   <doc>
   <doc-item name="NAME">cbdata_t, cbdata_call_t, cbdata_register_t,
   cbdata_type_t</doc-item>
   <doc-item name="SYNOPSIS">
   <insert id="cbdata_type def"/>
   <insert id="cbdata def"/>
   <insert id="cbdata_reg def"/>
   </doc-item>

   <doc-item name="DESCRIPTION">

   These data types are used by API functions and interface methods that
   provide callbacks with callback data. By using these data types instead of a
   simple <type>void *</type>, the callback data can be freed correctly when
   not needed anymore.

   The types <type>cbdata_register_t</type> and <type>cbdata_call_t</type> are
   only aliases for <type>cbdata_t</type>, used to annotate whether the object
   is passed to a registration function or a callback function. This is used by
   the automatic Python wrapping to ensure that the callback data is freed
   correctly.

   Objects of this type can be created by using either
   <fun>SIM_make_cbdata</fun> or <fun>SIM_make_simple_cbdata</fun>. The latter
   creates an untyped objects with no deallocation function, while the former
   takes a <type>cbdata_type_t</type> argument, specifying a type name and a
   deallocation function.

   The following example shows how an API function could be defined using these
   data types:

   <pre size="small">
   void for_all_ids(void (*callback)(const char *id,
                                     cbdata_call_t data),
                    cbdata_register_t data);
   </pre>

   Note how the two flavors of <type>cbdata_t</type> are used.
   <type>cbdata_register_t</type> is used to pass some data to
   <fun>for_all_ids</fun> which passes the same data unmodified to
   <fun>callback</fun>. Here is an example of how this function could be
   called; from C:

   <pre size="small">
   static void callback(const char *id, cbdata_call_t data)
   {
       const char *prefix = SIM_cbdata_data(&amp;data);
       printf("%s %s\n", prefix, id);
   }
         :
       for_all_ids(callback, SIM_make_simple_cbdata("Testing"));
   </pre>

   and from Python:

   <pre size="small">
   def callback(id, prefix):
       print("%s %s" % (prefix, id))

   for_all_ids(callback, "Testing")
   </pre>

   Note in particular that the Python code does not mention "cbdata" anywhere;
   it is all automatically handled by the Python wrapping code.

   The C version of the previous example used
   <fun>SIM_make_simple_cbdata</fun>, as the constant string <tt>"Testing"</tt>
   does not need any deallocation function. For dynamically allocated data, you
   must use <fun>SIM_make_cbdata</fun> instead:

   <pre size="small">
   static const cbdata_type_t malloced_int_type = {
       "integer",       // name
       free             // dealloc
   };

   static void callback(const char *id, cbdata_call_t data)
   {
       int *count = SIM_cbdata_data(&amp;data);
       printf("%d %s\n", *count, id);
       ++*count;
   }
         :
       int *counter = malloc(sizeof *counter);
       *counter = 1;
       for_all_ids(callback, SIM_make_cbdata(malloced_int_type, counter));
   </pre>

   In this example, <fun>for_all_ids</fun> is responsible for calling the
   deallocation function for the callback data after it has completed all calls
   to <fun>callback</fun>. It does this by calling <fun>SIM_free_cbdata</fun>,
   which in turn will call <var>malloced_int_type.dealloc</var>; i.e.,
   <fun>free</fun>.

   The same example in Python; we still do not have to call any cbdata function
   manually, but we do have to pass the counter in a one-element list since
   integers are immutable in Python:

   <pre size="small">
   def callback(id, count):
       print("%s %s" % (prefix, count[0]))
       count[0] += 1

   for_all_ids(callback, [1])
   </pre>

   While the use of <type>cbdata_t</type> over a simple <type>void *</type> in
   these examples seems redundant, they are needed if <fun>for_all_ids</fun>
   does not call <fun>callback</fun> before returning, but asynchronously at
   some later point in time. The use of <type>cbdata_t</type> also ensures that
   the data is freed correctly even when any of the involved functions is
   implemented in Python. This case often arises in conjunction with Simics
   interfaces.

   See the Callback Functions in Interfaces section in the
   <cite>Simics Model Builder User's Guide</cite> for more information on how
   to use the <type>cbdata_t</type> types.

   </doc-item>

   <doc-item name="SEE ALSO">
     <type>lang_void</type>, <fun>SIM_make_cbdata</fun>,
     <fun>SIM_make_simple_cbdata</fun>, <fun>SIM_free_cbdata</fun>,
     <fun>SIM_cbdata_data</fun>, <fun>SIM_cbdata_type</fun>
   </doc-item>

   </doc>
   </add>
*/

/* <add-type id="cbdata def"></add-type> */
typedef struct cbdata cbdata_t;
/* <add-type id="cbdata_reg def"></add-type> */
typedef cbdata_t cbdata_register_t, cbdata_call_t;

#ifndef PYWRAP
/* <add-type id="cbdata_type def"></add-type> */
typedef struct {
        const char *name;
        void (*dealloc)(void *data);
} cbdata_type_t;

#ifndef GULP_API_HELP
struct cbdata {
        const cbdata_type_t *type;
        void *data;
};
#endif  /* ! GULP_API_HELP */

/* <add-fun id="device api callbacks">
     <short>get cbdata data pointer</short>
     Return the data pointer of the callback data <param>cbd</param>.
     <doc-item name="SEE ALSO"><type>cbdata_t</type></doc-item>
   </add-fun> */
FORCE_INLINE void *
SIM_cbdata_data(const cbdata_t *cbd)
{ return cbd->data; }

/* <add-fun id="device api callbacks">
     <short>get cbdata type pointer</short>
     Return a pointer to the type information of the callback data
     <param>cbd</param>.
     <doc-item name="SEE ALSO"><type>cbdata_t</type></doc-item>
   </add-fun> */
FORCE_INLINE const cbdata_type_t *
SIM_cbdata_type(const cbdata_t *cbd)
{ return cbd->type; }

/* <add-fun id="device api callbacks">
     <short>create cbdata</short>
     Create new callback data of type <param>type</param> and value
     <param>data</param>.
     <doc-item name="SEE ALSO"><type>cbdata_t</type></doc-item>
   </add-fun> */
FORCE_INLINE cbdata_t
SIM_make_cbdata(const cbdata_type_t *type, void *data)
{ cbdata_t d = { type, data }; return d; }

/* <add-fun id="device api callbacks">
     <short>create untyped cbdata</short>
     Create new untyped callback data of value <param>data</param>. An untyped
     callback data has no <fun>dealloc</fun> function.
     <doc-item name="SEE ALSO"><type>cbdata_t</type></doc-item>
   </add-fun> */
FORCE_INLINE cbdata_t
SIM_make_simple_cbdata(void *obj)
{ return SIM_make_cbdata(NULL, obj); }

/* <add-fun id="device api callbacks">
     <short>free cbdata</short>
     Free the callback data <param>cbd</param> by calling its
     <fun>dealloc</fun> function.
     <doc-item name="SEE ALSO"><type>cbdata_t</type></doc-item>
   </add-fun> */
FORCE_INLINE void
SIM_free_cbdata(cbdata_t *cbd)
{
        const cbdata_type_t *type = SIM_cbdata_type(cbd);
        if (type && type->dealloc) type->dealloc(SIM_cbdata_data(cbd));
}

EXPORTED SIM_PYOBJECT *VT_make_python_cbdata(cbdata_t cbd);
EXPORTED cbdata_t VT_make_cbdata_from_python(SIM_PYOBJECT *pyobj);

#endif  /* ! PYWRAP */

#if defined(__cplusplus)
}
#endif

#endif
