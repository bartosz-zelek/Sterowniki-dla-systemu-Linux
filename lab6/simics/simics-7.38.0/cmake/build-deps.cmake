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

# Sets up Simics build deps; i.e. locating external libraries in non-standard
# locations specific to Simics lab infrastructure
#
# To keep these in sync with the settings in config/env-linux64.mk; run
# the following script: update-local-build-deps.sh
#

# TODO(ah): we should be more careful about using ENV mark-ups and remove C0103
# cmake-lint: disable=C0111,C0103

function(simics_setup_build_deps_root)
  # The idea here is that the user (or CI) provides the
  # SIMICS_BUILD_DEPS_ROOT as an absolute path matching current build-deps
  # version (as defined by config/build-deps-version.mk). It could be a sync to
  # local disk (by CI) or a deployment of build-deps using Conan (by
  # user) or a preconfigured setup on NFS as is the case in the Stockholm
  # lab. If not set, checks SIMICS_BUILD_DEPS_BASE and then if still not set it
  # defaults to NFS.
  if(NOT SIMICS_BUILD_DEPS_ROOT)
    file(READ "config/build-deps-version.mk" build-deps-version)
    string(REGEX MATCH "BUILD_DEPS_VERSION = ([0-9].[0-9]*)" _
      ${build-deps-version})
    set(version ${CMAKE_MATCH_1})
    message(VERBOSE "Simics build-deps version: ${version}")
    if(SIMICS_BUILD_DEPS_BASE AND
        DEFINED ENV{SIMICS_BUILD_DEPS_BASE} AND
        NOT ${SIMICS_BUILD_DEPS_BASE} STREQUAL $ENV{SIMICS_BUILD_DEPS_BASE})
      message(FATAL_ERROR "SIMICS_BUILD_DEPS_BASE cache and env variables are not equal")
    endif()
    if(NOT SIMICS_BUILD_DEPS_BASE AND
       DEFINED ENV{SIMICS_BUILD_DEPS_BASE})
      set(SIMICS_BUILD_DEPS_BASE $ENV{SIMICS_BUILD_DEPS_BASE})
    endif()
    if(NOT SIMICS_BUILD_DEPS_BASE)
      if(WIN32)
        # Use locally deployed build-deps ; or require user to provide the path
        set(SIMICS_BUILD_DEPS_BASE d:/conan_build_deps)
      else()
        set(SIMICS_BUILD_DEPS_BASE /usr/itm/conan_build_deps)
      endif()
    endif()
    set(SIMICS_BUILD_DEPS_ROOT ${SIMICS_BUILD_DEPS_BASE}/${version}
      CACHE STRING "Simics build-deps folder (with version)")
  endif()

  if(NOT IS_DIRECTORY ${SIMICS_BUILD_DEPS_ROOT})
    message(FATAL_ERROR "Unable to find Simics build-deps folder:"
      " ${SIMICS_BUILD_DEPS_ROOT} \nPlease setup SIMICS_BUILD_DEPS_ROOT"
      " cache variable")
  endif()

  message(VERBOSE "Simics build-deps root: ${SIMICS_BUILD_DEPS_ROOT}")

  mark_as_advanced(SIMICS_BUILD_DEPS_ROOT SIMICS_BUILD_DEPS_BASE)
endfunction()

function(simics_setup_build_deps)
  # Specific package settings (used to seed find_package() calls)
  # NOTE: by calling find_package early, we cache the results and avoid
  # duplicated scans per module
  #
  # Use update-local-build-deps.sh to keep them in sync with repo config
  set(SIMICS_BUILD_DEPS_HYPERSCAN
    ${SIMICS_BUILD_DEPS_ROOT}/hyperscan-5.4.2
    CACHE STRING "Simics Hyperscan package's folder")
  find_Hyperscan(${SIMICS_BUILD_DEPS_HYPERSCAN})

  # Use latest version of Boost ; we'll handle the SystemC differently
  set(SIMICS_BUILD_DEPS_BOOST
    ${SIMICS_BUILD_DEPS_ROOT}/boost-1.83.0
    CACHE STRING "Simics Boost package's folder")
  find_Boost(${SIMICS_BUILD_DEPS_BOOST})

  if(WIN32)
    # TODO(ah): Due to the lack of Conan build-deps on Windows these folders
    # and versions differ in name and structure compared to linux variants
    # below.
    set(SIMICS_BUILD_DEPS_ZLIB
      ${SIMICS_BUILD_DEPS_ROOT}/zlib-1.3.1
      CACHE STRING "Simics ZLIB package's folder")
    set(ZLIB_INCLUDE_DIR
      ${SIMICS_BUILD_DEPS_ROOT}/zlib-1.3.1/include
      CACHE STRING "Simics ZLIB include")
    set(ZLIB_LIBRARY
      ${SIMICS_BUILD_DEPS_ROOT}/zlib-1.3.1/lib/zlib1.dll
      CACHE STRING "Simics ZLIB lib")
    find_ZLIB(${SIMICS_BUILD_DEPS_ZLIB})
    # PNG depends on ZLIB; thus load it after
    set(SIMICS_BUILD_DEPS_PNG
      ${SIMICS_BUILD_DEPS_ROOT}/libpng-1.6.40
      CACHE STRING "Simics PNG package's folder")
    find_PNG(${SIMICS_BUILD_DEPS_PNG})
  else()
    set(SIMICS_BUILD_DEPS_ZLIB
      ${SIMICS_BUILD_DEPS_ROOT}/zlib-1.3.1
      CACHE STRING "Simics ZLIB package's folder")
    find_ZLIB(${SIMICS_BUILD_DEPS_ZLIB})
    # PNG depends on ZLIB; thus load it after
    set(SIMICS_BUILD_DEPS_PNG
      ${SIMICS_BUILD_DEPS_ROOT}/libpng-1.6.40
      CACHE STRING "Simics PNG package's folder")
    find_PNG(${SIMICS_BUILD_DEPS_PNG})
  endif()

  # Linux-only settings
  if(NOT WIN32)
    set(VALGRIND_INCLUDE_DIR
      ${SIMICS_BUILD_DEPS_ROOT}/valgrind-3.15.0/include
      CACHE PATH "Valgrind include directory")
  endif()

  mark_as_advanced(
    SIMICS_BUILD_DEPS_HYPERSCAN
    SIMICS_BUILD_DEPS_OPENSSL
    SIMICS_BUILD_DEPS_BOOST
    SIMICS_BUILD_DEPS_ZLIB
    SIMICS_BUILD_DEPS_PNG
    VALGRIND_INCLUDE_DIR)
endfunction()
