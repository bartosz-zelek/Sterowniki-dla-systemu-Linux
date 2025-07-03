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

#ifndef SIMICS_UTIL_FRAGS_H
#define SIMICS_UTIL_FRAGS_H

#include <simics/base-types.h>

#include <simics/util/alloc.h>
#include <simics/util/help-macros.h>
#include <simics/util/swabber.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="frags_api">
   <name index="true">frags_t, frags_it_t</name>
   <insert id="frags_t DOC"/>
</add> */

/* <add id="frags_t DOC">
     <ndx>frags_t</ndx>
     <ndx>frags_it_t</ndx>
     <name index="true">frags_t, frags_it_t</name>
     <doc>
       <doc-item name="NAME">frags_t, frags_it_t</doc-item>
       <doc-item name="SYNOPSIS">
         <insert id="frags_t def"/>
         <insert id="frags_it_t def"/>
       </doc-item>
       <doc-item name="DESCRIPTION">
         These types encapsulate a data packet, for use by
         models that send and receive data, such as network devices.

         The structures should never be used directly. Only use the accessor
         functions.
       </doc-item>
     </doc>
   </add>
*/

/* <add-type id="frags_t def"></add-type> */
typedef struct frags frags_t;

#ifndef PYWRAP

#define MAX_FRAGS_FRAGS 8

/* <add-type id="frags_it_t def"></add-type> */
typedef struct frags_it frags_it_t;

#ifndef GULP_API_HELP

typedef struct frags_frag {
        const uint8 *start;
        size_t len;
} frags_frag_t;

struct frags {
        size_t len;
        unsigned nfrags;
        frags_frag_t fraglist[MAX_FRAGS_FRAGS];
};

struct frags_it {
        const struct frags_frag *frag;
        size_t skip;
        size_t left;
};

#endif /* not GULP_API_HELP */

// An initializer that can be used in variable declarations
#define FRAGS_INIT { .len = 0, .nfrags = 0, .fraglist = {} }

/* <add-fun id="frags_api">
     <short>initialize a <type>frags_t</type></short>

     Initialize the <type>frags_t</type> <param>buf</param>. An alternative is
     to use the <const>FRAGS_INIT</const> constant value.

     <di name="RETURN VALUE">None</di>
     <di name="EXECUTION CONTEXT">Cell Context</di>
     <di name="EXAMPLE">
       <insert id="fg_init1"/>
       <insert id="fg_init0"/>
     </di>
     <di name="SEE ALSO"><fun>frags_init_add</fun></di>

   </add-fun> */
FORCE_INLINE void frags_init(frags_t *buf) { buf->len = 0; buf->nfrags = 0; }

/* <add-fun id="frags_api">
     <short>initialize a <type>frags_t</type> with an initial value</short>

     Initialize the <type>frags_t</type> <param>buf</param> and set it to
     represent the initial data <param>data</param> of size
     <param>len</param>. 

     This function is exactly equivalent to using <fun>frags_init()</fun>,
     followed by <fun>frags_add()</fun>, and is provided for convenience.

     <di name="RETURN VALUE">None</di>
     <di name="EXECUTION CONTEXT">Cell Context</di>
     <di name="EXAMPLE">
       <insert id="fg_init2"/>
     </di>
     <di name="SEE ALSO">
       <fun>frags_init</fun>, <fun>frags_add</fun>
     </di>

   </add-fun> */
FORCE_INLINE void
frags_init_add(frags_t *buf, const void *data, size_t len)
{
        buf->len = len;
        buf->nfrags = 1;
        {
                struct frags_frag frag = { (const uint8 *)data, len };
                buf->fraglist[0] = frag;
        }
}

/* <add-fun id="frags_api">
     <short>return the total data length</short>

     Return the total length of the data represented by <param>buf</param>.

     <di name="RETURN VALUE">The total data length</di>
     <di name="EXECUTION CONTEXT">Cell Context</di>
     <di name="EXAMPLE">
       <insert id="fg_len"/>
     </di>

   </add-fun> */
FORCE_INLINE size_t frags_len(const frags_t *buf) { return buf->len; }

