# © 2024 Intel Corporation
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
macro(find_Boost root)
  set(BOOST_ROOT ${root})
  # Boost prefers system paths over BOOST_ROOT; but we don't
  set(Boost_NO_SYSTEM_PATHS YES)
  find_package(Boost REQUIRED)
  message(DEBUG "Boost include: ${Boost_INCLUDE_DIRS}")
  message(DEBUG "Boost library dir: ${Boost_LIBRARY_DIRS}")
  if(NOT (    ${root} STRLESS ${Boost_INCLUDE_DIRS}
          AND ${root} STRLESS ${Boost_LIBRARY_DIRS}))
    message(FATAL_ERROR "Located paths for Boost are not all within ${root}")
  endif()
  # TODO(ah): for whatever reason; the Boost find_package mechanism does not
  #           honor that we have already found Boost here as part of top-level
  #           build-deps config. So we use our own cache mechanism for now
  #           based on Boost_FOUND and the dirs we need
  set(Boost_FOUND TRUE CACHE PATH "Simics: Boost found" FORCE)
  set(Boost_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} CACHE PATH
    "Simics: Boost include dir" FORCE)
  set(Boost_LIBRARY_DIRS ${Boost_LIBRARY_DIRS} CACHE PATH
    "Simics: Boost library dir" FORCE)
endmacro()

# TODO(ah): Boost is a bit special, as it contains a myriad of libs and not
#           just one; thus we should expand this feature to allow the user to
#           select which libs he needs instead of just getting the lib root
#           folder
macro(add_dependency_Boost target required)
  if(NOT Boost_FOUND)
    if(required)
      find_package(Boost REQUIRED)
    else()
      find_package(Boost)
    endif()
  endif()
  if(Boost_FOUND)
    target_include_directories(${target} SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
    if(Boost_LIBRARIES)
      target_link_libraries(${target} PRIVATE ${Boost_LIBRARIES})
    else()
      target_link_directories(${target} PRIVATE ${Boost_LIBRARY_DIRS})
    endif()
  endif()
endmacro()
