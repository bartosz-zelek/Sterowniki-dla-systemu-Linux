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

#ifndef SIMICS_MODEL_IFACE_IMAGE_H
#define SIMICS_MODEL_IFACE_IMAGE_H

#include <simics/base/types.h>
#include <simics/base/memory.h>
#include <simics/pywrap.h>
#include <simics/simulator/configuration.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add-type id="image_spage_t def"></add-type> */
typedef struct image_spage image_spage_t;

/* <add id="image_interface_t">
   This interface is used for handling big data images.

   <fun>read</fun> and <fun>write</fun> access a chunk of data at a time.
   Only accesses within the bounds of the image are allowed.

   <fun>clear_range</fun> fills an interval with null bytes,
   <fun>fill</fun> with any byte value.

   <fun>size</fun> returns the image size.

   <fun>get</fun> and <fun>set</fun> work like <fun>read</fun>
   and <fun>write</fun> but pass the data using a bytes_t instead,
   and can be used from Python.

   <fun>flush_writable</fun> writes out all unwritten changes to a writable
   backing file if one exists; otherwise, does nothing.

   Other methods are not currently for public use.

   <insert-until text="// ADD INTERFACE image_interface"/>
   </add>
   <add id="image_interface_exec_context">
    <table border="false">
     <tr><td><fun>read</fun></td><td>Cell Context</td></tr>
     <tr><td><fun>write</fun></td><td>Cell Context</td></tr>
     <tr><td><fun>for_all_spages</fun></td><td>Cell Context</td></tr>
     <tr><td><fun>set_persistent</fun></td><td>Cell Context</td></tr>
     <tr><td><fun>save_to_file</fun></td><td>Cell Context</td></tr>
     <tr><td><fun>save_diff</fun></td><td>Cell Context</td></tr>
     <tr><td><fun>clear_range</fun></td><td>Cell Context</td></tr>
     <tr><td><fun>fill</fun></td><td>Cell Context</td></tr>
     <tr><td><fun>size</fun></td><td>Cell Context</td></tr>
     <tr><td><fun>set</fun></td><td>Cell Context</td></tr>
     <tr><td><fun>get</fun></td><td>Cell Context</td></tr>
     <tr><td><fun>flush_writable</fun></td><td>Cell Context</td></tr>
    </table>
   </add> */
SIM_INTERFACE(image) {
#if !defined(PYWRAP)
        void (*read)(conf_object_t *img, void *NOTNULL to_buf, uint64 start,
                     size_t length);
        void (*write)(conf_object_t *img, const void *NOTNULL from_buf,
                      uint64 start, size_t length);
        int (*for_all_spages)(conf_object_t *img,
                              int (*NOTNULL f)(image_spage_t *NOTNULL p,
                                               uint64 ofs, void *arg),
                              void *arg);
#endif /* not PYWRAP */
        void (*set_persistent)(conf_object_t *obj);
        void (*save_to_file)(conf_object_t *NOTNULL obj,
                             const char *NOTNULL file,
                             uint64 start, uint64 length, save_flags_t flags);
        void (*save_diff)(conf_object_t *NOTNULL obj,
                          const char *NOTNULL file, save_flags_t flags);
        void (*clear_range)(conf_object_t *NOTNULL obj,
                            uint64 start, uint64 length);
        void (*fill)(conf_object_t *NOTNULL obj,
                     uint64 start, uint64 length, uint8 value);
        uint64 (*size)(conf_object_t *NOTNULL obj);
        void (*set)(conf_object_t *NOTNULL obj, uint64 ofs, bytes_t b);
        bytes_t (*get)(conf_object_t *NOTNULL obj, uint64 ofs, size_t size);
        void (*flush_writable)(conf_object_t *NOTNULL obj);
};

#define IMAGE_INTERFACE "image"
// ADD INTERFACE image_interface