/* <add-fun id="frags_api">
     <short>return an iterator</short>

     Return an iterator on the fragments that compose the data in
     <param>buf</param>, starting from offset <param>offset</param> and up to a
     length of <param>len</param>. To iterate on all the data in
     <param>buf</param>, <param>offset</param> should be set to 0 and
     <param>len</param> to the value returned by <fun>frags_len()</fun>.

     <di name="RETURN VALUE">An iterator on the fragments covering the desired
     data range</di>
     <di name="EXECUTION CONTEXT">Cell Context</di>
     <di name="EXAMPLE">
       <insert id="fg_it"/>
     </di>
     <di name="SEE ALSO">
       <fun>frags_it_end</fun>, <fun>frags_it_next</fun>,
       <fun>frags_it_len</fun>, <fun>frags_it_data</fun>
     </di>

   </add-fun> */
FORCE_INLINE frags_it_t
frags_it(const frags_t *buf, size_t offset, size_t len)
{
        const struct frags_frag *frag;
        size_t frag_offset;
        ASSERT(offset <= buf->len);
        frag = buf->fraglist;
        frag_offset = 0;
        while (frag_offset < offset
               && frag_offset + frag->len <= offset) {
                frag_offset += frag->len;
                frag++;
        }
        //ASSERT(frag_offset <= offset);
        //ASSERT(frag <= buf->frags + buf->nfrags);

        {
                frags_it_t ret = {
                        frag,
                        offset - frag_offset,
                        len
                };
                return ret;
        }
}

/* <add-fun id="frags_api">
     <short>return whether an iterator is finished</short>

     Return <const>true</const> when the iterator <param>it</param> does not
     have any next fragment to return at the next call of
     <fun>frags_it_next()</fun>, and <const>false</const> otherwise.

     <di name="RETURN VALUE"><const>true</const> if the iterator is finished,
     <const>false</const> otherwise.</di>
     <di name="EXECUTION CONTEXT">Cell Context</di>
     <di name="EXAMPLE">
       <insert id="fg_it"/>
     </di>
     <di name="SEE ALSO">
       <fun>frags_it</fun>, <fun>frags_it_next</fun>,
       <fun>frags_it_len</fun>, <fun>frags_it_data</fun>
     </di>

   </add-fun> */
FORCE_INLINE bool 
frags_it_end(frags_it_t it) { return it.left == 0; }

/* <add-fun id="frags_api">
     <short>return the data of the current fragment</short>

     Return a pointer to the data of the current fragment pointed by the
     iterator <param>it</param>.

     <di name="RETURN VALUE">The data of the current fragment</di>
     <di name="EXECUTION CONTEXT">Cell Context</di>
     <di name="EXAMPLE">
       <insert id="fg_it"/>
     </di>
     <di name="SEE ALSO">
       <fun>frags_it</fun>, <fun>frags_it_end</fun>,
       <fun>frags_it_next</fun>, <fun>frags_it_len</fun>
     </di>

   </add-fun> */
FORCE_INLINE const uint8 *
frags_it_data(frags_it_t it) { return it.frag->start + it.skip; }

/* <add-fun id="frags_api">
     <short>return the length of the current fragment</short>

     Return the length of the current fragment pointed by the iterator
     <param>it</param>.

     <di name="RETURN VALUE">The length of the current fragment</di>
     <di name="EXECUTION CONTEXT">Cell Context</di>
     <di name="EXAMPLE">
       <insert id="fg_it"/>
     </di>
     <di name="SEE ALSO">
       <fun>frags_it</fun>, <fun>frags_it_end</fun>,
       <fun>frags_it_next</fun>, <fun>frags_it_data</fun>
     </di>

   </add-fun> */
FORCE_INLINE size_t
frags_it_len(frags_it_t it) { return MIN(it.frag->len - it.skip, it.left); }

/* <add-fun id="frags_api">
     <short>return the next fragment's iterator</short>

     Return an iterator pointing at the next data fragment. This function
     should only be called if <fun>frags_end(it)</fun> returns
     <const>false</const>.

     <di name="RETURN VALUE">An iterator on the next fragment</di>
     <di name="EXECUTION CONTEXT">Cell Context</di>
     <di name="EXAMPLE">
       <insert id="fg_it"/>
     </di>
     <di name="SEE ALSO">
       <fun>frags_it</fun>, <fun>frags_it_end</fun>,
       <fun>frags_it_len</fun>, <fun>frags_it_data</fun>
     </di>

   </add-fun> */
