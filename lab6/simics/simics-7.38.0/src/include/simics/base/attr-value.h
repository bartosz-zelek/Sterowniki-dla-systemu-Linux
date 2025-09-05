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

#ifndef SIMICS_BASE_ATTR_VALUE_H
#define SIMICS_BASE_ATTR_VALUE_H

#ifndef TURBO_SIMICS

#include <stdarg.h>

#include <simics/host-info.h>
#include <simics/base/types.h>
#include <simics/base/version.h>
#include <simics/util/help-macros.h>
#include <simics/util/alloc.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="device api types">
   <name>attr_value_t</name>
   <insert id="attr_value_t DOC"/>
   </add> */

/* <add id="attr_value_t DOC">
   <name>attr_value_t</name>
   <ndx>attr_value_t</ndx>
   <doc>
   <doc-item name="NAME">attr_value_t</doc-item>
   <doc-item name="DESCRIPTION">
   The <type>attr_value_t</type> is the type used for all values in the
   configuration system. It is a tagged union.

   The following table shows the different types of values, the type of their
   payload in C, and the corresponding Python types:

   <center>
   <table border="true" long="false">
   <tr>
     <td><b>Kind</b></td> <td><b>C payload</b></td> <td><b>Python type</b></td>
   </tr>
   <tr><td>Invalid</td> <td>-</td> <td>raises exception</td></tr>
   <tr><td>String</td><td>const char *</td> <td>str or unicode</td></tr>
   <tr><td>Integer</td> <td>int64 or uint64</td> <td>int or long</td></tr>
   <tr><td>Boolean</td> <td>bool</td> <td>bool</td></tr>
   <tr><td>Floating</td><td>double</td> <td>float</td></tr>
   <tr><td>Object</td>
       <td>conf_object_t *</td> <td>simics.conf_object_t</td></tr>
   <tr><td>List</td>
       <td>array of attr_value_t</td> <td>list</td></tr>
   <tr><td>Dict</td>
       <td>array of pairs of attr_value_t</td> <td>dict</td></tr>
   <tr><td>Data</td>
       <td>array of bytes</td> <td>tuple of small integers</td></tr>
   <tr><td>Nil</td>     <td>-</td>           <td>None</td></tr>
   </table>
   </center>

   The members inside <type>attr_value_t</type> should not be accessed
   directly. Instead, use the corresponding functions for each type:

   <table>
    <tr>
      <td>Constructor</td> <td><fun>SIM_make_attr_<var>TYPE</var></fun></td>
    </tr>
    <tr><td>Destructor</td><td><fun>SIM_attr_free</fun></td></tr>
    <tr>
      <td>Type predicate</td> <td><fun>SIM_attr_is_<var>TYPE</var></fun></td>
    </tr>
    <tr><td>Access</td><td><fun>SIM_attr_<var>TYPE</var></fun></td></tr>
   </table>

   Values of type List and Dict can be modified using
   <fun>SIM_attr_<var>TYPE</var>_set_item</fun> and
   <fun>SIM_attr_<var>TYPE</var>_resize</fun>.

   None of these functions are available or needed in Python. The
   <type>attr_value_t</type> values are translated to the ordinary Python
   values as shown in the table above.

   Some values may have data in separate heap allocations. These are normally
   managed by the respective constructor and destructor methods, but careless
   copying of values may introduce aliasing errors. Use
   <fun>SIM_attr_copy</fun> to duplicate values. Again, this is of no concern
   in Python.

   </doc-item>
   <doc-item name="SEE ALSO">
     <fun>SIM_make_attr_int64</fun>, <fun>SIM_attr_is_integer</fun>,
     <fun>SIM_attr_integer</fun>, <fun>SIM_attr_free</fun>,
     <fun>SIM_attr_list_resize</fun>, <fun>SIM_attr_list_set_item</fun>,
     <fun>SIM_attr_dict_resize</fun>, <fun>SIM_attr_dict_set_item</fun>,
     <fun>SIM_attr_copy</fun>
   </doc-item>
   </doc>
   </add> */

