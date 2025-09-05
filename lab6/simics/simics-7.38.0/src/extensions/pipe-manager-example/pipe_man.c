/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

/* pipe manager: a class that receives and transmits data through a magic pipe
   connection to a pipe agent running in the target system. This code, together
   with the pipe agent, examplifies how to setup and communicate over a magic
   pipe. */

#include <magic_pipe_setup_interface.h>
#include <magic_pipe_reader_interface.h>
#include <magic_pipe_writer_interface.h>
#include <simics/simulator-api.h>
#include <simics/util/os.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>

#define PIPE_CLASS_NAME "pipe_manager"

/* The reserved Pipe Example magic number. */
#define PIPE_MAGIC 0xdc480f8f8ab10b87ULL

typedef struct {
        conf_object_t obj;

        /* The magic_pipe object to which we are connected, or NULL. */
        conf_object_t *pipe;
        const magic_pipe_setup_interface_t *pipe_su;
        const magic_pipe_reader_interface_t *pipe_rd;
        const magic_pipe_writer_interface_t *pipe_wr;

        /* Input file name and open file, if any. */
        char *ipath;
        FILE *ifile;

        /* Output file name and open file, if any. */
        char *opath;
        FILE *ofile;

        uint64_t magic;
        /* Hap counter, for debugging. */
        uint64_t haps;
} pipe_manager_t;

static pipe_manager_t *
pipe_manager_of_obj(conf_object_t *obj)
{
        static pipe_manager_t *pipe_man = NULL;
        static conf_class_t *pipe_class = NULL;
        if (pipe_man || !obj)
                return pipe_man;
        if (!pipe_class)
                pipe_class = SIM_get_class(PIPE_CLASS_NAME);
        conf_class_t *obj_class = SIM_object_class(obj);
        if (obj_class == pipe_class)
                pipe_man = (pipe_manager_t *)obj;
        return pipe_man;
}

/* Write the buffer data to the output file. The data may be returned in
   fragments and if so the function loops until all data is written to the file
   or an error occurs. */
static bytes_t
pipe_write_to_file(pipe_manager_t *man, uintptr_t bufh, bytes_t buf, FILE *outf)
{
        size_t offs = 0;
        while (buf.len) {
                size_t wrlen = fwrite(buf.data, 1, buf.len, outf);
                if (wrlen == 0)
                        break;
                offs += wrlen;
                buf = man->pipe_rd->read_data_direct(man->pipe, bufh, offs);
        }
        return buf;
}

/* Print the buffer data to the Simics CLI. The buffer data may be fragmented
   and if so the function will loop until all data is printed to the CLI, or an
   error occurs. */
static bytes_t
pipe_print_to_cli(pipe_manager_t *man, uintptr_t bufh, bytes_t buf)
{
        size_t offs = 0;
        while (buf.len) {
                SIM_write(buf.data, (int)buf.len);
                offs += buf.len;
                buf = man->pipe_rd->read_data_direct(man->pipe, bufh, offs);
        }
        return buf;
}

/* Protocol callback function prototype */
typedef int (*proto_handler_t)(pipe_manager_t *man, buffer_t *buf, void *desc);

/* Protocol callback vector type */
typedef struct proto_code {
        const char *name;
        proto_handler_t func;
        uint16_t code;
} proto_code_t;

/* Read data from the input file into the buffer. It will read as much data as
   is available and fits in the buffer. The buffer data may be fragmented this
   function will loop over those fragments. */
static buffer_t
pipe_read_from_file(pipe_manager_t *man, uintptr_t bufh, buffer_t buf,
                    FILE *inf)
{
        while (buf.len) {
                int rdlen = fread(buf.data, 1, buf.len, inf);
                if (rdlen > 0)
                        /* The write offset must be updated with the amount of
                           data used. */
                        man->pipe_wr->write_data_add(man->pipe, bufh, rdlen);
                if (rdlen < buf.len)
                        break;
                /* Gives direct access to the remaining space in the pipe
                   buffer. */
                buf = man->pipe_wr->write_data_direct(man->pipe, bufh);
        }
        return buf;
}

/* Reader protocol callback function. This function is registered with the
   magic pipe and called for the correct magic value in the buffer header. It
   is only allowed to read the buffer contents, and there may be more than one
   subscriber. */
static void
pipe_agent_reader(conf_object_t *cpu, uintptr_t bufh, uint64 magic)
{
        pipe_manager_t *man = pipe_manager_of_obj(NULL);
        man->haps++;
        size_t len = man->pipe_rd->read_buffer_size(man->pipe, bufh);
        if (!len)
                return;
        bytes_t buf = man->pipe_rd->read_data_direct(man->pipe, bufh, 0);
        if (man->ofile) {
                buf = pipe_write_to_file(man, bufh, buf, man->ofile);
                fflush(man->ofile);
        } else {
                buf = pipe_print_to_cli(man, bufh, buf);
        }
        SIM_LOG_INFO(4, cpu, 0, "Wrote %zu bytes to %s", buf.len, man->opath);
}