FORCE_INLINE frags_it_t
frags_it_next(frags_it_t it)
{
        ASSERT(it.left > 0);
        {
                frags_it_t ret = {
                        it.frag + 1,
                        0,
                        it.left - frags_it_len(it)
                };
                return ret;
        }
}

/* <add-fun id="frags_api">
     <short>add data to a <type>frags_t</type></short>

     Append the new data <param>data</param> of size <param>len</param> to
     <param>buf</param>.
     A <type>frags_t</type> can hold up to 8 data fragments.

     <di name="RETURN VALUE">None</di>
     <di name="EXECUTION CONTEXT">Cell Context</di>
     <di name="EXAMPLE">
       <insert id="fg_add"/>
     </di>
     <di name="SEE ALSO">
       <fun>frags_init_add</fun>, <fun>frags_init_add_from_frags</fun>, 
       <fun>frags_add_from_frags</fun>
     </di>

   </add-fun> */
FORCE_INLINE void
frags_add(frags_t *buf, const void *data, size_t len)
{
        ASSERT(buf->nfrags < MAX_FRAGS_FRAGS);
        {
                struct frags_frag frag = { (const uint8 *)data, len };
                buf->fraglist[buf->nfrags] = frag;
        }
        buf->nfrags++;
        buf->len += len;
}

/* <add-fun id="frags_api">
     <short>append an existing <type>frags_t</type> to another</short>

     Append <param>len</param> bytes of the data at offset
     <param>offset</param> in <param>src</param> to <param>dst</param>.

     <di name="RETURN VALUE">None</di>
     <di name="EXECUTION CONTEXT">Cell Context</di>
     <di name="EXAMPLE">
       <insert id="fg_add_fg"/>
     </di>
     <di name="SEE ALSO">
       <fun>frags_init_add</fun>, <fun>frags_add</fun>,
       <fun>frags_init_add_from_frags</fun>
     </di>

   </add-fun> */
FORCE_INLINE void
frags_add_from_frags(frags_t *dst, const frags_t *src,
                     size_t offset, size_t len)
{
        frags_it_t src_it;
        ASSERT(offset <= src->len);
        ASSERT(len <= (src->len - offset));

        for (src_it = frags_it(src, offset, len);
             !frags_it_end(src_it);
             src_it = frags_it_next(src_it))
                frags_add(dst, frags_it_data(src_it), frags_it_len(src_it));
}

/* <add-fun id="frags_api">
     <short>initialize a <type>frags_t</type> from another</short>

     Initialize <param>dst</param> and set its initial value to the data of
     size <param>len</param> starting at offset <param>offset</param> in
     <param>src</param>.

     This function is exactly equivalent to using <fun>frags_init()</fun>,
     followed by <fun>frags_add_from_frags()</fun>, and is provided for
     convenience.

     <di name="RETURN VALUE">None</di>
     <di name="EXECUTION CONTEXT">Cell Context</di>
     <di name="EXAMPLE">
       <insert id="fg_init_add_fg"/>
     </di>
     <di name="SEE ALSO">
       <fun>frags_init_add</fun>, <fun>frags_add</fun>,
       <fun>frags_add_from_frags</fun>
     </di>

   </add-fun> */
FORCE_INLINE void
frags_init_add_from_frags(frags_t *dst, const frags_t *src,
                          size_t offset, size_t len)
{
        frags_init(dst);
        frags_add_from_frags(dst, src, offset, len);
}

/* <add-fun id="frags_api">
     <short>extract the contents of a <type>frags_t</type></short>

     Copy the whole contents of <param>buf</param> to <param>vdst</param>. The
     destination buffer <param>vdst</param> should be large enough to contain
     all data in <param>buf</param>.

     This function is completely equivalent to <fun>frags_extract_slice()</fun>
     with an <param>offset</param> and a <param>length</param> covering the
     whole contents of the <type>frags_t</type>, and is provided for
     convenience.

     <di name="RETURN VALUE">None</di>
     <di name="EXECUTION CONTEXT">Cell Context</di>
     <di name="EXAMPLE">
       <insert id="fg_extract"/>
     </di>
     <di name="SEE ALSO">
       <fun>frags_extract_8</fun>, <fun>frags_extract_slice</fun>,
       <fun>frags_extract_alloc</fun>, <fun>frags_extract_slice_alloc</fun>
     </di>

   </add-fun> */
