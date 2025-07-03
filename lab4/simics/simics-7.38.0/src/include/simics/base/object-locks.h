/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_BASE_OBJECT_LOCKS_H
#define SIMICS_BASE_OBJECT_LOCKS_H

#include <simics/base/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct domain_lock domain_lock_t;

EXPORTED domain_lock_t *SIM_acquire_object(
        conf_object_t *NOTNULL obj,
        const char *NOTNULL function_name,
        const char *NOTNULL source_location);

EXPORTED domain_lock_t *SIM_acquire_target(
        conf_object_t *NOTNULL obj,
        const char *NOTNULL function_name,
        const char *NOTNULL source_location);

EXPORTED domain_lock_t *SIM_acquire_cell(
        conf_object_t *NOTNULL obj,
        const char *NOTNULL function_name,
        const char *NOTNULL source_location);

EXPORTED void SIM_release_object(
        conf_object_t *NOTNULL obj,
        domain_lock_t *lock);

EXPORTED void SIM_release_target(
        conf_object_t *NOTNULL obj,
        domain_lock_t *lock);

EXPORTED void SIM_release_cell(
        conf_object_t *NOTNULL obj,
        domain_lock_t *lock);

EXPORTED domain_lock_t *SIM_acquire_object_for_execution(
        conf_object_t *NOTNULL obj);

EXPORTED void SIM_yield_thread_domains(void);

/* Don't provide VT_acquire_* and VT_release_* in Python. There are
   SIM_ versions plus Python needs additional handling, see, e.g.,
   CORE_acquire_object: */
#ifndef PYWRAP
EXPORTED domain_lock_t *VT_acquire_object(
        conf_object_t *NOTNULL obj,
        const char *NOTNULL function_name,
        const char *NOTNULL source_location);

EXPORTED domain_lock_t *VT_acquire_cell(
        conf_object_t *NOTNULL obj,
        const char *NOTNULL function_name,
        const char *NOTNULL source_location);

EXPORTED domain_lock_t *VT_acquire_target(
        conf_object_t *NOTNULL obj,
        const char *NOTNULL func,
        const char *NOTNULL location);

EXPORTED void VT_release_object(
        conf_object_t *NOTNULL obj,
        domain_lock_t *lock);

EXPORTED void VT_release_cell(
        conf_object_t *NOTNULL obj,
        domain_lock_t *lock);

EXPORTED void VT_release_target(
        conf_object_t *NOTNULL obj,
        domain_lock_t *lock);
#endif  /* PYWRAP */

#define OLOCK_SYM_TO_STR2_(x)     #x
#define OLOCK_SYM_TO_STR_(x)      OLOCK_SYM_TO_STR2_(x)
#define OLOCK_FILE_LINE_          __FILE__ ":" OLOCK_SYM_TO_STR_(__LINE__)

/* <append-macro id="SIM_acquire_object"/> */
#define SIM_ACQUIRE_OBJECT(obj, lockp) \
        *(lockp) = SIM_acquire_object((obj), __func__, OLOCK_FILE_LINE_)
/* <append-macro id="SIM_acquire_object"/> */
#define SIM_RELEASE_OBJECT(obj, lockp) \
        SIM_release_object((obj), *(lockp))

/* <append-macro id="SIM_acquire_cell"/> */
#define SIM_ACQUIRE_CELL(obj, lockp) \
        *(lockp) = SIM_acquire_cell((obj), __func__, OLOCK_FILE_LINE_)
/* <append-macro id="SIM_acquire_cell"/> */
#define SIM_RELEASE_CELL(obj, lockp) \
        SIM_release_cell((obj), *(lockp))

/* <append-macro id="SIM_acquire_target"/> */
#define SIM_ACQUIRE_TARGET(obj, lockp) \
        *(lockp) = SIM_acquire_target((obj), __func__, OLOCK_FILE_LINE_)
/* <append-macro id="SIM_acquire_target"/> */
#define SIM_RELEASE_TARGET(obj, lockp) \
        SIM_release_target((obj), *(lockp))

EXPORTED void VT_assert_object_lock(
        conf_object_t *NOTNULL obj,
        const char *NOTNULL func,
        const char *NOTNULL line);

EXPORTED void VT_assert_cell_context(
        conf_object_t *NOTNULL obj,
        const char *NOTNULL file,
        const char *NOTNULL func);

#if defined(_MSC_VER) && !defined(__func__)
#define __func__ __FUNCTION__
#endif

EXPORTED domain_lock_t *SIM_drop_thread_domains(void);
EXPORTED void SIM_reacquire_thread_domains(domain_lock_t *NOTNULL lock);

/* <append-macro id="SIM_drop_thread_domains"/> */
#define SIM_DROP_THREAD_DOMAINS(obj, lockp) \
        *(lockp) = SIM_drop_thread_domains((obj))
/* <append-macro id="SIM_drop_thread_domains"/> */
#define SIM_REACQUIRE_THREAD_DOMAINS(obj, lockp) \
        SIM_reacquire_thread_domains((obj), *(lockp))



/*
  <add-macro id="simics api object locks">
  <short>assert thread domain is held</short>

  The <fun>SIM_ASSERT_OBJECT_LOCK</fun> function checks that the
  thread domain associated with <arg>obj</arg> is held. A hard failure
  is raised if the thread domain is not held.

  </add-macro>
*/
#define SIM_ASSERT_OBJECT_LOCK(obj) \
        VT_assert_object_lock((obj), __func__, OLOCK_FILE_LINE_)


/*
  <add-macro id="simics api object locks">
  <short>assert Cell Context</short>

  The <fun>SIM_ASSERT_CELL_CONTEXT</fun> function verifies that cell
  thread domain associated with <arg>obj</arg> is held by the calling thread
  and raises a hard failure if this is not the case.

  In other worlds, the macro ensures that the execution context is either
  Cell Context or Global Context.
  </add-macro>
*/
#define SIM_ASSERT_CELL_CONTEXT(obj) \
        VT_assert_cell_context((obj), __func__, OLOCK_FILE_LINE_)


#if defined(__cplusplus)
}
#endif

#endif