typedef struct attr_value attr_value_t;

#if !defined PYWRAP && !defined GULP_API_HELP

typedef enum {
        Sim_Val_Invalid,
        Sim_Val_String,
        Sim_Val_Integer,
        Sim_Val_Floating,
        Sim_Val_List,
        Sim_Val_Data,
        Sim_Val_Nil,
        Sim_Val_Object,
        Sim_Val_Dict,
        Sim_Val_Boolean,

        /* Internal use only */
        Sim_Val_Py_Object,
        Sim_Val_Unresolved_Object
} attr_kind_t;

typedef struct attr_dict_pair attr_dict_pair_t;

/* Direct access to attr_value_t is forbidden */
#ifdef SIMICS_CORE
 #define SIM_ATTR_PRIVATE(x) x
#else
 #define SIM_ATTR_PRIVATE(x) private_ ## x
#endif

struct attr_value {
        attr_kind_t SIM_ATTR_PRIVATE(kind);

        /* For List, Dict and Data: the number of items.
           For Integer: 0 for unsigned, 1 for signed. */
        unsigned SIM_ATTR_PRIVATE(size);

        union {
                char *string;                /* Sim_Val_String */
                int64 integer;               /* Sim_Val_Integer */
                bool boolean;                /* Sim_Val_Boolean */
                double floating;             /* Sim_Val_Floating */

                /* Sim_Val_List */
                struct attr_value *list;      /* [size] */

                /* Sim_Val_Dict */
                struct attr_dict_pair *dict;  /* [size] */

                /* Sim_Val_Data */
                uint8 *data;                  /* [size] */

                conf_object_t *object;       /* Sim_Val_Object */

                /* A Python object, currently only pre_conf_object */
                SIM_PYOBJECT *py_object;
        
                /* Sim_Val_Unresolved_Object (internal use only) */
                char *uobject;
        } SIM_ATTR_PRIVATE(u);
};

struct attr_dict_pair {
        struct attr_value   key;
        struct attr_value   value;
};

/* attr_value_t accessors and constructors */

EXPORTED NORETURN void VT_bad_attr_type(
        const char *function, attr_kind_t wanted, attr_value_t actual);
EXPORTED void VT_report_bad_attr_type(
        const char *function, attr_kind_t wanted, attr_value_t actual);

#endif  /* not PYWRAP and not GULP_API_HELP */

#if !defined PYWRAP && !defined TURBO_ASM_HDR_GEN && !defined STANDALONE

#ifdef GULP_API_HELP
/* Hack to make the strbuf_t typedef known to gulp so that it can parse the
   inline function bodies. */
typedef struct strbuf strbuf_t;
#endif

/* <add-fun id="device api attr_value_t">
     <short>make invalid attribute</short>
     Returns an <type>attr_value_t</type> of invalid type.

     <di name="EXECUTION CONTEXT">Cell Context</di>
   </add-fun> */
FORCE_INLINE attr_value_t
SIM_make_attr_invalid(void)
{
        attr_value_t ret;
        ret.SIM_ATTR_PRIVATE(kind) = Sim_Val_Invalid;
        ret.SIM_ATTR_PRIVATE(size) = 0;
        ret.SIM_ATTR_PRIVATE(u).integer = 0;
        return ret;
}

/* <add-fun id="device api attr_value_t">
     <short>make nil attribute</short>
     Returns an <type>attr_value_t</type> of type nil.

     <di name="EXECUTION CONTEXT">Cell Context</di>
   </add-fun> */
FORCE_INLINE attr_value_t
SIM_make_attr_nil(void)
{
        attr_value_t ret;
        ret.SIM_ATTR_PRIVATE(kind) = Sim_Val_Nil;
        ret.SIM_ATTR_PRIVATE(size) = 0;
        ret.SIM_ATTR_PRIVATE(u).integer = 0;
        return ret;
}