void frags_extract(const frags_t *buf, void *vdst);

/* <add-fun id="frags_api">
     <short>extract a slice of a <type>frags_t</type></short>

     Copy a slice of size <param>len</param>, starting at offset
     <param>offset</param>, of the contents of <param>buf</param>, to
     <param>vdst</param>. The destination buffer <param>vdst</param> should be
     able to contain at least <param>len</param> bytes.

     <di name="RETURN VALUE">None</di>
     <di name="EXECUTION CONTEXT">Cell Context</di>
     <di name="EXAMPLE">
       <insert id="fg_extract_slice"/>
     </di>
     <di name="SEE ALSO">
       <fun>frags_extract_8</fun>, <fun>frags_extract</fun>,
       <fun>frags_extract_alloc</fun>, <fun>frags_extract_slice_alloc</fun>
     </di>

   </add-fun> */
void frags_extract_slice(const frags_t *buf, void *vdst, size_t offset, 
                         size_t len);

/* <add-fun id="frags_api">
     <short>return a copy of the contents of a <type>frags_t</type></short>

     Return an allocated copy of the contents of <param>buf</param>. The buffer
     returned is allocated with <fun>MM_MALLOC()</fun>, and its ownership is
     passed to the caller, which should free it when appropriate.

     This function is equivalent to allocating a buffer of the correct size
     with <fun>MM_MALLOC()</fun> followed by a call to
     <fun>frags_extract()</fun>, and is provided for convenience.

     <di name="RETURN VALUE">A newly allocated copy of the contents</di>
     <di name="EXECUTION CONTEXT">Cell Context</di>
     <di name="EXAMPLE">
       <insert id="fg_extract_alloc"/>
     </di>
     <di name="SEE ALSO">
       <fun>frags_extract</fun>, <fun>frags_extract_slice</fun>,
       <fun>frags_extract_slice_alloc</fun>
     </di>

   </add-fun> */
void *frags_extract_alloc(const frags_t *buf);

/* <add-fun id="frags_api">
     <short>return a partial copy of the contents of a
     <type>frags_t</type></short>

     Return an allocated copy of a slice of size <param>len</param>, starting
     at offset <param>offset</param>, of the contents of
     <param>buf</param>. The return value is allocated with
     <fun>MM_MALLOC()</fun>, and its ownership is passed to the caller, which
     should free it when appropriate.

     This function is equivalent to allocating a buffer of the correct size
     with <fun>MM_MALLOC()</fun> followed by a call to
     <fun>frags_extract_slice()</fun>, and is provided for convenience.

     <di name="RETURN VALUE">A newly allocated, partial copy of the data</di>
     <di name="EXECUTION CONTEXT">Cell Context</di>
     <di name="EXAMPLE">
       <insert id="fg_extract_slice_alloc"/>
     </di>
     <di name="SEE ALSO">
       <fun>frags_extract</fun>, <fun>frags_extract_slice</fun>,
       <fun>frags_extract_alloc</fun>
     </di>

   </add-fun> */
void *frags_extract_slice_alloc(const frags_t *buf, size_t offset, size_t len);

/* <add-fun id="frags_api">
     <short>prefix a <type>frags_t</type> with a header</short>

     Create a <type>frags_t</type> composed of the header <param>header</param>
     of size <param>header_len</param>, followed by the contents of the
     <type>frags_t</type> <param>body</param>.

     This function is equivalent to a sequence of <fun>frags_init()</fun>,
     <fun>frags_add()</fun>, <fun>frags_add_from_frags()</fun> to build a new
     fragment containing the prefix. It is provided for convenience.

     <di name="RETURN VALUE">A new <type>frags_t</type> including
     <param>header</param> and <param>body</param></di>

     <di name="EXECUTION CONTEXT">Cell Context</di>
     <di name="EXAMPLE">
       <insert id="fg_prefix"/>
     </di>
     <di name="SEE ALSO">
       <fun>frags_suffix</fun>, <fun>frags_add_from_frags</fun>
     </di>

   </add-fun> */
