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

#ifndef SIMICS_BASE_LOG_H
#define SIMICS_BASE_LOG_H

#include <simics/base/types.h>
#include <simics/base/conf-object.h>
#include <simics/util/help-macros.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
   <add id="device api types obsolete">
   <name index="true">log_type_t</name>
   <insert id="log_type_t DOC"/>
   </add> */

/* <add id="log_type_t DOC">
   <ndx>log_type_t</ndx>
   <doc>
   <doc-item name="NAME">log_type_t</doc-item>
   <doc-item name="SYNOPSIS">
   <smaller>
   <insert id="log_type_t def"/>
   </smaller>
   </doc-item>
   <doc-item name="DESCRIPTION">
   This type defines different log types that are used by the logging
   facility to categorise messages.
   </doc-item>
   </doc>
   </add>
*/

/* <add-type id="log_type_t def">
   </add-type>
 */
typedef enum {
        Sim_Log_Info,           // Normal informational message
        Sim_Log_Error,          // Simics error
        Sim_Log_Spec_Violation, // target program violates the specification
        Sim_Log_Unimplemented,  // not implemented in Simics
        Sim_Log_Critical,       // Critical error stopping Simics
        Sim_Log_Trace,          // Breakpoint trace messages
        Sim_Log_Warning,        // Simics warning
        Sim_Log_Num_Types,      // Do not use
} log_type_t;

#define MIN_LOG_LEVEL 1
#define MAX_LOG_LEVEL 4

typedef struct log_object log_object_t;

EXPORTED void SIM_log_register_groups(conf_class_t *NOTNULL cls,
                                      const char *const *NOTNULL names);
EXPORTED void VT_log_message(conf_object_t *NOTNULL obj, int level,
                             int group_ids,
                             log_type_t log_type, const char *NOTNULL message);
EXPORTED void VT_log_message64(conf_object_t *NOTNULL obj, int level,
                               uint64 group_ids, log_type_t log_type,
                               const char *NOTNULL message);
EXPORTED void SIM_log_message(conf_object_t *NOTNULL obj, int level,
                              uint64 group_ids, log_type_t log_type,
                              const char *NOTNULL message);

EXPORTED COLD_FUNCTION void VT_critical_error(const char *NOTNULL short_msg,
                                              const char *NOTNULL long_msg);

#if !defined(PYWRAP)

EXPORTED void SIM_log_error(conf_object_t *NOTNULL dev, int grp,
                            const char *NOTNULL str, ...)
        PRINTF_FORMAT(3, 4);
EXPORTED void SIM_log_critical(conf_object_t *NOTNULL dev, int grp,
                               const char *NOTNULL str, ...)
        PRINTF_FORMAT(3, 4);
EXPORTED void SIM_log_info(int lvl, conf_object_t *NOTNULL dev, int grp,
                           const char *NOTNULL str, ...)
        PRINTF_FORMAT(4, 5);
EXPORTED void SIM_log_spec_violation(int lvl, conf_object_t *NOTNULL dev,
                                     int grp, const char *NOTNULL str, ...)
        PRINTF_FORMAT(4, 5);
EXPORTED void SIM_log_unimplemented(int lvl, conf_object_t *NOTNULL dev,
                                    int grp, const char *NOTNULL str, ...)
        PRINTF_FORMAT(4, 5);

EXPORTED void VT_log_warning(conf_object_t *NOTNULL dev, uint64 grp,
                             const char *NOTNULL str, ...)
        PRINTF_FORMAT(3, 4);
EXPORTED void VT_log_error(conf_object_t *NOTNULL dev, uint64 grp,
                           const char *NOTNULL str, ...)
        PRINTF_FORMAT(3, 4);
EXPORTED void VT_log_critical(conf_object_t *NOTNULL dev, uint64 grp,
                              const char *NOTNULL str, ...)
        PRINTF_FORMAT(3, 4);
EXPORTED void VT_log_info(int lvl, conf_object_t *NOTNULL dev, uint64 grp,
                          const char *NOTNULL str, ...)
        PRINTF_FORMAT(4, 5);
EXPORTED void VT_log_spec_violation(int lvl, conf_object_t *NOTNULL dev,
                                    uint64 grp, const char *NOTNULL str, ...)
        PRINTF_FORMAT(4, 5);
EXPORTED void VT_log_unimplemented(int lvl, conf_object_t *NOTNULL dev,
                                   uint64 grp, const char *NOTNULL str, ...)
        PRINTF_FORMAT(4, 5);

EXPORTED void SIM_log_message_vararg(conf_object_t *NOTNULL obj, int level,
                                     uint64 grp, log_type_t log_type,
                                     const char *NOTNULL fmt, ...)
        PRINTF_FORMAT(5, 6);

#endif /* !PYWRAP */

EXPORTED PURE_FUNCTION unsigned SIM_log_level(
        const conf_object_t *NOTNULL obj);
EXPORTED void SIM_set_log_level(conf_object_t *NOTNULL obj, unsigned level);

EXPORTED PURE_FUNCTION int VT_log_always_count(void);
EXPORTED unsigned VT_effective_log_level(const conf_object_t *NOTNULL obj);

/* These macros are useful when the log string is expensive to
   create. If logging isn't active, the format string arguments will
   not be evaluated. */

