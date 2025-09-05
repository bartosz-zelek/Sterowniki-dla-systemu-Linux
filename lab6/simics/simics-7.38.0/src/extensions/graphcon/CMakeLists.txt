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

# graphcon is distributed in Base, but depends on Hyperscan which may or may not
# be available to user. Disable this module by default when running CMake in a
# project created from Base package (not the repo). User can opt-in by copy
# module to project and modify current condition.
if(SIMICS_REPO_ROOT)
  set(CONSOLE_CLASS graphcon)
  set(WINSOME_CLASS gfx-frontend-winsome)
  simics_add_module(graphcon
    CLASSES ${CONSOLE_CLASS} ${WINSOME_CLASS}
    SOURCES gfx-console.c recorder.c gfx-break.c output.c vnc.c rect.c save.c winsome-frontend.c input.c module_load.py simics_start.py
    INIT_LOCAL
    SIMICS_API latest
  )
  # TODO(ah): MODULEDEPS had this dependency set, still required?
  add_dependencies(graphcon mini_winsome external-connection)
  target_link_libraries(graphcon
    PRIVATE console-common-graphcon crypto-engine::headers pthread
  )
  add_compile_definitions(GFX_CONSOLE=${CONSOLE_CLASS} WINSOME_FRONTEND=${WINSOME_CLASS} NOTIFY_OPEN=graphcon-open NOTIFY_CLOSE=graphcon-close SHOW_OBSOLETE_API )

  simics_add_external_build_dependency(graphcon Hyperscan)
  simics_add_external_build_dependency(graphcon PNG)

  if(WIN32)
    # TODO(ah): hard-code the path for now; should be part of build-deps
    set(x11-keysyms ${SIMICS_BUILD_DEPS_ROOT}/simics-pkgs-x11-keysymdef-include)
    target_include_directories(graphcon PRIVATE ${x11-keysyms})
  endif()
endif()