FORCE_INLINE frags_t
frags_prefix(const void *header, size_t header_len, const frags_t *body)
{
        frags_t buf;
        frags_init(&buf);
        frags_add(&buf, header, header_len);
        frags_add_from_frags(&buf, body, 0, frags_len(body));
        return buf;
}

/* <add-fun id="frags_api">
     <short>append a suffix to a <type>frags_t</type></short>

     Create a <type>frags_t</type> composed of the contents of the
     <type>frags_t</type> <param>body</param>, followed by the data
     <param>header</param> of size <param>header_len</param>.

     This function is equivalent to a sequence of <fun>frags_init()</fun>,
     <fun>frags_add_from_frags()</fun>, <fun>frags_add()</fun> to build a new
     fragment containing the suffix. It is provided for convenience.

     <di name="RETURN VALUE">A new <type>frags_t</type> including
     <param>body</param> and <param>header</param></di>

     <di name="EXECUTION CONTEXT">Cell Context</di>
     <di name="EXAMPLE">
       <insert id="fg_suffix"/>
     </di>
     <di name="SEE ALSO">
       <fun>frags_prefix</fun>, <fun>frags_add_from_frags</fun>
     </di>

   </add-fun> */
FORCE_INLINE frags_t
frags_suffix(const frags_t *body, void *header, size_t header_len)
{
        frags_t buf;
        frags_init(&buf);
        frags_add_from_frags(&buf, body, 0, frags_len(body));
        frags_add(&buf, header, header_len);
        return buf;
}

#define FRAGS_EXTRACTOR(End, end, bits)                                 \
FORCE_INLINE uint ## bits                                               \
frags_extract ## end ## bits (const frags_t *buf, size_t offset)        \
{                                                                       \
        uint ## bits tmp;                                               \
        frags_extract_slice(buf, &tmp, offset, bits / 8);               \
        return LOAD ## End ## bits (&tmp);                              \
}

FRAGS_EXTRACTOR(_BE, _, 8)
FRAGS_EXTRACTOR(_BE, _be, 16)
FRAGS_EXTRACTOR(_LE, _le, 16)
FRAGS_EXTRACTOR(_BE, _be, 32)
FRAGS_EXTRACTOR(_LE, _le, 32)
FRAGS_EXTRACTOR(_BE, _be, 64)
FRAGS_EXTRACTOR(_LE, _le, 64)

#ifdef DOC

/* <add-fun id="frags_api">
     <short>extract a value</short>

     Extract a 8, 16, 32 or 64 bits value in either big-endian (<fun>_be</fun>)
     or little-endian (<fun>_le</fun>) format from the contents of the
     <type>frags_t</type> <param>buf</param> at offset <param>offset</param>.

     <di name="RETURN VALUE">Extracted value</di>
     <di name="EXECUTION CONTEXT">Cell Context</di>
     <di name="EXAMPLE">
       <insert id="fg_extract_value"/>
     </di>
     <di name="SEE ALSO">
       <fun>frags_extract</fun>, <fun>frags_extract_slice</fun>
     </di>

   </add-fun> */
uint8 frags_extract_8(const frags_t *buf, size_t offset);
/* <append-fun id="frags_extract_8"/> */
uint16 frags_extract_be16(const frags_t *buf, size_t offset);
/* <append-fun id="frags_extract_8"/> */
uint16 frags_extract_le16(const frags_t *buf, size_t offset);
/* <append-fun id="frags_extract_8"/> */
uint32 frags_extract_be32(const frags_t *buf, size_t offset);
/* <append-fun id="frags_extract_8"/> */
uint32 frags_extract_le32(const frags_t *buf, size_t offset);
/* <append-fun id="frags_extract_8"/> */
uint64 frags_extract_be64(const frags_t *buf, size_t offset);
/* <append-fun id="frags_extract_8"/> */
uint64 frags_extract_le64(const frags_t *buf, size_t offset);

#endif  /* DOC */

#endif /* !defined(PYWRAP) */

#if defined(__cplusplus)
}
#endif

#endif
