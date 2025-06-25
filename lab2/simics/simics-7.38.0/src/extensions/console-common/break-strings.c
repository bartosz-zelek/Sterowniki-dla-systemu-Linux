/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "break-strings.h"
#include <simics/util/hashtab.h>
#include <simics/base/hap-producer.h>
#include <simics/base/log.h>
#include <simics/simulator/control.h>
#include <simics/simulator/hap-consumer.h>
#include <simics/simulator-iface/consoles.h>

#include <hs/hs.h>

// Store break string callback together with its user-supplied argument.
typedef struct break_handler {
        break_string_cb_t handler;
        lang_void *arg;
} break_handler_t;

// Data about one registered break string.
typedef struct break_string_data {
        // The break string.
        char *str;
        // Should break string be deactivated after match?
        bool oneshot;
        // Is break string being checked for matches?
        bool active;
        // Break ID used in hap callback.
        int64 hap_id;
        // Number of characters matched so far (deprecated).
        int num_matched_chars;
        // Callback when string matches.
        break_handler_t handler;

        // the break string escaped (useful for presenting to a user)
        char *str_escaped;

        // Is the string a regexp?
        bool regexp;

        hs_database_t *db;
        hs_stream_t *stream;
        hs_scratch_t *scratch;
} break_string_data_t;

struct console_break_data {
        conf_object_t *obj;      /* Object to use for logging and callbacks. */
        int log_group;           /* Log group to use. */

        // HAP id of next registered break string.
        int64 next_hap_id;
        // All registered break strings (also inactive ones.)
        VECT(break_string_data_t) break_strings;
};

// We use our own hap type for break strings.
static hap_type_t hap_break_string;

// Length of the longest proper prefix of s that is also a suffix of s.
static int
prefix_suffix(const char *s)
{
        int len = strlen(s);
        for (int i = 1; i < len; i++)
                if (memcmp(s, s + i, len - i) == 0)
                        return len - i;
        return 0;
}

static void
test_prefix_suffix()
{
        ASSERT(prefix_suffix("abcde") == 0);
        ASSERT(prefix_suffix("aaaa") == 3);
        ASSERT(prefix_suffix("abacaba") == 3);
        ASSERT(prefix_suffix("abeabcabeab") == 5);
}

// Length of the longest prefix of s being a proper suffix of
// the n first chars of s concatenated with c.
static int
mismatch_prefix(const char *s, char c, int n)
{
        if (n == 0)
                return (s[0] == c) ? 1 : 0;
        for (int i = n - 1; i >= 0; i--)
                if (s[i] == c && memcmp(s, s + n - i, i) == 0)
                        return i + 1;
        return 0;
}

static void
test_mismatch_prefix()
{
        ASSERT(mismatch_prefix("aaaaa", 'a', 0) == 1);
        ASSERT(mismatch_prefix("aaaaa", 'b', 4) == 0);
        ASSERT(mismatch_prefix("abcd", 'c', 3) == 0);
        ASSERT(mismatch_prefix("abcd", 'a', 3) == 1);
        ASSERT(mismatch_prefix("asan", 's', 3) == 2);
        ASSERT(mismatch_prefix("asdf", 'a', 0) == 1);
}

// Free break string data structure, including the break strings.
static void
clear_break_string_data(console_break_data_t *bd)
{
        VFORT(bd->break_strings, break_string_data_t, entry) {
                MM_FREE(entry.str);
                MM_FREE(entry.str_escaped);
        }
        VFREE(bd->break_strings);
}

typedef VECT(break_string_data_t) break_string_list_t;

typedef struct {
        break_string_list_t *matched;
        break_string_data_t *entry;
} hs_match_data_t;

static int
hs_match_cb(unsigned int id,
            unsigned long long from, unsigned long long to,
            unsigned int flags, void *ctx)
{
        hs_match_data_t *data = ctx;
        VADD(*data->matched, *data->entry);
        return 0;
}

