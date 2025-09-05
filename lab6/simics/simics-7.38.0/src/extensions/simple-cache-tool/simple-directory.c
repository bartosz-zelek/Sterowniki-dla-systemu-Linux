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

#include "simple-directory.h"

void
dir_insert(directory_t *d, physical_address_t pa, cache_line_t *cl)
{
        uint64 line_pa = pa >> d->cache_size_log2;
        unsigned i = (line_pa) & DIRECTORY_SIZE_MASK;
        directory_entry_t **prev = &d->entries[i];
        directory_entry_t *e = *prev;

        while (e) {
                if (e->line_address == line_pa) {
                        VADD(e->present_lines, cl);
                        return;
                }
                prev = &e->next;
                e = e->next;
        }

        e = MM_ZALLOC(1, directory_entry_t);
        e->line_address = line_pa;
        VADD(e->present_lines, cl);
        *prev = e;
}

void
dir_remove(directory_t *d, physical_address_t pa, cache_line_t *cl)
{
        uint64 line_pa = pa >> d->cache_size_log2;
        unsigned i = (line_pa) & DIRECTORY_SIZE_MASK;
        directory_entry_t **prev = &d->entries[i];
        directory_entry_t *e = *prev;

        while (e) {
                if (e->line_address == line_pa) {
                        VREMOVE_FIRST_MATCH(e->present_lines, cl);
                        if (VEMPTY(e->present_lines)) {
                                VFREE(e->present_lines); // may still use space
                                *prev = e->next;
                                MM_FREE(e);
                                return;
                        }
                }

                prev = &e->next;
                e = e->next;
        }
}

dir_lookup_info_t
dir_lookup(directory_t *d, physical_address_t pa)
{
        uint64 line_pa = pa >> d->cache_size_log2;
        int i = (line_pa) & DIRECTORY_SIZE_MASK;
        directory_entry_t **prev = &d->entries[i];
        directory_entry_t *e = *prev;

        while (e) {
                if (line_pa == e->line_address) {
                        return (dir_lookup_info_t) { .entry = e, .prev = prev };
                }
                prev = &e->next;
                e = e->next;
        }
        return (dir_lookup_info_t) { .entry = NULL, .prev = NULL };
}

/* Allocate the cache */
static conf_object_t *
alloc_object(void *arg)
{
        directory_t *dir = MM_ZALLOC(1, directory_t);
        return &dir->obj;
}

static int
delete_instance(conf_object_t *obj)
{
        directory_t *dir = obj_to_dir(obj);

        for (int i = 0; i < DIRECTORY_SIZE; i++) {
                for (directory_entry_t *e = dir->entries[i]; e;) {
                        VFREE(e->present_lines);
                        directory_entry_t *next = e->next;
                        MM_FREE(e);
                        e = next;
                }
        }
        return 0;
}

static attr_value_t
get_directory(conf_object_t *obj)
{
        directory_t *dir = obj_to_dir(obj);
        return SIM_make_attr_object(&dir->next_directory->obj);
}

static set_error_t
set_directory(conf_object_t *obj, attr_value_t *val)
{
        directory_t *d = obj_to_dir(obj);
        if (SIM_attr_is_invalid(*val))
                d->next_directory = NULL;
        else
                d->next_directory = obj_to_dir(SIM_attr_object(*val));
        return Sim_Set_Ok;
}

void
register_directory_class()
{
        const class_data_t funcs = {
                .alloc_object = alloc_object,
                .delete_instance = delete_instance,
                .description = "Directory controlling a particular cache level"
                " a in coherent cache configuration.",
                .class_desc = "directory controlling cache level",
                .kind = Sim_Class_Kind_Pseudo
        };

        conf_class_t *dir_class = SIM_register_class("simple_directory", &funcs);

        /* register cache attributes */
        SIM_register_attribute(
                dir_class, "next_directory",
                get_directory,
                set_directory,
                Sim_Attr_Optional,
                "o|n", "Directory of next level");
        
}
