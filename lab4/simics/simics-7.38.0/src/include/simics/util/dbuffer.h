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

#ifndef SIMICS_UTIL_DBUFFER_H
#define SIMICS_UTIL_DBUFFER_H

/*
   <add id="device api types obsolete">
   <name index="true">dbuffer_t</name>
   <insert id="dbuffer_t DOC"/>
   </add> */

/* <add id="dbuffer_t DOC">
   <ndx>dbuffer_t</ndx>
   <doc>
   <doc-item name="NAME">dbuffer_t</doc-item>
   <doc-item name="SYNOPSIS">
   <smaller>
   <insert id="dbuffer_t def"/>
   </smaller>
   </doc-item>
   <doc-item name="DESCRIPTION">
   This type is used to store blocks of binary data.  It is optimized
   for fast adding and removing of data, and does fast copying between
   buffers using copy-on-write semantics.

   The type is not inherently thread safe, so each instance must have a single
   thread as owner, and only the owner can read or write from the instance,
   however ownership can be transferred to another thread. To share the data
   with other threads, the instance must first be cloned using
   <fun>dbuffer_clone</fun>.

   <note>This is a legacy data type. New code should use one of
   <tt>frags_t</tt>, <tt>bytes_t</tt> or <tt>buffer_t</tt>.</note>

   </doc-item> </doc> </add>
*/

#include <simics/base/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add-type id="dbuffer_t def"></add-type> */
typedef struct dbuffer dbuffer_t;

#if !defined(PYWRAP) && !defined(STANDALONE)

/* Get a new buffer */
EXPORTED dbuffer_t *new_dbuffer(void);

/* Release a buffer.  The buffer should not be freed in any other
   way.  */
EXPORTED void dbuffer_free(dbuffer_t *dbuffer);

/* Get the length */
EXPORTED size_t dbuffer_len(const dbuffer_t *dbuffer);

/*
 * Add static data.  This will make the buffer reference the data
 * pointed to directly, without making a copy.
 *
 * If the 'adopt' flag is true, the control of the data block is
 * transferred to the dbuffer.  It is assumed to be a MM_MALLOCed
 * block that will be MM_FREEd when the dbuffer is freed.
 *
 * If the 'adopt' flag is false, the dbuffer will not free the
 * memory. Instead it is up to the caller to free the memory, but it
 * must not do so before the dbuffer is freed.  Actually, this
 * reference could be copied to other dbuffers, so great care has to
 * be taken.  This should only be used for buffers that will only be
 * read, since it hard to know if a write operation will actually
 * write to the buffer or to a copy.
*/
EXPORTED uint8 *dbuffer_append_external_data(
        dbuffer_t *dbuffer, void *data, size_t len, bool adopt);
EXPORTED uint8 *dbuffer_prepend_external_data(
        dbuffer_t *dbuffer, void *data, size_t len, bool adopt);

/* Copy from one buffer to another. */
EXPORTED void dbuffer_copy_append(
        dbuffer_t *dst, dbuffer_t *src, size_t offset, size_t len);
EXPORTED void dbuffer_copy_prepend(
        dbuffer_t *dst, dbuffer_t *src, size_t offset, size_t len);

/* Add a repeating byte value */
EXPORTED uint8 *dbuffer_append_value(
        dbuffer_t *dbuffer, int value, size_t len);
EXPORTED uint8 *dbuffer_prepend_value(
        dbuffer_t *dbuffer, int value, size_t len);

/* Add uninitialized memory */
EXPORTED uint8 *dbuffer_append(dbuffer_t *dbuffer, size_t len);
EXPORTED uint8 *dbuffer_prepend(dbuffer_t *dbuffer, size_t len);
EXPORTED uint8 *dbuffer_insert(dbuffer_t *dbuffer, size_t offset, size_t len);

/* Make the buffer smaller */
EXPORTED void dbuffer_remove(
        dbuffer_t *dbuffer, size_t offset, size_t remove_len);
