# © 2021 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

# Locate Hyperscan in Simics build-deps.
# Based on FindEXPAT.cmake from CMake 3.19 and the CMake wiki

#[=======================================================================[.rst:
FindHyperscan
-------------

Find the Intel(R) Hyperscan Library

Imported Targets
^^^^^^^^^^^^^^^^

This module defines the following :prop_tgt:`IMPORTED` targets:

``Hyperscan::hs``
  The Hyperscan ``hs`` library, if found.

Result Variables
^^^^^^^^^^^^^^^^

This module will set the following variables in your project:

``HYPERSCAN_INCLUDE_DIRS``
  where to find hs/hs.h, etc.
``HYPERSCAN_LIBRARIES``
  the libraries to link against to use Hyperscan.
``HYPERSCAN_FOUND``
  true if the Hyperscan headers and libraries were found.

Hints
^^^^^

Set ``HYPERSCAN_ROOT`` to a Hyperscan installation root to tell this module
where to look.

#]=======================================================================]

# cmake-lint: disable=C0103

find_package(PkgConfig QUIET)

pkg_check_modules(PC_HYPERSCAN QUIET libhs)

find_path(HYPERSCAN_INCLUDE_DIR
  NAMES hs/hs.h
  HINTS ${HYPERSCAN_ROOT} ${PC_HYPERSCAN_INCLUDE_DIRS}
  PATH_SUFFIXES include
)

find_library(HYPERSCAN_LIBRARY
  NAMES hs
  HINTS ${HYPERSCAN_ROOT} ${PC_HYPERSCAN_LIBRARY_DIRS}
  PATH_SUFFIXES lib64 lib
)

if(HYPERSCAN_INCLUDE_DIR AND EXISTS "${HYPERSCAN_INCLUDE_DIR}/hs/hs.h")
  file(STRINGS "${HYPERSCAN_INCLUDE_DIR}/hs/hs.h" hs_version_str
    REGEX "^#[\t ]*define[\t ]+HS_(MAJOR|MINOR|PATCH)[\t ]+[0-9]+$")

  unset(HYPERSCAN_VERSION)
  foreach(VPART MAJOR MINOR PATCH)
    foreach(VLINE ${hs_version_str})
      if(VLINE MATCHES "^#[\t ]*define[\t ]+HS_${VPART}[\t ]+([0-9]+)$")
        set(HYPERSCAN_VERSION_PART "${CMAKE_MATCH_1}")
        if(HYPERSCAN_VERSION)
          string(APPEND HYPERSCAN_VERSION ".${HYPERSCAN_VERSION_PART}")
        else()
          set(HYPERSCAN_VERSION "${HYPERSCAN_VERSION_PART}")
        endif()
      endif()
    endforeach()
  endforeach()
endif()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Hyperscan
  REQUIRED_VARS HYPERSCAN_LIBRARY HYPERSCAN_INCLUDE_DIR
  VERSION_VAR HYPERSCAN_VERSION)

if(HYPERSCAN_FOUND)
  set(HYPERSCAN_INCLUDE_DIRS "${HYPERSCAN_INCLUDE_DIR}")
  set(HYPERSCAN_LIBRARIES ${HYPERSCAN_LIBRARY})

  if(NOT TARGET Hyperscan::hs)
    add_library(Hyperscan::hs UNKNOWN IMPORTED)
    set_target_properties(Hyperscan::hs PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
      IMPORTED_LOCATION "${HYPERSCAN_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${HYPERSCAN_INCLUDE_DIR}")
  endif()
endif()

mark_as_advanced(HYPERSCAN_INCLUDE_DIR HYPERSCAN_LIBRARY)
