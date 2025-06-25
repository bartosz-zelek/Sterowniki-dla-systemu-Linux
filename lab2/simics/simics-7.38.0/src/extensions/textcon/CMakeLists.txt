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

# skip-cmake-conversion

# textcon is distributed in Base, but depends on Hyperscan which may or may not
# be available to user. Disable this module by default when running CMake in a
# project created from Base package (not the repo). User can opt-in by copy
# module to project and modify current condition.
if(SIMICS_REPO_ROOT)
  if(WIN32)
    set(pty pty-win32.c)
  else()
    set(pty pty-unix.c)
  endif()

  simics_add_module(textcon
    CLASSES textcon text-frontend-winsome
    SOURCES text-console.c driver.c recorder.c screen.c winsome-frontend.c vt100.c telnet.c host-serial.c module_load.py simics_start.py ${pty}
    INIT_LOCAL
    SIMICS_API latest
  )
  # TODO(ah): MODULEDEPS had this dependency set, still required?
  add_dependencies(textcon mini_winsome external-connection)
  target_link_libraries(textcon
    PRIVATE console-common-graphcon pthread
  )
  add_compile_definitions(NOTIFY_OPEN=textcon-open NOTIFY_CLOSE=textcon-close )

  simics_add_external_build_dependency(textcon Hyperscan)

  # Pre GCC8 the -O3 optimizations will trigger -Werror=strict-overflow.
  if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_C_COMPILER_VERSION VERSION_LESS "8.0.0")
      target_compile_options(textcon PRIVATE -Wno-strict-overflow)
    endif()
  endif()
endif()
