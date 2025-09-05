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

#ifndef SIMICS_UTIL_STRBUF_H
#define SIMICS_UTIL_STRBUF_H

#if !defined PYWRAP && !defined GULP_API_HELP

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <simics/build-id.h>
#include <simics/host-info.h>
#include <simics/base-types.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* Please do not access the members of this struct directly;
   use the accessor functions defined in this file.
   Invariants:
   - .s[.len] == '\0'
   - If .size == 0, then .len == 0 and .s points to a static null byte
   - If .size > 0, then 0 <= .len < .size 
     and .s points to an allocation of .size bytes. */
typedef struct {
	char *s;		/* string, always 0-terminated */
	unsigned size;		/* size of allocated buffer */
	unsigned len;		/* current length */
} strbuf_t;

strbuf_t sb_new(const char *s);
strbuf_t sb_newf(const char *format, ...) PRINTF_FORMAT(1, 2);
void sb_free(strbuf_t *sb);
char *sb_detach(strbuf_t *sb);
void sb_realloc(strbuf_t *sb, unsigned minlen);
int sb_write(const strbuf_t *sb, FILE *f);
bool sb_readline(strbuf_t *sb, FILE *f);
void sb_vaddfmt(strbuf_t *sb, const char *format, va_list va)
        PRINTF_FORMAT(2, 0);
void sb_addfmt(strbuf_t *sb, const char *format, ...) PRINTF_FORMAT(2, 3);
void sb_vfmt(strbuf_t *sb, const char *format, va_list va) PRINTF_FORMAT(2, 0);
void sb_fmt(strbuf_t *sb, const char *format, ...) PRINTF_FORMAT(2, 3);
/* deprecated, to be removed in 8 */
#if SIM_VERSION < BUILD_ID_SIM_8
void sb_addesc(strbuf_t *sb, char c, char delim);
#endif
void sb_escape(strbuf_t *buf, char delim);

/* return length of string, excluding nul */
FORCE_INLINE int
sb_len(const strbuf_t *sb)
{
	return (int)sb->len;
}

/* return string as C-string, always 0-terminated */
FORCE_INLINE char *
sb_str(const strbuf_t *sb)
{
	return sb->s;
}

/* Return the ith character in the string. Count from end of string if
   i is negative (-1 means the last character, 0 the first).
   No bounds checking is done; i must obey -sb_len(s) ≤ i < sb_len(s). */
FORCE_INLINE char
sb_char(const strbuf_t *sb, int i)
{
        if (i >= 0)
                return sb->s[i];
        else 
                return sb->s[(int)sb->len + i];
}


#define SB_INIT {(char *)"", 0, 0}

/* Initialize a string which has not been initialized yet.
   This is equivalent to initialising it with SB_INIT.
   Either sb_init() or initialisation to SB_INIT must be done before a string
   can be used. */
FORCE_INLINE void
sb_init(strbuf_t *sb)
{
	sb->s = (char *)"";
	sb->size = 0;
	sb->len = 0;
}

/* Extend storage of sb to hold at least minlen chars, including terminating
   nul. There should be no need to call this function directly; other
   functions do so when necessary. */
FORCE_INLINE void
sb_extend(strbuf_t *sb, unsigned minlen)
{
	/* fast path */
	if (minlen > sb->size)
		sb_realloc(sb, minlen);
}

/* set a string to the contents of a C-string */
FORCE_INLINE void
sb_set(strbuf_t *sb, const char *str)
{
	unsigned len = (unsigned)strlen(str);
	sb_extend(sb, len + 1);
	memcpy(sb->s, str, len + 1);
        sb->len = len;
}

/* set a string to the contents of another strbuf */
FORCE_INLINE void
sb_copy(strbuf_t *dst, const strbuf_t *src)
{
	sb_extend(dst, src->len + 1);
	memcpy(dst->s, src->s, src->len + 1);
	dst->len = src->len;
}

/* make a string empty */
FORCE_INLINE void
sb_clear(strbuf_t *sb)
{
	sb->len = 0;
        if (sb->size)
                sb->s[0] = '\0';
}

/* append sb2 to sb1 */
FORCE_INLINE void
sb_cat(strbuf_t *sb1, const strbuf_t *sb2)
{
	sb_extend(sb1, sb1->len + sb2->len + 1);
	memcpy(sb1->s + sb1->len, sb2->s, sb2->len + 1);
	sb1->len += sb2->len;
}

/* append a C-string to a strbuf */
FORCE_INLINE void
sb_addstr(strbuf_t *sb, const char *str)
{
	unsigned len = (unsigned)strlen(str);
	sb_extend(sb, sb->len + len + 1);
	memcpy(sb->s + sb->len, str, len + 1);
	sb->len += len;
}

/* append a counted string to a strbuf */
FORCE_INLINE void
sb_addmem(strbuf_t *sb, const char *str, unsigned len)
{
	sb_extend(sb, sb->len + len + 1);
	memcpy(sb->s + sb->len, str, len);
	sb->len += len;
        sb->s[sb->len] = '\0';
}

/* add a character to a string */
FORCE_INLINE void
sb_addc(strbuf_t *sb, char c)
{
	sb_extend(sb, sb->len + 2);
	sb->s[sb->len++] = c;
	sb->s[sb->len] = '\0';
}

/* add a character repeated a given number of times to a string */
FORCE_INLINE void
sb_addchars(strbuf_t *sb, char c, unsigned n)
{
	sb_extend(sb, sb->len + n + 1);
	memset(sb->s + sb->len, c, n);
	sb->len += n;
	sb->s[sb->len] = '\0';
}

/* Resize string. If extended, pad with null bytes. */
FORCE_INLINE void
sb_resize(strbuf_t *sb, unsigned size)
{
	if (size > sb->len) {
		sb_extend(sb, size + 1);
		memset(sb->s + sb->len, 0, size - sb->len);
	}
	sb->len = size;
	sb->s[size] = '\0';
}

/* return a copy of a string */
FORCE_INLINE strbuf_t
sb_dup(const strbuf_t *sb)
{
	strbuf_t new_sb = SB_INIT;
	sb_cat(&new_sb, sb);
	return new_sb;
}

/* Delete at most n characters from position start.
   If start is negative, count from the end. */
FORCE_INLINE void
sb_delete(strbuf_t *sb, int start, unsigned n)
{
        unsigned s = (start >= 0
                      ? (unsigned)start
                      : sb->len + (unsigned)start);

        /* Avoid deleting too many characters. */
        if (s + n > sb->len)
                n = sb->len - s;

        /* For an empty strbuf, sb->s points to a read-only location that must
           not be passed to memmove. */
        if (sb->len > 0)
                memmove(sb->s + s, sb->s + s + n, sb->len - (s + n) + 1);
        sb->len -= n;
}

/* Insert n characters from str at index start (start <= sb_len(sb)) */
FORCE_INLINE void
sb_insertmem(strbuf_t *sb, unsigned start, const char *str, unsigned n)
{
        sb_extend(sb, sb->len + n + 1);
        memmove(sb->s + start + n, sb->s + start, sb->len - start);
        memcpy(sb->s + start, str, n);
        sb->len += n;
        sb->s[sb->len] = '\0';
}

/* Insert a zero-terminated string at index start (start <= sb_len(sb)) */
FORCE_INLINE void
sb_insertstr(strbuf_t *sb, unsigned start, const char *str)
{
        sb_insertmem(sb, start, str, (unsigned)strlen(str));
}

#if defined(__cplusplus)
}
#endif

#endif  /* not PYWRAP and not GULP_API_HELP */

#endif
