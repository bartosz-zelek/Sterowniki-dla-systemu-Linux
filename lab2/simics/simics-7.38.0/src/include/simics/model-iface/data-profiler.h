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

#ifndef SIMICS_MODEL_IFACE_DATA_PROFILER_H
#define SIMICS_MODEL_IFACE_DATA_PROFILER_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(SIMICS_6_API) || defined(SHOW_OBSOLETE_API)
#define DEPRECATED_FUNC(f) f
#else
#define DEPRECATED_FUNC(f) _deprecated_ ## f
#endif

/* This interface allows manipulation of the profiling data stored in an
   object. (Note that not all objects that store profiling data implement this
   interface.) Internal use only. */
SIM_INTERFACE(data_profiler) {
        uint64 (*DEPRECATED_FUNC(accum_in_range))(conf_object_t *profiler,
                                                  uint64 start, uint64 end);

        /* write the profiling data to a file */
        void (*save)(conf_object_t *profiler, const char *NOTNULL file);

        /* read profiling data from a file, and adds it to whatever data was
           already present in the profiler. */
        void (*load)(conf_object_t *profiler, const char *NOTNULL file);

        /* resets all counters in the profiler to zero. */
        void (*clear)(conf_object_t *profiler);

#if !defined(PYWRAP)
        /* actually returns a prof_data_t* */
        void *(*get_prof_data)(conf_object_t *profiler);
#endif
};

#define DATA_PROFILER_INTERFACE "data_profiler"
// ADD INTERFACE data_profiler_interface

#undef DEPRECATED_FUNC

#if defined(__cplusplus)
}
#endif

#endif