static void
close_input_file(pipe_manager_t *man)
{
        fclose(man->ifile);
        man->ifile = NULL;
        MM_FREE(man->ipath);
        man->ipath = NULL;
}

static void
close_output_file(pipe_manager_t *man)
{
        fclose(man->ofile);
        man->ofile = NULL;
        MM_FREE(man->opath);
        man->opath = NULL;
}


/* Writer protocol callback function. This function is registered with the
   magic pipe and called for the correct magic value in the buffer header. It
   is allowed to write to the buffer, as much as it wants to. Other subscribers
   will get their turn to write to the buffer if enough space is available
   Write data does not take an offset is appended to the end of the buffer. It
   is not possible to read any data because the buffer is cleared before
   writing starts. */
static void
pipe_agent_writer(conf_object_t *cpu, uintptr_t bufh, uint64 magic)
{
        pipe_manager_t *man = pipe_manager_of_obj(NULL);
        if (man->ifile) {
                buffer_t buf = man->pipe_wr->write_data_direct(man->pipe, bufh);
                buf = pipe_read_from_file(man, bufh, buf, man->ifile);
                if (feof(man->ifile))
                        close_input_file(man);
                SIM_LOG_INFO(4, cpu, 0, "Read %zu bytes from %s",
                             buf.len, man->ipath);
        }
}

/* Connect to the magic pipe, by registering callbacks for some magic
   numbers. */
static set_error_t
connect_to_pipe(pipe_manager_t *man, conf_object_t *pipe)
{
        const magic_pipe_setup_interface_t *psu =
                SIM_c_get_interface(pipe, MAGIC_PIPE_SETUP_INTERFACE);
        const magic_pipe_reader_interface_t *prd =
                SIM_c_get_interface(pipe, MAGIC_PIPE_READER_INTERFACE);
        const magic_pipe_writer_interface_t *pwr =
                SIM_c_get_interface(pipe, MAGIC_PIPE_WRITER_INTERFACE);
        if (!psu || !prd || !pwr)
                return Sim_Set_Interface_Not_Found;

        man->pipe = pipe;
        man->pipe_su = psu;
        man->pipe_rd = prd;
        man->pipe_wr = pwr;

        /* register a reader and writer function for the reserved magic
           number. */
        psu->register_reserved_pipe(pipe, &man->obj, man->magic,
                                    pipe_agent_reader, pipe_agent_writer);
        return Sim_Set_Ok;
}

/* Disconnect from the magic pipe and close all input and output files. */
static void
disconnect_pipe(pipe_manager_t *man)
{
        if (man->ifile)
                close_input_file(man);

        if (man->ofile)
                close_output_file(man);

        man->pipe_su->unregister_pipe(man->pipe, &man->obj, man->magic);

        man->pipe = NULL;
        man->pipe_su = NULL;
        man->pipe_rd = NULL;
        man->pipe_wr = NULL;
}

/* Allocate a pipe manager instance. */
static conf_object_t *
pipe_alloc_object(void *data)
{
        pipe_manager_t *man = MM_ZALLOC(1, pipe_manager_t);
        return &man->obj;
}

/* Initialize the pipe manager object instance. */
static void *
pipe_init_object(conf_object_t *obj, void *param)
{
        pipe_manager_t *man = pipe_manager_of_obj(obj);
        /* Make sure the object isn't checkpointable
           because it contains external system state. */
        VT_set_object_checkpointable(obj, false);
        man->magic = PIPE_MAGIC;
        return man;
}

/* Get the connected state of the pipe manager. */
static attr_value_t
pipe_get_pipe(void *param, conf_object_t *obj, attr_value_t *idx)
{
        pipe_manager_t *man = pipe_manager_of_obj(obj);
        return SIM_make_attr_object(man->pipe);
}

/* Set the pipe manager connected attribute, and either connect to or
   disconnect from the magic. Also register or unregister the callbacks. */
static set_error_t
pipe_set_pipe(void *param, conf_object_t *obj, attr_value_t *val,
              attr_value_t *idx)
{
        pipe_manager_t *man = pipe_manager_of_obj(obj);
        if (SIM_attr_is_nil(*val)) {
                /* Attribute set to NIL; disconnect. */
                if (man->pipe)
                        disconnect_pipe(man);
                return Sim_Set_Ok;
        }

        conf_object_t *pipe = SIM_attr_object(*val);
        if (man->pipe && man->pipe != pipe) {
                /* Already connected to another pipe; disconnect first. */
                disconnect_pipe(man);
        }

        return connect_to_pipe(man, pipe);
}