#if defined(HAVE_VARARG_MACROS)
#if !defined(PYWRAP)
#if !defined(STANDALONE)

#if defined(SIMICS_CORE) && !defined(SIMICS_API_USE_LOG_ALWAYS_HAP)
#define SIMICS_API_USE_LOG_ALWAYS_HAP 1
#endif

/* Helper function to avoid warnings from signed/unsigned comparisons */
#ifdef SIMICS_API_USE_LOG_ALWAYS_HAP
PURE_FUNCTION FORCE_INLINE bool
VT_log_level_enough(conf_object_t *obj, unsigned lvl)
{ return unlikely(VT_effective_log_level(obj) >= lvl); }
#else
PURE_FUNCTION FORCE_INLINE bool
VT_log_level_enough(conf_object_t *obj, unsigned lvl)
{ return unlikely(SIM_log_level(obj) >= lvl); }
#endif /* SIMICS_API_USE_LOG_ALWAYS_HAP */
#else /* STANDALONE */

PURE_FUNCTION FORCE_INLINE bool
VT_log_level_enough(conf_object_t *obj, unsigned lvl)
{ return true; }

#endif /* !STANDALONE */
#endif /* !PYWRAP */

#define SIM_LOG_ERROR(dev, grp, ...)                                          \
        SIM_log_message_vararg((dev), 1, (grp), Sim_Log_Error, __VA_ARGS__)

#define SIM_LOG_CRITICAL(dev, grp, ...)                                       \
        SIM_log_message_vararg((dev), 1, (grp), Sim_Log_Critical, __VA_ARGS__)

#define SIM_LOG_INFO(lvl, dev, grp, ...)                                \
do {                                                                    \
        if (unlikely(VT_log_level_enough((dev), (unsigned)(lvl))))      \
                SIM_log_message_vararg((dev), (lvl), (grp),             \
                                       Sim_Log_Info, __VA_ARGS__);      \
} while (0)

#define SIM_LOG_SPEC_VIOLATION(lvl, dev, grp, ...)                      \
do {                                                                    \
        if (unlikely(VT_log_level_enough((dev), (lvl))))                \
                SIM_log_message_vararg((dev), (lvl), (grp),             \
                                       Sim_Log_Spec_Violation,          \
                                       __VA_ARGS__);                    \
} while (0)

#define SIM_LOG_UNIMPLEMENTED(lvl, dev, grp, ...)                       \
do {                                                                    \
        if (unlikely(VT_log_level_enough((dev), (lvl))))                \
                SIM_log_message_vararg((dev), (lvl), (grp),             \
                                       Sim_Log_Unimplemented,           \
                                       __VA_ARGS__);                    \
} while (0)

#define SIM_LOG_WARNING(dev, grp, ...)                                        \
        SIM_log_message_vararg((dev), 1, (grp), Sim_Log_Warning, __VA_ARGS__)

/* The following macros will log once at level lvl1 and raise the level
   to lvl2 afterwards to avoid the potential log spams. */
#define SIM_LOG_INFO_ONCE(lvl1, lvl2, dev, grp, ...)                    \
do {                                                                    \
        static char _already_printed = 0;                               \
        if (_already_printed == 0)                                      \
                SIM_LOG_INFO((lvl1), (dev), (grp), __VA_ARGS__);        \
        else                                                            \
                SIM_LOG_INFO((lvl2), (dev), (grp), __VA_ARGS__);        \
        _already_printed = 1;                                           \
} while (0)

#define SIM_LOG_SPEC_VIOLATION_ONCE(lvl1, lvl2, dev, grp, ...)          \
do {                                                                    \
        static char _already_printed = 0;                               \
        if (_already_printed == 0)                                      \
                SIM_LOG_SPEC_VIOLATION((lvl1), (dev), (grp), __VA_ARGS__);\
        else                                                            \
                SIM_LOG_SPEC_VIOLATION((lvl2), (dev), (grp), __VA_ARGS__);\
        _already_printed = 1;                                           \
} while (0)

#define SIM_LOG_UNIMPLEMENTED_ONCE(lvl1, lvl2, dev, grp, ...)           \
do {                                                                    \
        static char _already_printed = 0;                               \
        if (_already_printed == 0)                                      \
                SIM_LOG_UNIMPLEMENTED((lvl1), (dev), (grp), __VA_ARGS__);\
        else                                                            \
                SIM_LOG_UNIMPLEMENTED((lvl2), (dev), (grp), __VA_ARGS__);\
        _already_printed = 1;                                           \
} while (0)

#define SIM_LOG_WARNING_ONCE(dev, grp, ...)                             \
do {                                                                    \
        static char _already_printed = 0;                               \
        if (_already_printed == 0)                                      \
                SIM_LOG_WARNING((dev), (grp), __VA_ARGS__);             \
        _already_printed = 1;                                           \
} while (0)

#else

#define SIM_LOG_ERROR VT_log_error
#define SIM_LOG_CRITICAL VT_log_critical
#define SIM_LOG_INFO VT_log_info
#define SIM_LOG_SPEC_VIOLATION VT_log_spec_violation
#define SIM_LOG_UNIMPLEMENTED VT_log_unimplemented
#define SIM_LOG_WARNING VT_log_warning

#endif /* HAVE_VARARG_MACROS */

#if defined(__cplusplus)
}
#endif

#endif
