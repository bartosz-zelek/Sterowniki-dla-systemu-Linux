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

# cmake-lint: disable=C0103,C0111

# See hyperscan.cmake for details
function(find_OpenSSL root)
  set(OPENSSL_ROOT_DIR ${root})
  find_package(OpenSSL REQUIRED)
  message(DEBUG "OpenSSL include: ${OPENSSL_INCLUDE_DIR}")
  message(DEBUG "OpenSSL crypto library: ${OPENSSL_CRYPTO_LIBRARY}")
  message(DEBUG "OpenSSL ssl library: ${OPENSSL_SSL_LIBRARY}")
  if(NOT (    ${root} STRLESS ${OPENSSL_INCLUDE_DIR}
          AND ${root} STRLESS ${OPENSSL_CRYPTO_LIBRARY}
          AND ${root} STRLESS ${OPENSSL_SSL_LIBRARY}))
    message(FATAL_ERROR "Located paths for OpenSSL are not all within ${root}")
  endif()
endfunction()

function(add_dependency_OpenSSL target required)
  if(required)
    find_package(OpenSSL REQUIRED)
  else()
    find_package(OpenSSL)
  endif()
  if(OpenSSL_FOUND)
    target_include_directories(${target} PRIVATE ${OPENSSL_INCLUDE_DIR})
    target_link_libraries(${target}
      PRIVATE ${OPENSSL_CRYPTO_LIBRARY})
  endif()
endfunction()

set(SIMICS_BUILD_DEPS_OPENSSL
  ${SIMICS_BUILD_DEPS_ROOT}/openssl-3.4.0
  CACHE STRING "Simics OpenSSL package's folder")

find_OpenSSL(${SIMICS_BUILD_DEPS_OPENSSL})