/* <append-fun id="SIM_make_attr_int64"></append-fun> */
FORCE_INLINE attr_value_t
SIM_make_attr_uint64(uint64 i)
{
        attr_value_t ret;
        ret.SIM_ATTR_PRIVATE(kind) = Sim_Val_Integer;
        ret.SIM_ATTR_PRIVATE(size) = 0; /* unsigned */
        ret.SIM_ATTR_PRIVATE(u).integer = (int64)i;
        return ret;
}

/* <add-fun id="device api attr_value_t">
     <short>make integer attribute</short>
     Returns an <type>attr_value_t</type> of integer type with value
     <param>i</param>.

     <di name="EXECUTION CONTEXT">Cell Context</di>
   </add-fun> */
FORCE_INLINE attr_value_t
SIM_make_attr_int64(int64 i)
{
        attr_value_t ret;
        ret.SIM_ATTR_PRIVATE(kind) = Sim_Val_Integer;
        ret.SIM_ATTR_PRIVATE(size) = 1; /* signed */
        ret.SIM_ATTR_PRIVATE(u).integer = i;
        return ret;
}

/* <add-fun id="device api attr_value_t">
     <short>make boolean attribute</short>
     Returns an <type>attr_value_t</type> of boolean type.

     <di name="EXECUTION CONTEXT">Cell Context</di>
   </add-fun> */
FORCE_INLINE attr_value_t
SIM_make_attr_boolean(bool b)
{
        attr_value_t ret;
        ret.SIM_ATTR_PRIVATE(kind) = Sim_Val_Boolean;
        ret.SIM_ATTR_PRIVATE(size) = 0;
        ret.SIM_ATTR_PRIVATE(u).boolean = b;
        return ret;
}

/* <append-fun id="SIM_make_attr_string"></append-fun> */
FORCE_INLINE attr_value_t
SIM_make_attr_string_adopt(char *str)
{
        attr_value_t ret;
        ret.SIM_ATTR_PRIVATE(kind) = str ? Sim_Val_String : Sim_Val_Nil;
        ret.SIM_ATTR_PRIVATE(size) = 0;
        ret.SIM_ATTR_PRIVATE(u).string = str;
        return ret;
}

/* <add-fun id="device api attr_value_t">
     <short>make floating point attribute</short>
     Returns an <type>attr_value_t</type> of floating type with value
     <param>d</param>.

     <di name="EXECUTION CONTEXT">Cell Context</di>
   </add-fun> */
FORCE_INLINE attr_value_t 
SIM_make_attr_floating(double d)
{
        attr_value_t ret;
        ret.SIM_ATTR_PRIVATE(kind) = Sim_Val_Floating;
        ret.SIM_ATTR_PRIVATE(size) = 0;
        ret.SIM_ATTR_PRIVATE(u).floating = d;
        return ret;
}

/* <add-fun id="device api attr_value_t">
     <short>make object attribute</short>
     Returns an <type>attr_value_t</type> of object type
     with value <param>obj</param>. Returns a nil value if
     <param>obj</param> is <const>NULL</const>.
 
     <di name="EXECUTION CONTEXT">Cell Context</di>
   </add-fun> */
FORCE_INLINE attr_value_t
SIM_make_attr_object(conf_object_t *obj)
{
        attr_value_t ret;
        ret.SIM_ATTR_PRIVATE(kind) = obj ? Sim_Val_Object : Sim_Val_Nil;
        ret.SIM_ATTR_PRIVATE(size) = 0;
        ret.SIM_ATTR_PRIVATE(u).object = obj;
        return ret;
}

/* <append-fun id="SIM_make_attr_data"></append-fun> */
FORCE_INLINE attr_value_t
SIM_make_attr_data_adopt(size_t size, void *data)
{
        attr_value_t res;
        ASSERT_MSG(data != NULL || size == 0,
                   "SIM_make_attr_data_adopt: NULL data requires zero size");
        ASSERT_MSG(size <= 0xffffffffu,
                   "SIM_make_attr_data_adopt: too large size (>(2**32-1))");
        res.SIM_ATTR_PRIVATE(kind) = Sim_Val_Data;
        res.SIM_ATTR_PRIVATE(size) = (unsigned)size;
        res.SIM_ATTR_PRIVATE(u).data = (uint8 *)data;
        return res;
}