static void
check_hs_matches(console_break_data_t *bd,
                 break_string_list_t *matched,
                 break_string_data_t *entry,
                 char value)
{
        hs_match_data_t data = {.matched = matched, .entry = entry};
        hs_error_t ret = hs_scan_stream(entry->stream, &value, 1, 0,
                                        entry->scratch, hs_match_cb, &data);
        if (ret != HS_SUCCESS)
                SIM_LOG_ERROR(bd->obj, bd->log_group,
                              "Regexp match error %d", ret);
}

/* Check all active break strings for match, using the new incoming character.
   For every matching break string:
   - if it has a non-NULL callback, it is called
   - the break string hap is signalled, with the particular break string hap ID
     as index, the console as object and the string as hap callback argument.
   - if there is no callback and no hap callback, the simulation is stopped. */
void
console_check_break_strings(console_break_data_t *bd, char value)
{
        break_string_list_t matched = VNULL;
        VFORI(bd->break_strings, i) {
                break_string_data_t entry = VGET(bd->break_strings, i);
                if (entry.active)
                        check_hs_matches(bd, &matched, &entry, value);
        }

        VFORT(matched, break_string_data_t, entry) {
                if (entry.handler.handler)
                        entry.handler.handler(bd->obj,
                                              entry.str, entry.hap_id,
                                              entry.handler.arg);
                int num_cb = SIM_c_hap_occurred_always(
                        hap_break_string, bd->obj, entry.hap_id, entry.str);
                
                if (!entry.handler.handler && num_cb == 0) {
                        strbuf_t buf = sb_newf(
                                "Break in %s on string \"%s\"",
                                SIM_object_name(bd->obj), entry.str_escaped);
                        SIM_LOG_INFO(1, bd->obj, bd->log_group,
                                     "%s", sb_str(&buf));
                        SIM_break_simulation(sb_str(&buf));
                        sb_free(&buf);
                }

                if (entry.oneshot) {
                        entry.active = false;
                        VSET(bd->break_strings, entry.hap_id, entry);
                }
        }
        VFREE(matched);
}

// Register new break string with specified callback (which can be NULL.)
static int64
add_breakpoint(console_break_data_t *bd, const char *str, break_string_cb_t cb,
               lang_void *arg, bool oneshot, bool regexp)
{
        hs_compile_error_t *error;
        hs_database_t *db;
        unsigned flags = 0;
        if (regexp)
                flags |= HS_FLAG_MULTILINE | HS_FLAG_UTF8 | HS_FLAG_UCP;
        if (oneshot)
                flags |= HS_FLAG_SINGLEMATCH;

        hs_error_t ret;
        if (regexp)
                ret = hs_compile(str, flags, HS_MODE_STREAM, NULL, &db, &error);
        else
                ret = hs_compile_lit(str, flags, strlen(str),
                                     HS_MODE_STREAM, NULL, &db, &error);

        if (ret != HS_SUCCESS) {
                SIM_LOG_ERROR(bd->obj, bd->log_group,
                              "Regexp compilation error: %s\n",
                              error->message);
                hs_free_compile_error(error);
                return -1;
        }

        hs_stream_t *stream;
        ret = hs_open_stream(db, 0, &stream);
        if (ret != HS_SUCCESS) {
                SIM_LOG_ERROR(bd->obj, bd->log_group,
                              "Could not open regexp stream\n");
                return -1;
        }

        hs_scratch_t *scratch = NULL;
        ret = hs_alloc_scratch(db, &scratch);
        if (ret != HS_SUCCESS) {
                SIM_LOG_ERROR(bd->obj, bd->log_group,
                              "Could not allocate regexp scratch\n");
                return -1;
        }

        strbuf_t str_escaped = sb_new(str);
        if (!regexp) {
                sb_escape(&str_escaped, 0);
        }

        break_string_data_t entry = {
                .str = MM_STRDUP(str), .hap_id = bd->next_hap_id++,
                .active = true, .oneshot = oneshot, .regexp = regexp,
                .handler = {.handler = cb, .arg = arg},
                .str_escaped = sb_detach(&str_escaped),
                .db = db, .stream = stream, .scratch = scratch};
        VADD(bd->break_strings, entry);
        return entry.hap_id;
}

