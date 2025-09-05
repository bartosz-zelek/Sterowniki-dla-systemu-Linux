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

/* Header file to include instead of Python.h since it (actually pyconfig.h)
   defines some preprocessor symbols that collide with module-host-config.h. */

#ifndef SIMICS_PYTHON_HEADER_H
#define SIMICS_PYTHON_HEADER_H

#ifdef SIMICS_MODULE_HOST_CONFIG_H
 #error python-header.h must be included before any other Simics API headers
#endif

#ifdef Py_PYTHON_H
 #error python-header.h must be included before Python.h
#endif

#include <stdio.h>
#include <stddef.h>

/* pyconfig.h, included via Python.h, redefines the following symbols,
   so we need to make sure they are not defined before and not afterwards. */
#undef _POSIX_C_SOURCE
#undef LONG_BIT
#undef _XOPEN_SOURCE

#include <Python.h>

#undef _POSIX_C_SOURCE
#undef LONG_BIT
#undef _XOPEN_SOURCE

#define PYTHON_INCLUDED

#define SIM_PY_HASH_RET Py_ssize_t

#endif