EXPORTED attr_value_t SIM_make_attr_list_vararg(unsigned length, va_list va);
EXPORTED attr_value_t SIM_make_attr_list(unsigned length, ...);
EXPORTED attr_value_t SIM_alloc_attr_list(unsigned length);
EXPORTED attr_value_t SIM_make_attr_string(const char *str);
EXPORTED attr_value_t SIM_make_attr_data(size_t size, const void *data);
EXPORTED attr_value_t SIM_alloc_attr_dict(unsigned length);
EXPORTED void SIM_attr_list_set_item(attr_value_t *NOTNULL attr,
                                     unsigned index, attr_value_t elem);
EXPORTED void SIM_attr_list_resize(attr_value_t *NOTNULL attr,
                                   unsigned newsize);
EXPORTED void SIM_attr_dict_set_item(attr_value_t *NOTNULL attr,
                                     unsigned index,
                                     attr_value_t key, attr_value_t value);
EXPORTED void SIM_attr_dict_resize(attr_value_t *NOTNULL attr,
                                   unsigned newsize);
EXPORTED attr_value_t SIM_attr_copy(attr_value_t val);

EXPORTED bool SIM_attr_scanf(attr_value_t *NOTNULL list,
                             const char *NOTNULL fmt, ...);

EXPORTED bool SIM_ascanf(attr_value_t *NOTNULL list,
                         const char *NOTNULL fmt, ...);

EXPORTED attr_value_t VT_make_attr(const char *fmt, ...);

#if defined __GNUC__
/* Safe macro replacement for SIM_make_attr_list, that ensures that all 
   arguments are of the right type and that the count (which must be a
   constant expression) is correct.

   We add a dummy argument to the end to dodge a GCC -pedantic warning
   because ... must match at least one argument (C99 §6.10.3p4). */
#define SIM_make_attr_list(...)                                 \
  SIM_MAKE_ATTR_LIST_AUX(__VA_ARGS__, {Sim_Val_Invalid})
#define SIM_MAKE_ATTR_LIST_AUX(n, ...)                                  \
  (__extension__ ({                                                     \
     attr_value_t _a[] = {__VA_ARGS__};                                 \
     struct _s { int bad_argument_count : (n) == ALEN(_a) - 1; };       \
     VT_make_attr_list_from_array(n, _a);                               \
  }))

/* Internal function: Create a list attribute initialised by the values from
   elems (shallowly copied).
   (This function should probably be moved out-of-line; it is only inline
   to maintain compatibility with older base packages.) */
FORCE_INLINE attr_value_t
VT_make_attr_list_from_array(unsigned length, const attr_value_t *elems)
{
        attr_value_t ret;
        ret.SIM_ATTR_PRIVATE(kind) = Sim_Val_List;
        ret.SIM_ATTR_PRIVATE(size) = length;
        ret.SIM_ATTR_PRIVATE(u).list = MM_MALLOC(length, attr_value_t);
        if (length)        /* Silence Coverity */
                memcpy(ret.SIM_ATTR_PRIVATE(u).list, elems,
                       sizeof(attr_value_t) * length);
        return ret;
}
#endif  /* __GNUC__ */

EXPORTED void SIM_attr_free(attr_value_t *NOTNULL value);
EXPORTED void SIM_free_attribute(attr_value_t value);

/* <append-fun id="SIM_attr_is_integer"/> */
FORCE_INLINE bool
SIM_attr_is_nil(attr_value_t attr)
{
        return attr.SIM_ATTR_PRIVATE(kind) == Sim_Val_Nil;
}

/* <append-fun id="SIM_attr_is_integer"/> */
FORCE_INLINE bool
SIM_attr_is_int64(attr_value_t attr)
{
        return attr.SIM_ATTR_PRIVATE(kind) == Sim_Val_Integer
                && (attr.SIM_ATTR_PRIVATE(size)
                    || attr.SIM_ATTR_PRIVATE(u).integer >= 0);
}