int64
bs_add(console_break_data_t *bd, const char *str,
       break_string_cb_t cb, lang_void *arg)
{
        return add_breakpoint(bd, str, cb, arg, false, false);
}

int64
bs_add_single(console_break_data_t *bd, const char *str,
              break_string_cb_t cb, lang_void *arg)
{
        return add_breakpoint(bd, str, cb, arg, true, false);
}

int64
bs_add_regexp(console_break_data_t *bd, const char *str,
              break_string_cb_t cb, lang_void *arg)
{
        return add_breakpoint(bd, str, cb, arg, false, true);
}

void
bs_remove(console_break_data_t *bd, int64 bp_id)
{
        if (bp_id >= 0 && bp_id < VLEN(bd->break_strings)) {
                // Just deactivate the string and keep it in the list.
                break_string_data_t entry = VGET(bd->break_strings, bp_id);
                if (entry.active) {
                        entry.active = false;
                        VSET(bd->break_strings, bp_id, entry);

                        if (hs_close_stream(entry.stream, entry.scratch,
                                            NULL, NULL) != HS_SUCCESS)
                                SIM_LOG_ERROR(bd->obj, bd->log_group,
                                              "Could not close regexp stream");
                        if (hs_free_scratch(entry.scratch) != HS_SUCCESS)
                                SIM_LOG_ERROR(bd->obj, bd->log_group,
                                              "Could not free regexp scratch");
                        if (hs_free_database(entry.db) != HS_SUCCESS)
                                SIM_LOG_ERROR(bd->obj, bd->log_group,
                                              "Could not free regexp");
                }
        }
}

attr_value_t
bs_attr_break_strings(console_break_data_t *bd)
{
        attr_value_t ret = SIM_alloc_attr_list(VLEN(bd->break_strings));
        VFORI(bd->break_strings, i) {
                break_string_data_t entry = VGET(bd->break_strings, i);
                attr_value_t list = SIM_make_attr_list(
                        6,
                        SIM_make_attr_string(entry.str),
                        SIM_make_attr_boolean(entry.active),
                        SIM_make_attr_boolean(entry.oneshot),
                        SIM_make_attr_int64(entry.hap_id),
                        SIM_make_attr_uint64(entry.num_matched_chars),
                        SIM_make_attr_boolean(entry.regexp));
                SIM_attr_list_set_item(&ret, i, list);                
        }
        return ret;
}

static void
local_tests()
{
        test_prefix_suffix();
        test_mismatch_prefix();
}

static void *
hs_alloc(size_t size)
{
        return MM_MALLOC(size, uint8);
}

static void
hs_free(void *ptr)
{
        return MM_FREE(ptr);
}

console_break_data_t *
make_break_strings(conf_object_t *obj, int log_group)
{
        console_break_data_t *bd = MM_ZALLOC(1, console_break_data_t);
        bd->obj = obj;
        bd->log_group = log_group;
        VINIT(bd->break_strings);
        return bd;
}

void
destroy_break_strings(console_break_data_t *bd)
{
        clear_break_string_data(bd);
        MM_FREE(bd);
}

void
register_break_strings(conf_class_t *cl)
{
        local_tests();

        hap_break_string = SIM_hap_add_type(
                "Console_Break_String",
                "s", "break_string",
                "break_id",
                "Triggered when the output matches a string set to"
                " break on. The <i>break_id</i> is the number"
                " associated with the string breakpoint.", 1);

        hs_error_t ret = hs_set_allocator(hs_alloc, hs_free);
        ASSERT(ret == HS_SUCCESS);
}
                