EXPORTED void dbuffer_remove_head(dbuffer_t *dbuffer, size_t remove_len);
EXPORTED void dbuffer_remove_tail(dbuffer_t *dbuffer, size_t remove_len);

static inline void 
dbuffer_truncate(dbuffer_t *dbuffer, size_t new_size)
{
        dbuffer_remove_tail(dbuffer, dbuffer_len(dbuffer) - new_size);
}

/*
 * Extract data for reading or writing.  The 'offset' and 'len'
 * parameter specify a region of the buffer to read from or write to.
 * The returned pointer is guaranteed to point to a contiguous block
 * of memory.
 *
 * The returned pointer is only valid until the next operation on the
 * dbuffer.  The only operation that is guaranteed not to invalidate
 * the pointer is the dbuffer_len() function.
 *
 * The *_some versions of these functions take an 'actual_len'
 * parameter, and may return a smaller buffer than requested.  The
 * actual number of valid bytes in the returned buffer is then written
 * to *actual_len, and is smaller if it would have had to copy data to
 * return a pointer to the whole region.  If NULL is passed for
 * 'actual_len', it will force the functions to return the full
 * region.
 *
 * The other functions always create a complete region, equivalent to
 * passing NULL in the 'actual_len' parameter.
 *
 * The *_all versions of these functions assume 0 for 'offset', and
 * buffer_len(dbuffer) for 'len'.
 */

/* The pointer returned by dbuffer_read() may point to memory shared
   with other buffers, and should not be written to. */
EXPORTED const uint8 *dbuffer_read(
        dbuffer_t *dbuffer, size_t offset, size_t len);
EXPORTED const uint8 *dbuffer_read_some(dbuffer_t *dbuffer, size_t offset,
                                        size_t len, size_t *actual_len);
EXPORTED const uint8 *dbuffer_read_all(dbuffer_t *dbuffer);

/* A buffer returned by dbuffer_update() is not used by any other
   dbuffer_t and can be freely read from or written to. */
EXPORTED uint8 *dbuffer_update(dbuffer_t *dbuffer, size_t offset, size_t len);
EXPORTED uint8 *dbuffer_update_some(dbuffer_t *dbuffer, size_t offset,
                                    size_t len, size_t *actual_len);
EXPORTED uint8 *dbuffer_update_all(dbuffer_t *dbuffer);

/* A buffer returned by dbuffer_replace() is not used by any other
   dbuffer_t and may contain junk.  This function should only be used
   when the whole buffer section will be overwritten with new data. */
EXPORTED uint8 *dbuffer_replace(dbuffer_t *dbuffer, size_t offset, size_t len);
EXPORTED uint8 *dbuffer_replace_some(dbuffer_t *dbuffer, size_t offset,
                                     size_t len, size_t *actual_len);
EXPORTED uint8 *dbuffer_replace_all(dbuffer_t *dbuffer);

/* Make a full copy of another buffer */
EXPORTED dbuffer_t *dbuffer_clone(dbuffer_t *dbuffer);

/* Split a buffer in two.  The data after 'offset' is left in the old
   dbuffer, and the data before 'offset' is returned in a newly
   allocated dbuffer. */
EXPORTED dbuffer_t *dbuffer_split(dbuffer_t *dbuffer, size_t offset);

EXPORTED void dbuffer_clear(dbuffer_t *dbuffer);
EXPORTED void dbuffer_set(dbuffer_t *dbuffer, int val, size_t len);

EXPORTED bytes_t dbuffer_bytes(dbuffer_t *dbuffer);

/* Append data to the end of the dbuffer buf. The dbuffer's size will
   increase by nelem bytes and nelem bytes will be copied from the
   data pointed to by data into the dbuffer. */
static inline void
dbuffer_append_data(dbuffer_t *buf, const void *data, size_t nmemb)
{
        memcpy(dbuffer_append(buf, nmemb), data, nmemb);
}

#endif /* !PYWRAP */

#if defined(__cplusplus)
}
#endif

#endif