/* Get the hap count attribute */
static attr_value_t
pipe_get_haps(void *param, conf_object_t *obj, attr_value_t *idx)
{
        pipe_manager_t *man = pipe_manager_of_obj(obj);
        return SIM_make_attr_uint64(man->haps);
}

/* Set the hap count attribute. Only useful for debugging purposes. */
static set_error_t
pipe_set_haps(void *param, conf_object_t *obj, attr_value_t *val,
              attr_value_t *idx)
{
        pipe_manager_t *man = pipe_manager_of_obj(obj);
        man->haps = SIM_attr_integer(*val);
        return Sim_Set_Ok;
}

/* Get the hap count attribute */
static attr_value_t
pipe_get_magic(void *param, conf_object_t *obj, attr_value_t *idx)
{
        pipe_manager_t *man = pipe_manager_of_obj(obj);
        return SIM_make_attr_uint64(man->magic);
}

/* Set the hap count attribute. Only useful for debugging purposes. */
static set_error_t
pipe_set_magic(void *param, conf_object_t *obj, attr_value_t *val,
               attr_value_t *idx)
{
        pipe_manager_t *man = pipe_manager_of_obj(obj);
        if (man->pipe)
                return Sim_Set_Illegal_Value;
        man->magic = SIM_attr_integer(*val);
        if (!man->magic)
                man->magic = PIPE_MAGIC;
        return Sim_Set_Ok;
}

/* Get the input file attribute */
static attr_value_t
pipe_get_input(void *param, conf_object_t *obj, attr_value_t *idx)
{
        pipe_manager_t *man = pipe_manager_of_obj(obj);
        return SIM_make_attr_string(man->ipath);
}

/* Set the input file attribute */
static set_error_t
pipe_set_input(void *param, conf_object_t *obj, attr_value_t *val,
               attr_value_t *idx)
{
        pipe_manager_t *man = pipe_manager_of_obj(obj);
        const char *path = NULL;
        FILE *file = NULL;
        if (SIM_attr_is_string(*val)) {
                path = SIM_attr_string(*val);
                file = os_fopen(path, "rb");
                if (!file)
                        return Sim_Set_Illegal_Value;
        }

        if (man->ifile)
                close_input_file(man);

        if (file) {
                man->ipath = MM_STRDUP(path);
                man->ifile = file;
        }
        return Sim_Set_Ok;
}

/* Get the output file attribute */
static attr_value_t
pipe_get_output(void *param, conf_object_t *obj, attr_value_t *idx)
{
        pipe_manager_t *man = pipe_manager_of_obj(obj);
        return SIM_make_attr_string(man->opath);
}

/* Set the output file attribute */
static set_error_t
pipe_set_output(void *param, conf_object_t *obj, attr_value_t *val,
                attr_value_t *idx)
{
        pipe_manager_t *man = pipe_manager_of_obj(obj);
        const char *path = NULL;
        FILE *file = NULL;
        if (SIM_attr_is_string(*val)) {
                path = SIM_attr_string(*val);
                file = os_fopen(path, "wb");
                if (!file)
                        return Sim_Set_Illegal_Value;
        }

        if (man->ofile)
                close_output_file(man);

        if (file) {
                man->opath = MM_STRDUP(path);
                man->ofile = file;
        }
        return Sim_Set_Ok;
}

/* Register the pipe manager class and some attributes. */
void
init_local()
{
        class_data_t cdata = {
                .alloc_object = pipe_alloc_object,
                .init_object = pipe_init_object,
                .class_desc =
                "forwards data through a magic pipe connection",
                .description =
                "A class that receives and transmits data through a magic pipe"
                " connection to a pipe agent running in the target system.",
        };
        conf_class_t *cl = SIM_register_class(PIPE_CLASS_NAME, &cdata);

        SIM_register_typed_attribute(cl, "haps",
                                     pipe_get_haps, NULL,
                                     pipe_set_haps, NULL,
                                     Sim_Attr_Optional, "i", NULL,
                                     "Magic hap count");

        SIM_register_typed_attribute(cl, "input",
                                     pipe_get_input, NULL,
                                     pipe_set_input, NULL,
                                     Sim_Attr_Pseudo, "s|n", NULL,
                                     "Input file");

        SIM_register_typed_attribute(cl, "magic",
                                     pipe_get_magic, NULL,
                                     pipe_set_magic, NULL,
                                     Sim_Attr_Pseudo, "i", NULL,
                                     "Magic number of the pipe agent");

        SIM_register_typed_attribute(cl, "output",
                                     pipe_get_output, NULL,
                                     pipe_set_output, NULL,
                                     Sim_Attr_Pseudo, "s|n", NULL,
                                     "Output file");

        SIM_register_typed_attribute(cl, "pipe",
                                     pipe_get_pipe, NULL,
                                     pipe_set_pipe, NULL,
                                     Sim_Attr_Pseudo, "o|n", NULL,
                                     "Connected pipe object or NIL");
}