/* <append-fun id="SIM_attr_is_integer"/> */
FORCE_INLINE bool
SIM_attr_is_uint64(attr_value_t attr)
{
        return attr.SIM_ATTR_PRIVATE(kind) == Sim_Val_Integer
                && (!attr.SIM_ATTR_PRIVATE(size)
                    || attr.SIM_ATTR_PRIVATE(u).integer >= 0);
}

/* <add-fun id="device api attr_value_t">
     <short><type>attr_value_t</type> type predicates</short>

     Indicates whether the value stored in <arg>attr</arg> is of the specified
     type. <fun>SIM_attr_is_int64</fun> and <fun>SIM_attr_is_uint64</fun> 
     additionally test whether the integer value would fit in the given C type.

     <di name="EXECUTION CONTEXT">Cell Context</di>
   </add-fun> */
FORCE_INLINE bool
SIM_attr_is_integer(attr_value_t attr)
{
        return attr.SIM_ATTR_PRIVATE(kind) == Sim_Val_Integer;
}

#define BAD_ATTR_KIND(attr, desired) \
    unlikely((attr).SIM_ATTR_PRIVATE(kind) != Sim_Val_ ## desired)

#define VALIDATE_ATTR_KIND_FATAL(func, attr, desired) do {                    \
        if (BAD_ATTR_KIND(attr, desired))                       \
                VT_bad_attr_type(#func, Sim_Val_ ## desired, attr);     \
} while (0)

#define VALIDATE_ATTR_KIND(func, attr, desired, fallback) do {     \
        if (BAD_ATTR_KIND(attr, desired)) {                                 \
                VT_report_bad_attr_type(#func, Sim_Val_ ## desired, attr);  \
                return fallback;                                            \
        }                                                                   \
} while (0)

/* <add-fun id="device api attr_value_t">
     <short>extract values stored in <type>attr_value_t</type> values</short>

     Extract a value encapsulated in <param>attr</param>. It is an error to
     call an accessor function with an <param>attr</param> of the wrong type.

     <fun>SIM_attr_integer</fun> returns the integer attribute value
     modulo-reduced to the interval
     <math>[-2<sup>63</sup>,2<sup>63</sup>-1]</math>.
     (Converting the return value to <type>uint64</type> gives the integer
     attribute value modulo-reduced to <math>[0,2<sup>64</sup>-1]</math>.)

     <fun>SIM_attr_string</fun>, <fun>SIM_attr_data</fun> and
     <fun>SIM_attr_list</fun> return values owned by <param>attr</param>. 
     Ownership is not transferred to the caller.
     
     <fun>SIM_attr_string_detach</fun> returns the string
     in <param>attr</param> and changes the value pointed to by
     <param>attr</param> into a nil attribute. Ownership of the string is
     transferred to the caller.
     
     <fun>SIM_attr_object_or_nil</fun> accepts an <param>attr</param> parameter
     of either object or nil type. In case of a nil attribute, the function
     returns NULL.
     
     <fun>SIM_attr_list_size</fun> and <fun>SIM_attr_dict_size</fun> return
     the number of items in the list and key-value pairs in the dict
     respectively. <fun>SIM_attr_data_size</fun> returns the number of bytes
     in the data value.
     
     <fun>SIM_attr_list_item</fun> returns the item at <param>index</param>.
     The index must be less than the number of items in the list. The item
     returned is still owned by <param>attr</param>. Ownership is not
     transferred to the caller.
     
     <fun>SIM_attr_list</fun> returns a pointer directly into the internal
     array of the attribute value; it is mainly present as an optimisation. Use
     <fun>SIM_attr_list_item</fun> and <fun>SIM_attr_list_set_item</fun>
     for type-safety instead.
     
     <fun>SIM_attr_dict_key</fun> and <fun>SIM_attr_dict_value</fun> return
     the key and value at <param>index</param>. The index must be less than the
     number of items in the dict. The value returned is still owned by
     <param>attr</param>. Ownership is not transferred to the caller.
     
     <di name="EXECUTION CONTEXT">
     All contexts (including Threaded Context)
     </di>

   </add-fun> */
FORCE_INLINE int64
SIM_attr_integer(attr_value_t attr)
{
        VALIDATE_ATTR_KIND(SIM_attr_integer, attr, Integer, 0);
        return attr.SIM_ATTR_PRIVATE(u).integer;
}

/* <append-fun id="SIM_attr_is_integer"/> */
FORCE_INLINE bool
SIM_attr_is_boolean(attr_value_t attr)
{
        return attr.SIM_ATTR_PRIVATE(kind) == Sim_Val_Boolean;
}

/* <append-fun id="SIM_attr_integer"/> */
FORCE_INLINE bool
SIM_attr_boolean(attr_value_t attr)
{
        VALIDATE_ATTR_KIND(SIM_attr_boolean, attr, Boolean, false);
        return attr.SIM_ATTR_PRIVATE(u).boolean;
}

/* <append-fun id="SIM_attr_is_integer"/> */
FORCE_INLINE bool
SIM_attr_is_string(attr_value_t attr)
{
        return attr.SIM_ATTR_PRIVATE(kind) == Sim_Val_String;
}

/* <append-fun id="SIM_attr_integer"/> */
FORCE_INLINE const char *
SIM_attr_string(attr_value_t attr)
{
        VALIDATE_ATTR_KIND(SIM_attr_string, attr, String, "");
        return attr.SIM_ATTR_PRIVATE(u).string;
}

/* <append-fun id="SIM_attr_integer"/> */
FORCE_INLINE char *
SIM_attr_string_detach(attr_value_t *attr)
{
        char *ret;
        VALIDATE_ATTR_KIND(SIM_attr_string_detach, *attr, String,
                           (SIM_attr_free(attr),
                            *attr = SIM_make_attr_nil(),
                            MM_STRDUP("")));
        ret = (char *)attr->SIM_ATTR_PRIVATE(u).string;
        *attr = SIM_make_attr_nil();
        return ret;
}

/* <append-fun id="SIM_attr_is_integer"/> */
FORCE_INLINE bool
SIM_attr_is_floating(attr_value_t attr)
{
        return attr.SIM_ATTR_PRIVATE(kind) == Sim_Val_Floating;
}

/* <append-fun id="SIM_attr_integer"/> */
FORCE_INLINE double
SIM_attr_floating(attr_value_t attr)
{
        VALIDATE_ATTR_KIND(SIM_attr_floating, attr, Floating, 0);
        return attr.SIM_ATTR_PRIVATE(u).floating;
}

/* <append-fun id="SIM_attr_is_integer"/> */
FORCE_INLINE bool
SIM_attr_is_object(attr_value_t attr)
{
        return attr.SIM_ATTR_PRIVATE(kind) == Sim_Val_Object;
}

/* <append-fun id="SIM_attr_integer"/> */
FORCE_INLINE conf_object_t *
SIM_attr_object(attr_value_t attr)
{
        VALIDATE_ATTR_KIND_FATAL(SIM_attr_object, attr, Object);
        return attr.SIM_ATTR_PRIVATE(u).object;
}

/* <append-fun id="SIM_attr_integer"/> */
FORCE_INLINE conf_object_t *
SIM_attr_object_or_nil(attr_value_t attr)
{
        return SIM_attr_is_nil(attr) ? NULL : SIM_attr_object(attr);
}

/* <append-fun id="SIM_attr_is_integer"/> */
FORCE_INLINE bool
SIM_attr_is_invalid(attr_value_t attr)
{
        return attr.SIM_ATTR_PRIVATE(kind) == Sim_Val_Invalid;
}

/* <append-fun id="SIM_attr_is_integer"/> */
FORCE_INLINE bool
SIM_attr_is_data(attr_value_t attr)
{
        return attr.SIM_ATTR_PRIVATE(kind) == Sim_Val_Data;
}

/* <append-fun id="SIM_attr_integer"/> */
FORCE_INLINE unsigned
SIM_attr_data_size(attr_value_t attr)
{
        VALIDATE_ATTR_KIND(SIM_attr_data_size, attr, Data, 0);
        return attr.SIM_ATTR_PRIVATE(size);
}

/* <append-fun id="SIM_attr_integer"/> */
FORCE_INLINE const uint8 *
SIM_attr_data(attr_value_t attr)
{
        VALIDATE_ATTR_KIND(SIM_attr_data, attr, Data, NULL);
        return attr.SIM_ATTR_PRIVATE(u).data;
}

/* <append-fun id="SIM_attr_is_integer"/> */
FORCE_INLINE bool
SIM_attr_is_list(attr_value_t attr)
{
        return attr.SIM_ATTR_PRIVATE(kind) == Sim_Val_List;
}

/* <append-fun id="SIM_attr_integer"/> */
FORCE_INLINE unsigned
SIM_attr_list_size(attr_value_t attr)
{
        VALIDATE_ATTR_KIND(SIM_attr_list_size, attr, List, 0);
        return attr.SIM_ATTR_PRIVATE(size);
}

/* <append-fun id="SIM_attr_integer"/> */
FORCE_INLINE attr_value_t
SIM_attr_list_item(attr_value_t attr, unsigned index)
{
        VALIDATE_ATTR_KIND_FATAL(SIM_attr_list_item, attr, List);
        ASSERT_VFMT(index < SIM_attr_list_size(attr),
                    ("SIM_attr_list_item: Index %d out of range"
                     " in list of length %d",
                     index, SIM_attr_list_size(attr)));
        return attr.SIM_ATTR_PRIVATE(u).list[index];
}

/* <append-fun id="SIM_attr_integer"/> */
FORCE_INLINE attr_value_t *
SIM_attr_list(attr_value_t attr)
{
        VALIDATE_ATTR_KIND(SIM_attr_list, attr, List, NULL);
        return attr.SIM_ATTR_PRIVATE(u).list;
}

/* <append-fun id="SIM_attr_is_integer"/> */
FORCE_INLINE bool
SIM_attr_is_dict(attr_value_t attr)
{
        return attr.SIM_ATTR_PRIVATE(kind) == Sim_Val_Dict;
}

/* <append-fun id="SIM_attr_integer"/> */
FORCE_INLINE unsigned
SIM_attr_dict_size(attr_value_t attr)
{
        VALIDATE_ATTR_KIND(SIM_attr_dict_size, attr, Dict, 0);
        return attr.SIM_ATTR_PRIVATE(size);
}

/* <append-fun id="SIM_attr_integer"/> */
FORCE_INLINE attr_value_t
SIM_attr_dict_key(attr_value_t attr, unsigned index)
{
        VALIDATE_ATTR_KIND_FATAL(SIM_attr_dict_key, attr, Dict);
        ASSERT_VFMT(index < SIM_attr_dict_size(attr),
                    ("SIM_attr_dict_key: Index %d out of range"
                     " in dictionary of size %d",
                     index, SIM_attr_dict_size(attr)));
        return attr.SIM_ATTR_PRIVATE(u).dict[index].key;
}

/* <append-fun id="SIM_attr_integer"/> */
FORCE_INLINE attr_value_t
SIM_attr_dict_value(attr_value_t attr, unsigned index)
{
        VALIDATE_ATTR_KIND_FATAL(SIM_attr_dict_value, attr, Dict);
        ASSERT_VFMT(index < SIM_attr_dict_size(attr),
                    ("SIM_attr_dict_value: Index %d out of range"
                     " in dictionary of size %d",
                     index, SIM_attr_dict_size(attr)));
        return attr.SIM_ATTR_PRIVATE(u).dict[index].value;
}

#endif  /* not PYWRAP and not TURBO_ASM_HDR_GEN and not STANDALONE */

#ifndef SIMICS_CORE
 #undef SIM_ATTR_PRIVATE
#endif

EXPORTED bool VT_attr_values_equal(attr_value_t a1, attr_value_t a2);

#if defined(__cplusplus)
}
#endif

#endif  /* ! TURBO_SIMICS */

#endif