/* <add id="linear_image_interface_t">
     <note>
       This interface is an experimental feature. It is excluded from
       the standard support program, and is subject to change or removal
       without notice.
     </note>

     The <iface>linear_image</iface> interface permits direct access to the
     data in <class>image</class> objects by requesting a linear allocation
     for the contents. Doing so is not recommended for very large images,
     since there must be space for all data in memory as a contiguous
     block.

     <fun>get_base</fun> returns the linear allocation block if one has
     already been set. Otherwise, a block of the correct size is allocated,
     set and returned. In the latter case, the block is owned by the image
     object and should not be freed by the user. If <param>retsize</param>
     is non-null, it is used to return the size of the image.

     <fun>set_base</fun> specifies an existing memory block to be used for
     the image contents. The block must be at least the size of the image,
     and should be aligned to a multiple of 4096 bytes. The caller is
     responsible for the allocation of the block, which must remain
     allocated for the remaining lifetime of the image object.

     <fun>prepare_range</fun> must be called, with the matching access type,
     before any direct access to data in a linear block by user code.
     It is then permitted to access bytes in the range
     <math>
       [<param>offs</param>, <param>offs</param> + <param>size</param>)
     </math>. For <param>type</param> = <tt>Sim_RW_Write</tt>, the 
     permission to modify data in that range only extends until any other
     objects using the image have the opportunity to do so (typically, when
     the modelling function returns control to the simulator).

     <fun>set_base</fun> and <fun>get_base</fun> cannot be called after
     image data has been accessed (read or written) for the first time.

     <insert-until text="// ADD INTERFACE linear_image_interface"/>
   </add>
   <add id="linear_image_interface_exec_context">
     <table border="false">
       <tr>
         <td><fun>set_base</fun></td>
         <td>Global Context</td>
       </tr>
       <tr>
         <td><fun>get_base</fun></td>
         <td>Global Context</td>
       </tr>
       <tr>
         <td><fun>prepare_range</fun></td>
         <td>Cell Context</td>
       </tr>
     </table>
   </add>  */
SIM_INTERFACE(linear_image) {
#if !defined(PYWRAP)
        uint8 *(*get_base)(conf_object_t *obj, size_t *retsize);
        void (*set_base)(conf_object_t *obj, uint8 *NOTNULL base);
#endif /* not PYWRAP */
        void (*prepare_range)(conf_object_t *NOTNULL obj,
                              read_or_write_t type, size_t offs, size_t size);
};
#define LINEAR_IMAGE_INTERFACE "linear_image"
// ADD INTERFACE linear_image_interface

/* <add id="image_snoop_interface_t"> The image snoop interface is used to get
   information about when image pages are written to. Note that with the
   addition of inhibit bits in the <iface>direct_memory</iface> interface, the
   image snoop interface is rarely needed for model functionality.

   The <fun>page_modified</fun> function is always called the first time a page
   is written to. It may also be called additional times even if a page has
   already been written to. A user of the image snoop interface can at any time
   reset this mechanism so that all pages are considered not written to and
   therefore the <fun>page_modified</fun> function will be called again on
   future writes. The reset can be accomplished either through the memory page
   update interface or through the pool protect interface.

   The <iface>image snoop</iface> interface can, for example, be used by frame
   buffer devices to efficiently keep track of areas of the frame buffer to
   redraw, or for a CPU module that builds cached representations of code pages
   to invalidate such caches when memory is modified.

   Listeners using this interface are installed with the
   <attr>image_snoop_devices</attr> attribute in the
   <class>image</class> class.

   <insert-until text="// ADD INTERFACE image_snoop_interface"/>
   </add>
   <add id="image_snoop_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(image_snoop) {
        void (*page_modified)(conf_object_t *obj, conf_object_t *img,
                              uint64 offset, uint8 *page_data,
                              image_spage_t *spage);
};
#define IMAGE_SNOOP_INTERFACE "image_snoop"
// ADD INTERFACE image_snoop_interface

#if defined(__cplusplus)
}
#endif

#endif
