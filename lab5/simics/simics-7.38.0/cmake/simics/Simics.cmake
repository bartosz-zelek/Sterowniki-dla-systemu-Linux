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

# See README.md for details on usage

cmake_minimum_required(VERSION 3.22)

# cmake-lint: disable=R0912,R0915,C0111,C0113

macro(simics_global_cmake_properties)
  # True for everything since we build shared library modules for Simics
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)

  # All Simics modules need the GNU extensions.
  set(CMAKE_C_EXTENSIONS ON)
  set(CMAKE_CXX_EXTENSIONS ON)
endmacro()

function(simics_project_setup)
  message(CHECK_START "Detecting Simics")
  message(VERBOSE "Using Simics.cmake package at: ${CMAKE_CURRENT_LIST_DIR}")

  # Host OS specifics
  if(WIN32)
    set(host_type win64)
    set(bat_suffix .bat)
  else()
    set(host_type linux64)
  endif()
  set(project_setup bin/project-setup${bat_suffix})

  # Detecting Simics Project location:
  # a) Provided by user, via SIMICS_PROJECT_DIR, i.e. created somewhere else
  # b) In the root of the ${CMAKE_SOURCE_DIR};
  #    e.g. Simics Base repo or a traditional Simics project
  # c) Not in (a) nor (b); create a new one in ${CMAKE_CURRENT_BINARY_DIR}
  #    using the Simics Base from which Simics.cmake was included OR pointed
  #    to by SIMICS_BASE_DIR variable
  if(SIMICS_PROJECT_DIR)
    # pass; no need to do anything more at this point
  elseif(EXISTS ${CMAKE_SOURCE_DIR}/.project-properties)
    set(SIMICS_PROJECT_DIR "${CMAKE_SOURCE_DIR}")
  else()
    set(SIMICS_PROJECT_DIR ${CMAKE_CURRENT_BINARY_DIR})
    if(SIMICS_BASE_DIR)
      set(simics_base ${SIMICS_BASE_DIR})
    else()
      get_filename_component(simics_base ${CMAKE_CURRENT_LIST_DIR} DIRECTORY)
      get_filename_component(simics_base ${simics_base} DIRECTORY)
    endif()
    if(NOT EXISTS ${simics_base}/${project_setup})
      message(FATAL_ERROR
        "Simics Base package path is not valid: ${simics_base}\n"
        "Either load Simics.cmake from Base or explicitly provide"
        " SIMICS_BASE_DIR.")
    endif()
    execute_process(
      COMMAND ${simics_base}/${project_setup} . --ignore-existing-files
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMAND_ECHO NONE OUTPUT_QUIET
      COMMAND_ERROR_IS_FATAL ANY)
  endif()
  set(SIMICS_PROJECT_DIR "${SIMICS_PROJECT_DIR}" CACHE PATH
    "Path to the associated Simics project" FORCE)
  message(STATUS "Found Simics project: ${SIMICS_PROJECT_DIR}")

  # Simics project settings
  set(SIMICS_EXECUTABLE ${SIMICS_PROJECT_DIR}/simics${bat_suffix} CACHE
    FILEPATH "Simics executable" FORCE)
  set(SIMICS_HOST_TYPE  ${host_type} CACHE INTERNAL "Simics host type")

  # Simics expects all modules to reside in the project lib dir
  if(DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(SIMICS_HOST_LIB_DIR ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} CACHE PATH
      "Simics project <host>/lib directory; where all modules resides" FORCE)
    message(VERBOSE "Modules will be emitted to: ${SIMICS_HOST_LIB_DIR}")
  else()
    set(SIMICS_HOST_LIB_DIR
      ${SIMICS_PROJECT_DIR}/${host_type}/lib CACHE PATH
      "Simics project <host>/lib directory; where all modules resides"
      FORCE)
    message(DEBUG "Modules will be emitted to: ${SIMICS_HOST_LIB_DIR}")
  endif()
  if(DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(SIMICS_HOST_BIN_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} CACHE PATH
      "Simics project <host>/bin directory; where binaries resides" FORCE)
    message(VERBOSE "Binaries will be emitted to: ${SIMICS_HOST_BIN_DIR}")
  else()
    set(SIMICS_HOST_BIN_DIR ${SIMICS_PROJECT_DIR}/${host_type}/bin
      CACHE PATH
      "Simics project <host>/bin directory; where binaries resides" FORCE)
    message(DEBUG "Binaries will be emitted to: ${SIMICS_HOST_BIN_DIR}")
  endif()
  define_property(GLOBAL PROPERTY SIMICS_HOST_LIB_DIR INHERITED
    BRIEF_DOCS "Where to create Simics module"
    FULL_DOCS  "Where to create Simics module")
  define_property(DIRECTORY PROPERTY SIMICS_HOST_LIB_DIR INHERITED
    BRIEF_DOCS "Where to create Simics module"
    FULL_DOCS  "Where to create Simics module")
  define_property(TARGET PROPERTY SIMICS_HOST_LIB_DIR INHERITED
    BRIEF_DOCS "Where to create Simics module"
    FULL_DOCS  "Where to create Simics module")
  set_property(GLOBAL PROPERTY SIMICS_HOST_LIB_DIR ${SIMICS_HOST_LIB_DIR})

  # Detect Simics Base package associated with current project
  # NOTE: we *do* want to run this on each configuration run to detect if the
  #       Simics project has been updated or not and we ignore the
  #       SIMICS_BASE_DIR passed into the configuration phase.
  execute_process(
    COMMAND ${SIMICS_PROJECT_DIR}/bin/project-paths${bat_suffix}
    OUTPUT_VARIABLE project_paths_json
    WORKING_DIRECTORY ${SIMICS_PROJECT_DIR})

  unset(SIMICS_BASE_DIR)
  string(JSON out GET ${project_paths_json} "simics-base")
  cmake_path(CONVERT "${out}"
    TO_CMAKE_PATH_LIST _simics_base_dir NORMALIZE)
  set(SIMICS_BASE_DIR "${_simics_base_dir}" CACHE PATH
    "Path to Simics Base package" FORCE)
  if(NOT EXISTS ${SIMICS_BASE_DIR})
    message(FATAL_ERROR
      "Simics Base package path is not valid: ${SIMICS_BASE_DIR}")
  endif()
  message(STATUS "Found Simics Base package: ${SIMICS_BASE_DIR}")

  string(JSON out GET ${project_paths_json} "simics-python")
  cmake_path(CONVERT "${out}"
    TO_CMAKE_PATH_LIST _simics_python_dir NORMALIZE)
  if(NOT EXISTS ${SIMICS_PYTHON_DIR})
    set(SIMICS_PYTHON_DIR "${_simics_python_dir}" CACHE PATH
      "Path to Simics Python package" FORCE)
  endif()
  if(NOT EXISTS ${SIMICS_PYTHON_DIR})
    message(FATAL_ERROR
      "Simics Python package path is not valid: ${SIMICS_PYTHON_DIR}")
  endif()
  message(STATUS "Found Simics Python package: ${SIMICS_PYTHON_DIR}")

  # TODO (henrik):
  # Because package 1033 does not look like a real Python installation we must
  # handle it as a special case. We *do* want to use it if provided, so we only
  # call find_package() to locate host python if 1033 is not provided. If we can
  # re-arrange 1033 package to look like a Python installation then we can always
  # call find_package() to locate both host python and 1033 alike.
  # If that is done, remove all Python_FOUND conditionals further down.
  if(NOT EXISTS ${SIMICS_PYTHON_DIR}/${host_type})
    set(Python_ROOT_DIR ${SIMICS_PYTHON_DIR})
    find_package(Python COMPONENTS Interpreter Development)
  endif()
  
  # Make sure configuration is re-run if project is updated
  set_directory_properties(PROPERTIES CMAKE_CONFIGURE_DEPENDS "${SIMICS_PROJECT_DIR}/.project-properties/project-paths;${SIMICS_PROJECT_DIR}/.package-list;${SIMICS_BASE_DIR}/.package-list")

  # Detecting Simics Base version
  file(STRINGS ${SIMICS_BASE_DIR}/packageinfo/Simics-Base-${host_type}
    version REGEX "^version: (.*)")
  # NOTE: we need CMake 3.29 to use ${CMAKE_MATCH_1} from file(...) ; so
  # until we bump that far we need to run another regexp
  if(version MATCHES "^version: (.*)")
    set(SIMICS_VERSION ${CMAKE_MATCH_1} CACHE INTERNAL
      "Simics (full) version")
    # NOTE: currently, the *CMake package* mirrors the same version as Simics
    # Base; but this could (and perhaps should) be a separate version
    set(Simics_VERSION ${SIMICS_VERSION} CACHE INTERNAL
      "Simics CMake package version")
  else()
    message(FATAL_ERROR "Unable to parse packageinfo of Simics Base package")
  endif()
  message(STATUS "Found Simics Base version: ${SIMICS_VERSION}")

  # Set Simics version related parameters
  string(REPLACE "." ";" version_list ${SIMICS_VERSION})
  list(GET version_list 0 SIMICS_VERSION_MAJOR)
  list(GET version_list 1 SIMICS_VERSION_MINOR)
  list(GET version_list 2 SIMICS_VERSION_PATCH)
  set(SIMICS_VERSION_MAJOR "${SIMICS_VERSION_MAJOR}" CACHE INTERNAL
    "Simics major version")
  set(SIMICS_VERSION_MINOR "${SIMICS_VERSION_MINOR}" CACHE INTERNAL
    "Simics minor version")
  set(SIMICS_VERSION_PATCH "${SIMICS_VERSION_PATCH}" CACHE INTERNAL
    "Simics patch version")
  set(SIMICS_API "${SIMICS_VERSION_MAJOR}" CACHE STRING
    "Default Simics API used if not set by module")

  message(CHECK_PASS "done")

  # Project configuration
  set(SIMICS_BUILD_ID "" CACHE STRING
    "Build-id value compiled into the module")
  if(SIMICS_BUILD_ID)
    message(VERBOSE "Simics build-id: ${SIMICS_BUILD_ID}")
    if(NOT ${SIMICS_BUILD_ID} MATCHES "^[-a-zA-Z0-9_.]+:[0-9]+$")
      message(FATAL_ERROR
        "Invalid format: build-id must be 'namespace:number'")
    endif()
    if("simics:" STRLESS ${SIMICS_BUILD_ID} AND NOT SIMICS_REPO_ROOT)
      message(FATAL_ERROR
        "Invalid namespace: 'simics' is a reserved namespace")
    endif()
  endif()

  # Setup custom find_package(<module>) wrappings used by Simics Base
  if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/find-package.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/find-package.cmake)
    simics_setup_custom_find_package()
  endif()

  # Simics script paths
  set(SIMICS_PYTHON_EXECUTABLE
    ${SIMICS_PROJECT_DIR}/bin/mini-python${bat_suffix} CACHE FILEPATH
    "Simics Python executable" FORCE)
  set(SIMICS_MODULE_ID_PY
    ${SIMICS_BASE_DIR}/scripts/build/module_id.py CACHE FILEPATH
    "Simics build variable" FORCE)
  set(SIMICS_COPY_DEVICE_XML_PY
    ${SIMICS_BASE_DIR}/scripts/build/copy_device_xml.py CACHE FILEPATH
    "Simics build variable" FORCE)
  set(SIMICS_COPY_PYTHON_PY
    ${SIMICS_BASE_DIR}/scripts/copy_python.py CACHE FILEPATH
    "Simics build variable" FORCE)
  set(SIMICS_ANALYZE_TRAMPOLINES_PY
    ${SIMICS_BASE_DIR}/scripts/build/analyze-trampolines.py CACHE FILEPATH
    "Simics build variable" FORCE)

  # Conditional scripts that are part of doc-and-packaging #6007 package (or
  # Base repo)
  if(NOT DEFINED SIMICS_EXTRACT_LIMITATIONS_PY)
    message(CHECK_START "Looking for Doc-and-Packaging")
    _simics_load_packages()
    set(SIMICS_EXTRACT_LIMITATIONS_PY
      SIMICS_EXTRACT_LIMITATIONS_PY-NOTFOUND)
    foreach(package ${packages})
      if(EXISTS ${package}/scripts/dmlx2html/extract-device-limitations.py)
        set(SIMICS_EXTRACT_LIMITATIONS_PY
          ${package}/scripts/dmlx2html/extract-device-limitations.py)
        break()
      endif()
    endforeach()
    set(SIMICS_EXTRACT_LIMITATIONS_PY ${SIMICS_EXTRACT_LIMITATIONS_PY}
      CACHE FILEPATH "Simics build variable" FORCE)
    if(SIMICS_EXTRACT_LIMITATIONS_PY)
      message(CHECK_PASS "found")
    else()
      message(CHECK_FAIL "not found")
    endif()
  endif()

  # Simics library
  set(SIMICS_INC ${SIMICS_BASE_DIR}/src/include CACHE PATH
    "Simics build variable" FORCE)
  set(SIMICS_LIB ${SIMICS_BASE_DIR}/${host_type}/bin CACHE PATH
    "Simics build variable" FORCE)
  set(SIMICS_SYS_LIB ${SIMICS_BASE_DIR}/${host_type}/sys/lib CACHE PATH
    "Simics build variable" FORCE)
  define_property(GLOBAL PROPERTY SIMICS_API INHERITED
    BRIEF_DOCS "API used by Simics module"
    FULL_DOCS "API used by Simics module")
  define_property(DIRECTORY PROPERTY SIMICS_API INHERITED
    BRIEF_DOCS "API used by Simics module"
    FULL_DOCS "API used by Simics module")
  define_property(TARGET PROPERTY SIMICS_API INHERITED
    BRIEF_DOCS "API used by Simics module"
    FULL_DOCS "API used by Simics module")
  set_property(GLOBAL PROPERTY SIMICS_API ${SIMICS_API})

  add_library(Simics::Simics INTERFACE IMPORTED GLOBAL)
  target_include_directories(Simics::Simics INTERFACE ${SIMICS_INC})
  target_link_libraries(Simics::Simics INTERFACE vtutils simics-common)
  target_compile_definitions(Simics::Simics
    INTERFACE -DSIMICS_$<TARGET_PROPERTY:SIMICS_API>_API)
  target_link_directories(Simics::Simics INTERFACE ${SIMICS_LIB})
  add_library(Simics::includes INTERFACE IMPORTED GLOBAL)
  target_include_directories(Simics::includes INTERFACE ${SIMICS_INC})

  if(WIN32)
    target_link_libraries(Simics::Simics INTERFACE ssp)
  endif()

  # Simics DML
  # TODO(ah): if DMLC_DIR and DMLC_PYTHON is *not* customized, they are
  # expected to automatically update to point to the corresponding base
  # package. This used to be possible by adding FORCE here, but we cannot do
  # that anymore as that would prevent the user from customizing these two
  # variables. Perhaps there is a way to do both?
  set(SIMICS_DMLC_DIR ${SIMICS_BASE_DIR}/${host_type}/bin
    CACHE FILEPATH "Simics DML compiler directory")
  set(SIMICS_DMLC_PYTHON ${SIMICS_PYTHON_EXECUTABLE} CACHE FILEPATH
    "Python interpreter for DMLC")
  set(SIMICS_DMLC ${SIMICS_DMLC_PYTHON} -X utf8 ${SIMICS_DMLC_DIR}/dml/python
    CACHE STRING "Simics DML compiler invocation" FORCE)
  message(STATUS "Invoking DMLC as: ${SIMICS_DMLC}")
  define_property(GLOBAL PROPERTY SIMICS_DMLC_FLAGS
    BRIEF_DOCS "Flags for DML compiler" FULL_DOCS "Flags for DML compiler")
  define_property(DIRECTORY PROPERTY SIMICS_DMLC_FLAGS INHERITED
    BRIEF_DOCS "Flags for DML compiler" FULL_DOCS "Flags for DML compiler")
  define_property(TARGET PROPERTY SIMICS_DMLC_FLAGS INHERITED
    BRIEF_DOCS "Flags for DML compiler" FULL_DOCS "Flags for DML compiler")
  define_property(SOURCE PROPERTY SIMICS_DMLC_FLAGS INHERITED
    BRIEF_DOCS "Flags for DML compiler" FULL_DOCS "Flags for DML compiler")
  add_library(Simics::DML INTERFACE IMPORTED GLOBAL)
  target_include_directories(Simics::DML INTERFACE
    ${SIMICS_DMLC_DIR}/dml/include  # for C
    ${SIMICS_DMLC_DIR}/dml          # for DML
    ${SIMICS_DMLC_DIR}/dml/api/$<TARGET_PROPERTY:SIMICS_API>)
  set(SIMICS_GEN_DML_COMMANDS_JSON_PY
    ${SIMICS_BASE_DIR}/scripts/build/gen_dml_compile_commands.py
    CACHE FILEPATH "Simics DML commands json generator" FORCE)

  # For DML language server; we need to store away some metadata
  define_property(GLOBAL PROPERTY SIMICS_DML_MODULES
    BRIEF_DOCS "DML modules/targets"
    FULL_DOCS "DML modules/targets")
  define_property(TARGET PROPERTY SIMICS_DML_SOURCES
    BRIEF_DOCS "DML module's source files"
    FULL_DOCS "DML module's source files")

  # Custom target for Simics module list introspection
  add_custom_target(simics-modules)
  add_custom_target(list-simics-modules
    COMMENT "List Simics modules"
    COMMAND ${CMAKE_COMMAND} -E
      echo $<TARGET_PROPERTY:simics-modules,MANUALLY_ADDED_DEPENDENCIES>
    COMMAND_EXPAND_LISTS)

  # C++ API

  # This property is used by simics-cc-api to setup the include directory for
  # the generated API headers
  define_property(TARGET PROPERTY SIMICS_GENERATED_CC_ROOT INHERITED
    BRIEF_DOCS "Root of generated C++ API files"
    FULL_DOCS "Path to Base where auto-generated C++ headers is located")
  set_property(GLOBAL PROPERTY SIMICS_GENERATED_CC_ROOT
    ${SIMICS_BASE_DIR}/${host_type}/api)

  # In the case where we do *not* include the c++-api module automatically,
  # we need to explicitly add it here in order for Simics::C++ target to work
  add_subdirectory(${SIMICS_BASE_DIR}/src/devices/c++-api simics-cc-api
    EXCLUDE_FROM_ALL)

  # Python related
  # Simics Python settings
  if(Python_FOUND)
    set(SIMICS_PYTHON_INC
      ${Python_INCLUDE_DIRS} CACHE PATH
      "Simics build variable" FORCE)
  else()
    set(SIMICS_PYTHON_INC
      ${SIMICS_PYTHON_DIR}/${host_type}/include/python3.10 CACHE PATH
      "Simics build variable" FORCE)
  endif()  
  set(SIMICS_PYWRAP_DIR ${SIMICS_BASE_DIR}/${host_type}/bin/pywrap
    CACHE PATH "Simics build variable" FORCE)
  if (WIN32)
    if(Python_FOUND)
      set(SIMICS_PYTHON_LIB ${Python_RUNTIME_LIBRARY_DIRS}
        CACHE PATH "Simics Python build variable" FORCE)
      add_library(Simics::Python SHARED IMPORTED GLOBAL)
      set_target_properties(Simics::Python PROPERTIES
        IMPORTED_LOCATION ${Python_LIBRARIES}
        IMPORTED_IMPLIB ${Python_SABI_LIBRARIES}
      )
    else()
      set(SIMICS_PYTHON_LIB ${SIMICS_PYTHON_DIR}/${host_type}/bin
        CACHE PATH "Simics Python build variable" FORCE)
      add_library(Simics::Python SHARED IMPORTED GLOBAL)
      set_target_properties(Simics::Python PROPERTIES
        IMPORTED_LOCATION ${SIMICS_PYTHON_LIB}/py3/python3${CMAKE_SHARED_LIBRARY_SUFFIX}
        IMPORTED_IMPLIB ${SIMICS_PYTHON_LIB}/py3/python3.lib
      )
    endif()
  else()
    if(Python_FOUND)
      set(SIMICS_PYTHON_LIB ${Python_RUNTIME_LIBRARY_DIRS}
        CACHE PATH "Simics Python build variable" FORCE)
    else()
      set(SIMICS_PYTHON_LIB ${SIMICS_PYTHON_DIR}/${host_type}/sys/lib
        CACHE PATH "Simics Python build variable" FORCE)
    endif()
    add_library(Simics::Python INTERFACE IMPORTED GLOBAL)
  endif()
  target_include_directories(Simics::Python INTERFACE ${SIMICS_PYTHON_INC})
  target_link_directories(Simics::Python INTERFACE ${SIMICS_PYTHON_LIB})
  target_compile_definitions(Simics::Python
    INTERFACE -DPy_LIMITED_API=0x030A0000 -DPY_MAJOR_VERSION=3)
  target_compile_options(Simics::Python
    INTERFACE -Wno-write-strings -Wno-undef)

  # In order to toggle the THREAD_SAFE flag, use this property;
  # defaults to FALSE
  define_property(DIRECTORY PROPERTY SIMICS_MODULE_NOT_THREAD_SAFE INHERITED
    BRIEF_DOCS "NOT_THREAD_SAFE; boolean"
    FULL_DOCS "NOT_THREAD_SAFE; boolean")

  # Find location of libasan used by kit
  # gcc -print-file-name=<library> will always return 0, even if no library
  # is found. It also will always return some text. In the case the the
  # library is found, it will return the full path. Otherwise it will return
  # <library>\n...
  if (CMAKE_SYSTEM_NAME MATCHES Linux)
    execute_process(
      COMMAND ${CMAKE_C_COMPILER} -print-file-name=libasan.so
      OUTPUT_VARIABLE LIBASAN_PATH
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    set(lsan_supp_out ${CMAKE_BINARY_DIR}/simics-lsan.supp)
    file(REMOVE ${lsan_supp_out})
    file(READ ${SIMICS_BASE_DIR}/scripts/simics-lsan.supp suppressions)
    file(APPEND ${lsan_supp_out} "${suppressions}")
    if (LSAN_SUPPRESSION_FILE)
      file(READ ${LSAN_SUPPRESSION_FILE} suppressions)
      file(APPEND ${lsan_supp_out} "${suppressions}")
    endif()

    get_filename_component(SAN_DIR ${LIBASAN_PATH} DIRECTORY)
    if(EXISTS ${LIBASAN_PATH})
      # NOTE: ASAN only works on Linux; so cache variables can be INTERNAL
      set(LIBASAN_PATH ${LIBASAN_PATH} CACHE INTERNAL "Path to libasan")
      set(SAN_DIR ${SAN_DIR} CACHE INTERNAL "Directory of sanitizers")
      set(SANITIZERS_SUPPORTED ON CACHE INTERNAL
        "Sanitizer flags supported")
      set(LSAN_OPTIONS
        "print_suppressions=false:suppressions=${lsan_supp_out}"
        CACHE INTERNAL "LSAN runtime options")
      set(ASAN_OPTIONS "")
      if(ASAN_STACK_USE_AFTER_RETURN)
        set(USE_ASAN ON CACHE BOOL
          "Compile modules marked with simics_add_sanitizers with ASAN"
          FORCE)
        string(APPEND ASAN_OPTIONS "detect_stack_use_after_return=1")
      endif()
      # Every additional SAN option append should start with ":"
      set(malloc_context_size 15)
      if(LSAN_MALLOC_CONTEXT_SIZE)
        set(malloc_context_size ${LSAN_MALLOC_CONTEXT_SIZE})
      endif()
      string(APPEND ASAN_OPTIONS
        ":fast_unwind_on_malloc=0:malloc_context_size=${malloc_context_size}")
      set(ASAN_OPTIONS ${ASAN_OPTIONS} CACHE INTERNAL
        "ASAN runtime options")

      set(asan_env
        "export ASAN_OPTIONS=${ASAN_OPTIONS} "
        "export LSAN_OPTIONS=${LSAN_OPTIONS} "
        "export LD_PRELOAD=${LIBASAN_PATH}:$ENV{LD_PRELOAD}")
      add_custom_target(simics-asan
        COMMAND bash -c "${asan_env} && ${SIMICS_EXECUTABLE} -e 'cd ${SIMICS_PROJECT_DIR}'"
        USES_TERMINAL
        VERBATIM)
    endif()
  endif()

  mark_as_advanced(ASAN_OPTIONS LIBASAN_PATH SANITIZERS_SUPPORTED SAN_DIR
    SIMICS_BASE_DIR SIMICS_ANALYZE_TRAMPOLINES_PY SIMICS_COPY_DEVICE_XML_PY
    SIMICS_COPY_PYTHON_PY SIMICS_DMLC SIMICS_EXECUTABLE
    SIMICS_EXTRACT_LIMITATIONS_PY SIMICS_GEN_DML_COMMANDS_JSON_PY
    SIMICS_HOST_LIB_DIR SIMICS_HOST_TYPE SIMICS_INC SIMICS_LIB
    SIMICS_MODULE_ID_PY SIMICS_PROJECT_DIR SIMICS_PYTHON_EXECUTABLE
    SIMICS_PYTHON_INC SIMICS_PYWRAP_DIR SIMICS_SYS_LIB Simics_VERSION
    SIMICS_VERSION SIMICS_VERSION_MAJOR SIMICS_VERSION_MINOR
    SIMICS_VERSION_PATCH)
endfunction()

macro(_simics_load_packages)
  # Locate and parse .package-list
  #
  # Affected variables
  #  packages - list of absolute paths to Base and add-on packages
  set(_package_list "")
  if(EXISTS ${SIMICS_PROJECT_DIR}/.package-list)
    message(VERBOSE "Using .package-list from local project")
    set(_package_list ${SIMICS_PROJECT_DIR}/.package-list)
  elseif(EXISTS ${SIMICS_BASE_DIR}/.package-list)
    message(VERBOSE "Using .package-list from Simics Base installation")
    set(_package_list ${SIMICS_BASE_DIR}/.package-list)
  endif()
  set(_packages ${SIMICS_BASE_DIR})
  # On Windows, file() does not handle empty file as input
  if(EXISTS ${_package_list})
    file(STRINGS "${_package_list}" addon_package_paths)
    foreach(line ${addon_package_paths})
      if(IS_ABSOLUTE ${line})
        list(APPEND _packages ${line})
      else()
        list(APPEND _packages ${SIMICS_BASE_DIR}/${line})
      endif()
    endforeach()
  endif()
  # Windows-ism; need to convert path to cmake /-slash before passed to GLOB
  cmake_path(CONVERT "${_packages}" TO_CMAKE_PATH_LIST packages NORMALIZE)
endmacro()

# By default, we search in both the current project, Simics Base and all add-on
# packages; but the user can opt-out of this and only search the project's
# modules/* by passing IN_PROJECT or only Base and the add-on packages by
# passing IN_PACKAGES.
function(simics_find_and_add_modules)
  cmake_parse_arguments(GLOB "IN_PROJECT;IN_PACKAGES" "" "" ${ARGN})

  if(NOT GLOB_IN_PROJECT AND NOT GLOB_IN_PACKAGES)
    set(GLOB_IN_PROJECT ON)
    set(GLOB_IN_PACKAGES ON)
  endif()

  if(GLOB_IN_PROJECT)
    # First we glob in the local Simics project assuming modules/ subdir *and*
    # generic symlinks to arbitrary repos
    #
    # NOTE: globbing for */CMakeList.txt is tempting but what if the user has
    #       its own top-level CMake logic in the root? Assuming modules are
    #       always in some subdirectory below the root is the safer bet. If
    #       this is not the case, the top-level is expected to add what's
    #       needed.
    # NOTE: for the same reasons it's not safe to do recursive globbing
    set(local_modules "")
    file(GLOB module_configs CONFIGURE_DEPENDS "*/*/CMakeLists.txt")
    foreach(config ${module_configs})
      get_filename_component(module ${config} DIRECTORY)
      add_subdirectory(${module})
      get_filename_component(_module_name ${module} NAME)
      list(APPEND local_modules ${_module_name})
    endforeach()
    list(SORT local_modules)
    message(VERBOSE "Modules found in project: ${local_modules}")
  endif()

  # Early exit if user does not want to glob all packages
  if(NOT GLOB_IN_PACKAGES)
    return()
  endif()

  # Then we glob all modules in Base + add-on packages
  # NOTE: we must exclude the local modules or their targets will clash
  _simics_load_packages()
  set(package_modules "")
  foreach(package ${packages})
    set(subdirs "components" "devices" "extensions")
    foreach(subdir ${subdirs})
      file(GLOB build_files CONFIGURE_DEPENDS
        "${package}/src/${subdir}/*/CMakeLists.txt")
      foreach(build_file ${build_files})
        get_filename_component(build_directory ${build_file} DIRECTORY)
        get_filename_component(_module_name ${build_directory} NAME)
        if(NOT ${_module_name} IN_LIST package_modules)
          if(NOT ${_module_name} IN_LIST local_modules)
            add_subdirectory(${build_directory} pkgs/${_module_name}
              EXCLUDE_FROM_ALL)
            list(APPEND package_modules ${_module_name})
          else()
            message(TRACE
              "Skipping module found in local project: ${_module_name}")
          endif()
        else()
          message(TRACE
            "Skipping module already added: ${_module_name}")
        endif()
      endforeach()
    endforeach()
  endforeach()
  list(SORT package_modules)
  message(DEBUG "Modules found in packages: ${package_modules}")
endfunction()

# Keep for backwards compatibility
function(simics_find_and_load_modules)
  if(SIMICS_GLOB_ONLY_PROJECT)
    simics_find_and_add_modules(IN_PROJECT)
  else()
    simics_find_and_add_modules()
  endif()
endfunction()

function(generate_dml_compile_commands_json)
  if(NOT CMAKE_EXPORT_COMPILE_COMMANDS)
    return()
  endif()
  message(CHECK_START "Updating dml_compile_commands.json")

  set(dml_compile_metadata ${CMAKE_BINARY_DIR}/dml_compile_metadata.txt)
  file(REMOVE ${dml_compile_metadata})

  get_property(propval GLOBAL PROPERTY SIMICS_DML_MODULES)
  foreach(module ${propval})
    get_property(dmlc_flags_set TARGET ${module} PROPERTY SIMICS_DMLC_FLAGS SET)

    # Add default flags (without --) ; these are explicitly added by
    # _generate_simics_dml_module_files using generator expressions so we
    # cannot just query a property here.
    set(dmlc_flags_filtered)
    get_target_property(api ${module} SIMICS_API)
    list(APPEND dmlc_flags_filtered simics-api=${api})
    list(APPEND dmlc_flags_filtered info)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
      list(APPEND dmlc_flags_filtered g)
    endif()
    # strip '--' and '-' from dmlc_flags so options can be passed to python and
    # not be interpreted as parameters. Quoting all of it did not work.
    if(dmlc_flags_set)
      get_target_property(dmlc_flags ${module} SIMICS_DMLC_FLAGS)
      foreach(flag ${dmlc_flags})
        string(REGEX REPLACE "(^--|^-)" "" out ${flag})
        list(APPEND dmlc_flags_filtered ${out})
      endforeach()
    endif()

    # Add include dirs
    get_target_property(include_dirs ${module} INCLUDE_DIRECTORIES)
    get_target_property(tgts ${module} LINK_LIBRARIES)
    foreach(tgt ${tgts})
      if(TARGET ${tgt})
        get_target_property(link_dirs ${tgt} INTERFACE_INCLUDE_DIRECTORIES)
        list(APPEND include_dirs ${link_dirs})
      endif()
    endforeach()
    # Expand the API version in generator expressions
    set(include_dirs_expanded)
    foreach(entry ${include_dirs})
      string(REPLACE "$<TARGET_PROPERTY:SIMICS_API>" "${api}" out ${entry})
      list(APPEND include_dirs_expanded ${out})
    endforeach()

    get_target_property(source_dir ${module} SOURCE_DIR)
    get_target_property(sources ${module} SIMICS_DML_SOURCES)
    foreach(source ${sources})
      # Handle per-dml source file property here and append if present
      get_source_file_property(source_file_dmlc_flags ${source_dir}/${source}
        TARGET_DIRECTORY ${module} SIMICS_DMLC_FLAGS)
      set(source_dmlc_flags_filtered)
      if(source_file_dmlc_flags)
        foreach(flag ${source_file_dmlc_flags})
          string(REGEX REPLACE "(^--|^-)" "" out ${flag})
          list(APPEND source_dmlc_flags_filtered ${out})
        endforeach()
      endif()

      # Need to combine the two lists here or they will not expand properly
      set(dmlc_flags)
      list(APPEND dmlc_flags ${dmlc_flags_filtered})
      list(APPEND dmlc_flags ${source_dmlc_flags_filtered})

      file(APPEND ${dml_compile_metadata}
        "${source_dir}/${source}\n${dmlc_flags}\n${include_dirs_expanded}\n")
    endforeach()
  endforeach()

  execute_process(
    COMMAND ${SIMICS_PYTHON_EXECUTABLE} ${SIMICS_GEN_DML_COMMANDS_JSON_PY}
      ${CMAKE_BINARY_DIR}/dml_compile_commands.json
      ${dml_compile_metadata})
  message(CHECK_PASS "done")
endfunction()

function(_add_c_preprocessor_command)
  # Add custom command to run C preprocessor.
  #
  # Arguments
  #   OUTPUT          output file
  #   SOURCE          input file
  #   TARGET          CMake target to inherit compile definitions, include
  #                   directories, and compile options
  #   EXTRA_C_FLAGS   extra compiler flags added after all flags inherited
  #                   from the TARGET

  set(one_value_args TARGET SOURCE OUTPUT)
  set(multi_value_args EXTRA_C_FLAGS)
  cmake_parse_arguments(CPP "" "${one_value_args}"
    "${multi_value_args}" ${ARGN})

  string(TOUPPER "${CMAKE_BUILD_TYPE}" build_type)
  string(REPLACE " " ";" c_flags
    "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_${build_type}}")

  add_custom_command(
    OUTPUT ${CPP_OUTPUT}
    COMMAND ${CMAKE_C_COMPILER}
      "-D$<JOIN:$<TARGET_PROPERTY:${CPP_TARGET},COMPILE_DEFINITIONS>,;-D>"
      "-I$<JOIN:$<TARGET_PROPERTY:${CPP_TARGET},INCLUDE_DIRECTORIES>,;-I>"
      ${c_flags}
      $<TARGET_PROPERTY:${CPP_TARGET},COMPILE_OPTIONS>
      ${CPP_EXTRA_C_FLAGS}
      -E ${CPP_SOURCE} -o ${CPP_OUTPUT}
    COMMAND_EXPAND_LISTS VERBATIM
    IMPLICIT_DEPENDS C ${CPP_SOURCE}
    DEPENDS ${CPP_SOURCE})
endfunction()

macro(_handle_mapped_parent_dir potential_symlink expected_parent out_list_name)
  string(CONCAT python_cmd "from pathlib import Path\;"
    "p=Path('" ${potential_symlink} "')\;"
    "print(p.is_symlink())")
  execute_process(COMMAND ${SIMICS_PYTHON_EXECUTABLE} -c ${python_cmd}
    OUTPUT_VARIABLE file_is_link
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(${file_is_link} MATCHES "True")
    string(CONCAT python_cmd "from pathlib import Path\;"
      "p=Path('" ${potential_symlink} "')\;"
      "print(p.readlink().parent.as_posix())")
    execute_process(COMMAND ${SIMICS_PYTHON_EXECUTABLE} -c ${python_cmd}
      OUTPUT_VARIABLE parent_of_resolved_symlink
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REPLACE "//?/" "" parent_of_resolved_symlink
      ${parent_of_resolved_symlink})
    if (NOT ${parent_of_resolved_symlink} MATCHES ${expected_parent})
      list(APPEND ${out_list_name} -W ${parent_of_resolved_symlink})
    endif()
  endif()
endmacro()

macro(_generate_simics_interface_module_files)
  # Inputs
  #   MODULE_NAME
  #   MODULE_PYNAME
  #   MODULE_SOURCES
  #   MODULE_PYWRAP
  #
  # Affects variables
  #   MODULE_SOURCES          generated Python wrappers and other code
  #   MODULE_PYWRAP           updated with .h files found in _SOURCES

  function(_remove_coverage_compile_options lib)
    get_target_property(_compile_flags ${lib} COMPILE_OPTIONS)
    list(REMOVE_ITEM _compile_flags "--coverage")
    list(REMOVE_ITEM _compile_flags "-fprofile-arcs")
    list(REMOVE_ITEM _compile_flags "-ftest-coverage")
    set_target_properties(${lib} PROPERTIES
      COMPILE_OPTIONS "${_compile_flags}")
  endfunction()

  function(_pywrap_file file)
    get_filename_component(dir_of_file ${file} DIRECTORY)
    if("${dir_of_file}" STREQUAL "")
      set(dir_of_file ${CMAKE_CURRENT_SOURCE_DIR})
    endif()
    get_filename_component(file ${file} NAME)
    string(CONCAT full_file_path ${dir_of_file} "/" ${file})

    string(REPLACE ".h" "" stem ${file})
    string(REPLACE "-" "_" file_pyname ${stem})
    set(typemaps_t pyifaces-${file}.t)
    set(typemaps_c pyifaces-${file}.c)
    set(typemaps_i pyifaces-${file}.i)

    # Generate the typemaps .t and .c files
    set(simics_typemaps ${SIMICS_PYWRAP_DIR}/py-typemaps.c)
    if(WIN32)
      _handle_mapped_parent_dir(${simics_typemaps} ${SIMICS_PYWRAP_DIR}
        ADDITIONAL_PYWRAP_ARGS)
    endif()

    add_custom_command(
      OUTPUT ${typemaps_t} ${typemaps_c}
      COMMAND ${CMAKE_COMMAND}
        -DPYWRAP_TYPEMAPS_C=${typemaps_c}
        -DPYWRAP_TYPEMAPS_T=${typemaps_t}
        -DTYPEMAPS=${simics_typemaps}
        -DFILE=${file}
        -DMODULE_SOURCE_DIR=${dir_of_file}
        -P ${Simics_DIR}/pyiface_typemap.cmake
      DEPENDS ${simics_typemaps} ${full_file_path})

    # Compile the typemap .i file
    _add_c_preprocessor_command(
      OUTPUT ${typemaps_i}
      SOURCE ${typemaps_c}
      TARGET ${MODULE_NAME}
      EXTRA_C_FLAGS -DPYWRAP)

    # Generate python trampolines and wrappers sources
    set(trampolines pyiface-${file}-trampolines.c)
    set(wrappers pyiface-${file}-wrappers.c)
    if(WIN32)
      _handle_mapped_parent_dir(${full_file_path} ${dir_of_file}
        ADDITIONAL_PYWRAP_ARGS)
    endif()
    add_custom_command(
      OUTPUT ${trampolines} ${wrappers}
      COMMAND ${CMAKE_COMMAND}
        -E env LD_LIBRARY_PATH=${SIMICS_LIB}:${SIMICS_SYS_LIB}
        ${SIMICS_LIB}/pywrapgen
        -W ${SIMICS_PYWRAP_DIR} -W . -W ${dir_of_file}
        ${ADDITIONAL_PYWRAP_ARGS}
        -n simmod.${MODULE_PYNAME}.${file_pyname}
        -t ${typemaps_t} ${typemaps_i}
        -o pyiface-${file}
      DEPENDS ${typemaps_t} ${typemaps_i})

    # Define/build trampolines lib
    set(trampolines_lib ${MODULE_PYNAME}_${file}_trampolines)
    add_library(${trampolines_lib} OBJECT ${trampolines})
    set_target_properties(${trampolines_lib} PROPERTIES
      POSITION_INDEPENDENT_CODE ON
      SIMICS_API $<TARGET_PROPERTY:${MODULE_NAME},SIMICS_API>)
    target_include_directories(${trampolines_lib}
      PRIVATE $<TARGET_PROPERTY:${MODULE_NAME},INCLUDE_DIRECTORIES>)
    target_compile_definitions(${trampolines_lib}
      PRIVATE $<TARGET_PROPERTY:${MODULE_NAME},COMPILE_DEFINITIONS>)
    # Trampolines *must* always be compiled with optimization, or the logic
    # that pythonwraps the interfaces will fail. Also disable stack
    # protection explicitly in case it has been enabled. Must disable ICF
    # (Identical Code Folding) for trampolines to work on GCC >= 5.0
    # target_compile_options turned to PUBLIC to make them accessible by
    # _remove_coverage_compile_options
    target_compile_options(${trampolines_lib}
      PUBLIC -O2 -fno-stack-protector -fdisable-ipa-icf)
    target_link_libraries(${trampolines_lib}
      PRIVATE Simics::Python Simics::Simics)
    _remove_coverage_compile_options(${trampolines_lib})

    # Generate trampolines data
    set(trampolines_data pyiface-${file}-trampoline-data.h)
    add_custom_command(
      OUTPUT ${trampolines_data}
      COMMAND ${CMAKE_OBJDUMP} -dw $<TARGET_OBJECTS:${trampolines_lib}>
        > $<TARGET_OBJECTS:${trampolines_lib}>d
      COMMAND ${SIMICS_PYTHON_EXECUTABLE} ${SIMICS_ANALYZE_TRAMPOLINES_PY}
        ${SIMICS_HOST_TYPE} < $<TARGET_OBJECTS:${trampolines_lib}>d
        > ${trampolines_data}
        DEPENDS ${trampolines_lib})

    # Define/build wrappers lib
    set(wrappers_lib ${MODULE_PYNAME}_${file}_wrappers)
    add_library(${wrappers_lib} OBJECT
      ${wrappers} ${trampolines_data})
    set_target_properties(${wrappers_lib} PROPERTIES
      POSITION_INDEPENDENT_CODE ON
      SIMICS_API $<TARGET_PROPERTY:${MODULE_NAME},SIMICS_API>)
    target_include_directories(${wrappers_lib}
      PRIVATE $<TARGET_PROPERTY:${MODULE_NAME},INCLUDE_DIRECTORIES>)
    target_compile_definitions(${wrappers_lib}
      PRIVATE $<TARGET_PROPERTY:${MODULE_NAME},COMPILE_DEFINITIONS>)
    # target_compile_options turned to PUBLIC to make them accessible by
    # _remove_coverage_compile_options
    target_compile_options(${wrappers_lib}
      PUBLIC -Wno-vla -Wno-missing-field-initializers)
    target_link_libraries(${wrappers_lib}
      PRIVATE Simics::Python Simics::Simics)
    _remove_coverage_compile_options(${wrappers_lib})

    # return results
    list(APPEND MODULE_SOURCES $<TARGET_OBJECTS:${trampolines_lib}>
      $<TARGET_OBJECTS:${wrappers_lib}>)
    set(MODULE_SOURCES ${MODULE_SOURCES} PARENT_SCOPE)
    list(APPEND MODULE_LIBRARIES $<TARGET_LIBRARIES:Simics::Python>)
    set(MODULE_LIBRARIES Simics::Python PARENT_SCOPE)
  endfunction()

  # Explicit PYWRAP is still needed to wrap types and functions
  set(not_just_headers ${MODULE_PYWRAP})
  list(FILTER not_just_headers EXCLUDE REGEX ".*\.h$")
  if(not_just_headers)
    message(FATAL_ERROR "PYWRAP must be *.h")
  endif()

  # Extract all .h files from SOURCES that contains things to python wrap
  set(headers ${MODULE_SOURCES})
  list(FILTER headers INCLUDE REGEX ".*\.h$")
  foreach(src ${headers})
    # Skip non-existing files; the _pywrap_file assumes only local files
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${src})
      file(STRINGS ${src} things_to_wrap
        REGEX ".*(SIM_INTERFACE|SIM_PY_ALLOCATABLE|SIM_CUSTOM_ATOM).*")
      if(things_to_wrap)
        if(NOT ${src} IN_LIST MODULE_PYWRAP)
          list(APPEND MODULE_PYWRAP ${src})
        endif()
      endif()
    endif()
  endforeach()

  # NOTE: we need to wrap all PYWRAP headers individually to meet current
  #       Simics requirements and implementation. Existing python code
  #       depends on this
  foreach(file ${MODULE_PYWRAP})
    _pywrap_file(${file})
  endforeach()

  # Need to set/update MODULE_PYWRAP in parent scope; where it's used to
  # generate flags for module_id.py. Perhaps we should use properties instead
  # of "global" variables.
  set(MODULE_PYWRAP ${MODULE_PYWRAP} PARENT_SCOPE)
endmacro()

macro(_generate_simics_dml_module_files)
  # Inputs
  #   MODULE_DML_SOURCES
  #
  # Affected variables
  #   MODULE_SOURCES      generated C-file
  #   MODULE_DML_DEVS     list of prefixes for module id
  #   MDOULE_DML_XMLS     list of .xml files generated by DMLC
  #   MODULE_LIBRARIES    link the module library with Simics::DML
  #
  set_property(GLOBAL APPEND PROPERTY SIMICS_DML_MODULES ${MODULE_NAME})
  foreach(source ${MODULE_DML_SOURCES})
    if(IS_ABSOLUTE ${source})
      message(FATAL_ERROR "DML files must have path relative to the"
        " source or binary directory: ${source}")
    endif()
    if(source MATCHES "\\.\\.")
      message(WARNING "Avoid .. in DML file path, the result can be"
        " unpredictable: ${source}")
    endif()

    get_filename_component(basename ${source} NAME_WE)
    set(dml_dev ${basename}-dml)

    set(dml_depfile ${dml_dev}.d)
    set(dml_c ${dml_dev}.c)
    set(dml_xml ${dml_dev}.xml)
    set(dml_h ${dml_dev}.h)
    set(dml_protos ${dml_dev}-protos.c)
    set(dml_struct ${dml_dev}-struct.h)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${source})
      set(dml_source ${CMAKE_CURRENT_SOURCE_DIR}/${source})
    else()
      set(dml_source ${CMAKE_CURRENT_BINARY_DIR}/${source})
    endif()

    # Only top-level .dml files are to be used; warn about adding other
    # files to SOURCES
    if(EXISTS ${dml_source})
      file(STRINGS ${dml_source} bad_file REGEX "^device.*;[ ]*$")
      if(NOT bad_file)
        message(FATAL_ERROR
          "Missing 'device' statement in file: ${source}")
      endif()
    endif()

    # Since there is no generator expression for source file properties; we
    # extract it here and pass it by value. But this also means that the
    # property must be set *before* invoking simics_add_module.
    get_source_file_property(source_file_dmlc_flags ${source}
      SIMICS_DMLC_FLAGS)
    if(NOT source_file_dmlc_flags)
      unset(source_file_dmlc_flags)
    endif()

    # NOTE: On Windows you cannot invoke .bat files from .bat files ; so we
    # need to split this command in two: one for the deps and one for the C
    # source generation.
    add_custom_command(
      OUTPUT ${dml_depfile}
      COMMAND ${SIMICS_DMLC}
        --simics-api=$<TARGET_PROPERTY:${MODULE_NAME},SIMICS_API> --info
        $<$<CONFIG:Debug>:-g>
        $<TARGET_PROPERTY:${MODULE_NAME},SIMICS_DMLC_FLAGS>
        ${source_file_dmlc_flags}
        "-I$<JOIN:$<TARGET_PROPERTY:${MODULE_NAME},INCLUDE_DIRECTORIES>,;-I>"
        ${dml_source} ${dml_dev}
        --dep ${dml_depfile} --dep-target ${dml_c} --no-dep-phony
      COMMAND_EXPAND_LISTS
      DEPENDS ${dml_source})
    add_custom_command(
      OUTPUT ${dml_c} ${dml_xml}
      BYPRODUCTS ${dml_h} ${dml_protos} ${dml_struct}
      # NOTE: if you change things here, also change
      #       generate_dml_compile_commands_json below
      COMMAND ${SIMICS_DMLC}
        --simics-api=$<TARGET_PROPERTY:${MODULE_NAME},SIMICS_API> --info
        $<$<CONFIG:Debug>:-g>
        $<TARGET_PROPERTY:${MODULE_NAME},SIMICS_DMLC_FLAGS>
        ${source_file_dmlc_flags}
        "-I$<JOIN:$<TARGET_PROPERTY:${MODULE_NAME},INCLUDE_DIRECTORIES>,;-I>"
        ${dml_source} ${dml_dev}
      COMMAND_EXPAND_LISTS
      DEPFILE ${dml_depfile}
      DEPENDS ${dml_source} ${dml_depfile})

    list(APPEND MODULE_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/${dml_c})
    list(APPEND MODULE_DML_DEVS ${dml_dev})
    list(APPEND MODULE_DML_XMLS ${dml_xml})

    set(dml_limitations ${dml_dev}-limitations.docu)
    if(SIMICS_EXTRACT_LIMITATIONS_PY)
      add_custom_command(
        OUTPUT ${dml_limitations}
        COMMAND ${SIMICS_PYTHON_EXECUTABLE}
          ${SIMICS_EXTRACT_LIMITATIONS_PY}
          <${dml_xml} >${dml_limitations}
        COMMAND_EXPAND_LISTS
        DEPENDS ${dml_xml})
      list(APPEND MODULE_SOURCES
        ${CMAKE_CURRENT_BINARY_DIR}/${dml_limitations})
      list(APPEND MODULE_DML_LIMITATIONS ${dml_limitations})
    endif()
  endforeach()
  list(APPEND MODULE_LIBRARIES Simics::DML)
endmacro()

macro(_generate_simics_module_id)
  # Inputs
  #   MODULE_NAME
  #   MODULE_CLASSES
  #   MODULE_COMPONENTS
  #   MODULE_INTERFACES
  #   MODULE_PYWRAP
  #   MODULE_INIT_LOCAL
  #
  # Affects variables
  #   MODULE_SOURCES              generated source file with module id
  #
  set(module_id_c ${MODULE_NAME}-module-id.c)

  if(MODULE_INIT_LOCAL)
    set(user_init_local --user-init-local)
  endif()
  if(MODULE_INTERFACES)
    set(list_filename
      ${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}-py-iface-list)
    string(JOIN "\n" ifaces ${MODULE_INTERFACES})
    file(WRITE ${list_filename} ${ifaces})
    set(py_iface_list --py-iface-list ${list_filename})
  endif()
  if(MODULE_PYWRAP)
    set(iface_py_module "")
    foreach(header ${MODULE_PYWRAP})
      get_filename_component(header ${header} NAME)
      string(REPLACE ".h" "" stem ${header})
      string(REPLACE "-" "_" module ${stem})
      list(APPEND iface_py_module --iface-py-module ${module})
    endforeach()
    # Required to be compatible with Simics Base
    list(APPEND iface_py_module --py-ver 3)
  endif()
  if(MODULE_DML_DEVS)
    set(dml_dev "")
    foreach(dml-dev ${MODULE_DML_DEVS})
      list(APPEND dml_dev --dml-dev ${dml-dev})
    endforeach()
  endif()
  if(SIMICS_BUILD_ID)
    set(build_id --build-id ${SIMICS_BUILD_ID})
  endif()
  set(thread_safe_flag "yes")
  get_directory_property(not_thread_safe SIMICS_MODULE_NOT_THREAD_SAFE)
  if(not_thread_safe)
    set(thread_safe_flag "no")
  endif()
  add_custom_command(
    OUTPUT ${module_id_c}
    COMMAND ${SIMICS_PYTHON_EXECUTABLE} ${SIMICS_MODULE_ID_PY}
      --c-module-id
      --output ${module_id_c}
      --module-name ${MODULE_NAME}
      --classes "'${MODULE_CLASSES};'"
      --components "'${MODULE_COMPONENTS};'"
      --host-type ${SIMICS_HOST_TYPE}
      --thread-safe ${thread_safe_flag}
      ${iface_py_module}
      ${py_iface_list}
      ${user_init_local}
      ${dml_dev}
      ${build_id}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)

  list(APPEND MODULE_SOURCES ${module_id_c})
endmacro()

macro(_add_library)
  add_library(${MODULE_NAME} MODULE ${MODULE_SOURCES})
  target_compile_definitions(${MODULE_NAME}
    PRIVATE -DHAVE_MODULE_DATE)
  target_include_directories(${MODULE_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
  target_link_libraries(${MODULE_NAME}
    PRIVATE Simics::Simics ${MODULE_LIBRARIES})
  get_target_property(lib_dir ${MODULE_NAME} SIMICS_HOST_LIB_DIR)
  set_target_properties(${MODULE_NAME} PROPERTIES
    PREFIX ""
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    LIBRARY_OUTPUT_DIRECTORY ${lib_dir})
  if(WIN32)
    # On Windows we _must_ export the symbols needed by Simics
    target_link_options(${MODULE_NAME}
      PRIVATE ${SIMICS_BASE_DIR}/config/project/exportmap.def)
  else()
    # Hide all symbols not needed by Simics; and add GCC-specific build-id
    target_link_options(${MODULE_NAME}
      PRIVATE -Wl,--version-script,${SIMICS_BASE_DIR}/config/project/exportmap.elf,--build-id)
  endif()
  if(MODULE_SIMICS_API)
    set_target_properties(${MODULE_NAME}
      PROPERTIES SIMICS_API ${MODULE_SIMICS_API})
  endif()

  # If .xml files were generated, copy to lib
  if(MODULE_DML_XMLS)
    add_custom_command(
      TARGET ${MODULE_NAME} POST_BUILD
      COMMAND ${SIMICS_PYTHON_EXECUTABLE} ${SIMICS_COPY_DEVICE_XML_PY}
        --copy-to=${lib_dir} ${MODULE_DML_XMLS})
  endif()
  if(MODULE_DML_LIMITATIONS)
    add_custom_command(
      TARGET ${MODULE_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E cat
        ${MODULE_DML_LIMITATIONS}
        >${lib_dir}/${MODULE_NAME}-limitations.docu)
  endif()

  # Auto-add a target for the include files
  add_library(${MODULE_NAME}-includes INTERFACE)
  target_include_directories(${MODULE_NAME}-includes INTERFACE .)
  add_library(${MODULE_NAME}::headers ALIAS ${MODULE_NAME}-includes)
  add_library(${MODULE_NAME}::imports ALIAS ${MODULE_NAME}-includes)
  add_library(${MODULE_NAME}::includes ALIAS ${MODULE_NAME}-includes)
  # If we build the modules that we depend on, which in general would be
  # interface modules, we can immediately run the unit tests as we would have
  # the python wrappings of those interfaces. This is the only reason why we
  # add a dependency here. But in some cases, such as wrapping CMake in GNU
  # Make, you don't want to build all deps. Thus user can opt-out of this.
  if(SIMICS_AUTO_ADD_DEPENDENCIES_ON_MODULE_WITH_HEADERS)
    add_dependencies(${MODULE_NAME}-includes ${MODULE_NAME})
  endif()
endmacro()

macro(_install_simics_python_file source)
  # Arguments
  #   source             .py file to install into the module's simmod folder
  #
  # Affects variables
  #   MODULE_ARTIFACTS   .pyc file is appended to this variable, building up
  #                      a list of module artifacts to depend on
  #
  if(IS_ABSOLUTE "${source}")
    message(FATAL_ERROR "Python files must have path relative to the source"
      " or binary directory: ${source}")
  endif()
  if(source MATCHES "\\.\\.")
    message(FATAL_ERROR "Use of .. in Python file path (${source}) is not"
      " supported; use `simics_copy_python_files()` to"
      " copy files.")
  endif()

  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${source})
    set(py_source ${CMAKE_CURRENT_SOURCE_DIR}/${source})
  else()
    set(py_source ${CMAKE_CURRENT_BINARY_DIR}/${source})
  endif()

  get_target_property(lib_dir ${MODULE_NAME} SIMICS_HOST_LIB_DIR)
  set(py_install_dir ${lib_dir}/python-py3/simmod/${MODULE_PYNAME})
  set(target ${py_install_dir}/${source})

  add_custom_command(
    OUTPUT ${target}
    COMMAND ${SIMICS_PYTHON_EXECUTABLE} ${SIMICS_COPY_PYTHON_PY}
      --compile ${py_source} ${target}
    DEPENDS ${py_source})

  list(APPEND MODULE_ARTIFACTS ${target})
endmacro()

macro(_install_simics_python_module)
  foreach(source ${MODULE_PY_SOURCES})
    _install_simics_python_file(${source})
  endforeach()

  add_custom_target(${MODULE_NAME}-artifacts DEPENDS ${MODULE_ARTIFACTS})
  add_dependencies(${MODULE_NAME} ${MODULE_NAME}-artifacts)
  _update_additional_clean_files_property_for_simmod(${MODULE_NAME})
endmacro()

macro(_filter_sources filter_exclude_var filter_include_var regex)
  set(${filter_include_var} ${${filter_exclude_var}})
  list(FILTER ${filter_exclude_var} EXCLUDE REGEX ${regex})
  list(FILTER ${filter_include_var} INCLUDE REGEX ${regex})
endmacro()

macro(_parse_simics_module_arguments)
  set(one_value_args SIMICS_API)
  set(multi_value_args CLASSES COMPONENTS INTERFACES SOURCES
    PYWRAP IFACE_FILES)
  set(options INIT_LOCAL)
  cmake_parse_arguments(MODULE "${options}" "${one_value_args}"
    "${multi_value_args}" ${ARGN})

  if(MODULE_UNPARSED_ARGUMENTS)
    string(REPLACE ";" " " _unparsed_args "${MODULE_UNPARSED_ARGUMENTS}")
    message(FATAL_ERROR
      "Found unparsed arguments for Simics module"
      " ${MODULE_NAME}: ${_unparsed_args}")
  endif()

  if(MODULE_IFACE_FILES)
    message(DEPRECATION "IFACE_FILES have been replaced by PYWRAP")
    set(MODULE_PYWRAP ${MODULE_IFACE_FILES})
  endif()

  # split C++, Python, DML, and other sources
  if(MODULE_SOURCES)
    _filter_sources(MODULE_SOURCES MODULE_PY_SOURCES ".*\.py$")
  endif()
  if(MODULE_SOURCES)
    _filter_sources(MODULE_SOURCES MODULE_DML_SOURCES ".*\.dml$")
  endif()

  # Transform module name used for the python wrapping. This is exactly what
  # Simics' module.mk does to support existing module names
  string(REPLACE "-" "_" MODULE_PYNAME ${MODULE_NAME})
  string(REPLACE "+" "__" MODULE_PYNAME ${MODULE_PYNAME})

  if(MODULE_SIMICS_API STREQUAL latest)
    set(MODULE_SIMICS_API ${SIMICS_API})
  endif()
  set(MODULE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endmacro()

function(simics_add_module name)
  set(MODULE_NAME ${name})
  _parse_simics_module_arguments(${ARGN})

  if(MODULE_INTERFACES OR MODULE_PYWRAP OR MODULE_SOURCES)
    _generate_simics_interface_module_files()
  endif()
  if(MODULE_DML_SOURCES)
    _generate_simics_dml_module_files()
  endif()

  _generate_simics_module_id()

  # The _add_library call must be after the _generate macros; as they are
  # building up the MODULE_SOURCES variable used to invoke add_library(...)
  # within the _add_library macro.
  _add_library()

  # Setting properties must be done *after* add_library has been invoked
  set_target_properties(${MODULE_NAME} PROPERTIES
    SIMICS_MODULE_CLASSES "${MODULE_CLASSES}")
  if(MODULE_DML_SOURCES)
    set_target_properties(${MODULE_NAME} PROPERTIES
      SIMICS_DML_SOURCES "${MODULE_DML_SOURCES}")
  endif()

  # Optionally sign the module (for external users in 6 only)
  if(SIMICS_SIGN_MODULES)
    _sign_module()
  endif()

  # Copy/install the module's python files into the simmod dir
  _install_simics_python_module()

  # Convenient target to build it all
  add_dependencies(simics-modules ${MODULE_NAME})
endfunction()

function(_update_additional_clean_files_property_for_simmod module)
  set(MODULE_NAME ${module})
  string(REPLACE "-" "_" MODULE_PYNAME ${MODULE_NAME})
  string(REPLACE "+" "__" MODULE_PYNAME ${MODULE_PYNAME})

  get_target_property(lib_dir ${MODULE_NAME} SIMICS_HOST_LIB_DIR)
  set(py_install_dir ${lib_dir}/python-py3/simmod/${MODULE_PYNAME})
  set_target_properties(${MODULE_NAME} PROPERTIES
    ADDITIONAL_CLEAN_FILES
      ${SIMICS_HOST_LIB_DIR}/python-py3/simmod/${MODULE_PYNAME})
endfunction()

function(simics_copy_python_files module)
  # Copy files from another module or CMake target
  #
  # Arguments
  #   module          The main target; the Simics module affected by the copy
  #   FROM            The other target, from where to copy the files
  #   FILES           multi-value argument listing all the files to copy
  #
  set(MODULE_NAME ${module})
  cmake_parse_arguments(MODULE "" "FROM" "FILES" ${ARGN})

  # Transform module name used for the python wrapping. This is exactly what
  # Simics' module.mk does to support existing module names
  string(REPLACE "-" "_" MODULE_PYNAME ${MODULE_NAME})
  string(REPLACE "+" "__" MODULE_PYNAME ${MODULE_PYNAME})

  get_target_property(lib_dir ${MODULE_NAME} SIMICS_HOST_LIB_DIR)
  set(py_install_dir ${lib_dir}/python-py3/simmod/${MODULE_PYNAME})
  set(MODULE_ARTIFACTS)
  foreach(src ${MODULE_FILES})
    set(dst ${py_install_dir}/${src})
    add_custom_command(
      OUTPUT ${dst}
      COMMAND ${SIMICS_PYTHON_EXECUTABLE} ${SIMICS_COPY_PYTHON_PY}
        --compile $<TARGET_PROPERTY:${MODULE_FROM},SOURCE_DIR>/${src}
        ${dst}
      COMMAND_EXPAND_LISTS
      DEPENDS $<TARGET_PROPERTY:${MODULE_FROM},SOURCE_DIR>/${src})
    list(APPEND MODULE_ARTIFACTS ${dst})
  endforeach()

  # We need another custom target to make things work; as we cannot run this
  # code from within simics_add_module without polluting the arguments
  add_custom_target(${MODULE_NAME}-copy-python-files ALL DEPENDS
    ${MODULE_ARTIFACTS})
  add_dependencies(${MODULE_NAME} ${MODULE_NAME}-copy-python-files)
  _update_additional_clean_files_property_for_simmod(${MODULE_NAME})
endfunction()

function(simics_add_sanitizers module_name)
  set(MODULE_NAME ${module_name})
  if(USE_ASAN OR USE_UBSAN)
    if(WIN32)
      message(FATAL_ERROR
        "Sanitizers are only supported on linux64 hosts")
    endif()
    if(NOT SANITIZERS_SUPPORTED)
      message(FATAL_ERROR "Sanitizers are not supported by your compiler")
    endif()
    target_compile_options(${MODULE_NAME} PRIVATE -fno-omit-frame-pointer)
    # Need to add this to runtime library search path for when compiler
    # outside of PATH is used
    target_link_options(${MODULE_NAME} PRIVATE -fno-omit-frame-pointer
      -Wl,-rpath,${SAN_DIR})
  endif()
  if(USE_ASAN)
    target_compile_options(${MODULE_NAME} PRIVATE -fsanitize=address)
    target_link_options(${MODULE_NAME} PRIVATE -fsanitize=address)
  endif()
  if(USE_UBSAN)
    target_compile_options(${MODULE_NAME} PRIVATE -fsanitize=undefined)
    target_link_options(${MODULE_NAME} PRIVATE -fsanitize=undefined)
  endif()
endfunction()

function(simics_add_dead_methods_check target)
  cmake_parse_arguments(DEAD_CODE "" "" EXTRA_SOURCES ${ARGN})

  if(USE_DML_DEAD_METHODS_CHECK)
    add_custom_command(
      TARGET ${target}
      POST_BUILD
      COMMAND ${CMAKE_COMMAND}
        -E echo "Checking if module ${target} contains dead DML methods"
      COMMAND ${SIMICS_PYTHON_EXECUTABLE}
        ${SIMICS_BASE_DIR}/scripts/build/dml_dead_methods.py
        -B ${SIMICS_BASE_DIR}
        -b $<TARGET_PROPERTY:${target},BINARY_DIR>
        -s $<TARGET_PROPERTY:${target},SOURCE_DIR>
        ${DEAD_CODE_EXTRA_SOURCES})
  endif()
endfunction()


function(simics_add_test test)
  # Adds a Simics s-*.py test to CMake; so it can be run by CTest
  #
  # If not provided by the `NAMESPACE` argument, the namespace will be
  # constructed by the current directory's parent's directory name; which
  # should correspond to the module name. Please note that Simics CMake allows
  # you to have multiple modules defined within the same module folder, and
  # thus ${MODULE_NAME} cannot be used.
  #
  # To enable CTest the top-level CMakeLists.txt must `include(CTest)` as
  # described by https://cmake.org/cmake/help/latest/module/CTest.html
  #
  # The building of test binaries can and should be conditional to
  # `BUILD_TESTING` which defaults to ON by the standard CTest module.
  #
  # Arguments:
  #   test      The name of the test; i.e. s-${test}.py
  #             - OR -
  #             The path to the s-test.py file (then name will be extracted)
  #   CWD       Can be used to specify the current-working-directory ; for
  #             example if the tests are located in a shared location other
  #             than the current source directory. Defaults to
  #             `CMAKE_CURRENT_SOURCE_DIR`.
  #   SYS_PATH  Can be set to a list of paths to be added to sys.path before the
  #             test runs. These are sent to Simics using the -e argument.
  #   ENV       Can be set to a list of VAR=value pairs used to set environment
  #             variables. These are set as test parameters using CMake/CTest.
  #   NAMESPACE Assign the namespace explicitly. Useful if the tests are
  #             located in a different structure than what is assumed by
  #             Simics; or if multiple tests should share the same namespace.
  #   NAME      Give the test a new name (namespace will still be added).
  #             Useful if you glob files from shared folder and they end up
  #             having the same names.
  #   PYTEST    Runs the test through `pytest` instead of Simics.
  #             Assumes `pytest` can be found in `PATH`, or the path to pytest
  #             must be provided through the `PYTEST_BINARY` cache variable.
  #   LABELS    A list of labels that is directly passed on to the LABELS
  #             property of the created test via set_tests_properties
  cmake_parse_arguments(TEST
    "PYTEST"
    "CWD;NAMESPACE;NAME"
    "SYS_PATH;ENV;LABELS" ${ARGN})

  if(TEST_CWD)
    set(cwd ${TEST_CWD})
  else()
    set(cwd ${CMAKE_CURRENT_SOURCE_DIR})
  endif()

  if(EXISTS ${cwd}/${test} OR
      (IS_ABSOLUTE ${test} AND EXISTS ${test}))
    set(test_file ${test})
    cmake_path(GET test FILENAME file)
    if(${file} MATCHES "^s-.*.py")
      string(REGEX REPLACE "^s-" "" t ${file})
      string(REPLACE ".py" "" test ${t})
    endif()
    if(${file} MATCHES "^s-.*.simics")
      string(REGEX REPLACE "^s-" "" t ${file})
      string(REPLACE ".simics" "" test ${t})
    endif()
  elseif(EXISTS ${cwd}/s-${test}.py)
    set(test_file "s-${test}.py")
  elseif(EXISTS ${cwd}/s-${test}.simics)
    set(test_file "s-${test}.simics")
  else()
    message(FATAL_ERROR "Could not locate test: ${test}")
  endif()

  if(TEST_NAMESPACE)
    set(namespace ${TEST_NAMESPACE})
  else()
    cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH _module)
    cmake_path(GET _module FILENAME namespace)
  endif()
  if(TEST_NAME)
    set(testname ${namespace}::${TEST_NAME})
  else()
    set(testname ${namespace}::${test})
  endif()

  ## Invocation (with or without sys.path)
  if(TEST_PYTEST)
    if(NOT PYTEST_BINARY)
      set(PYTEST_BINARY pytest CACHE INTERNAL "Path to the `pytest` launcher")
    endif()
    add_test(NAME ${testname}
      COMMAND ${PYTEST_BINARY} ${test}
      WORKING_DIRECTORY ${cwd})
  elseif(TEST_SYS_PATH)
    foreach(path ${TEST_SYS_PATH})
      # NOTE: we utilize that lists in CMake automatically add ';'
      list(APPEND sys_path "sys.path.append(\'${path}\')")
    endforeach()
    add_test(NAME ${testname}
      COMMAND ${SIMICS_EXECUTABLE} --batch-mode -e "@${sys_path}" ${test_file}
      WORKING_DIRECTORY ${cwd})
  else()
    add_test(NAME ${testname}
      COMMAND ${SIMICS_EXECUTABLE} --batch-mode ${test_file}
      WORKING_DIRECTORY ${cwd})
  endif()

  # The DEF_SOURCE_LINE property is used by the VSCode CMake Tools extension
  # to locate test files. However, custom properties are not shown by the
  # `ctest --show-only=json-v1` (which is used by the CMake Tools extension)
  # output until cmake v3.30.0
  set_tests_properties(${testname} PROPERTIES DEF_SOURCE_LINE "${cwd}/${test_file}:1")

  # Add name to suite if present so we can auto-append a fixture
  if(DEFINED _tests)
    list(APPEND _tests ${testname})
    set(_tests ${_tests} PARENT_SCOPE)
  endif()

  ## Setup environment
  # We utilize the ability to pass FOO=1 BAR=1 without the need for set: format
  # here; thus this property must come first. The other calls must use
  # ENVIRONMENT_MODIFICATION
  if(TEST_ENV)
    set_tests_properties(${testname} PROPERTIES ENVIRONMENT "${TEST_ENV}")
  endif()

  ## Add all labels (if any)
  if(TEST_LABELS)
    set_tests_properties(${testname} PROPERTIES LABELS "${TEST_LABELS}")
  endif()

  ## Create a sandbox
  # In order to support stest.scratch_file; we must set SANDBOX and create
  # a scratch folder within it.
  set(sandbox "${CMAKE_CURRENT_BINARY_DIR}/sandbox")
  file(MAKE_DIRECTORY ${sandbox}/scratch)
  set(env_mod)
  list(APPEND env_mod "SANDBOX=set:${sandbox}")
  ## Setup ASAN
  if(NOT WIN32 AND USE_ASAN AND SANITIZERS_SUPPORTED)
    list(APPEND env_mod "LSAN_OPTIONS=set:${LSAN_OPTIONS};LD_PRELOAD=path_list_prepend:${LIBASAN_PATH};ASAN_OPTIONS=set:${ASAN_OPTIONS}")
  endif()
  set_tests_properties(${testname}
    PROPERTIES ENVIRONMENT_MODIFICATION "${env_mod}")
endfunction()

macro(simics_test_suite_begin)
  set(_tests "")
endmacro()
macro(simics_test_suite_end)
  if(DEFINED _tests)
    # TODO(ah): namespace is copy-pasted from simics_add_test for now
    cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH _module)
    cmake_path(GET _module FILENAME namespace)

    # It's expected for a suite to run from an empty scratch
    set(scratch "${CMAKE_CURRENT_BINARY_DIR}/sandbox/scratch")
    add_test(NAME ${namespace}::clean-scratch
      COMMAND ${CMAKE_COMMAND} -E rm -rf ${scratch})
    add_test(NAME ${namespace}::create-scratch
      COMMAND ${CMAKE_COMMAND} -E make_directory ${scratch})
    set_tests_properties(${namespace}::clean-scratch
      PROPERTIES FIXTURES_SETUP clean-scratch)
    set_tests_properties(${namespace}::create-scratch
      PROPERTIES FIXTURES_SETUP clean-scratch)
    set_tests_properties(${namespace}::create-scratch
      PROPERTIES DEPENDS ${namespace}::clean-scratch)
    foreach(test ${_tests})
      set_tests_properties(${test}
        PROPERTIES FIXTURES_REQUIRED clean-scratch)
    endforeach()
  endif()
  unset(_tests)
endmacro()

function(simics_package)
  set(one_value_args NAME NUMBER VERSION TYPE BUILD_ID BUILD_ID_NAMESPACE)
  cmake_parse_arguments(PACKAGE "" "${one_value_args}" "" ${ARGN})

  if(NOT PACKAGE_NAME)
    message(FATAL_ERROR "No NAME specified for SIMICS_PACKAGE_INFO.")
  endif()
  if(NOT PACKAGE_NUMBER)
    message(FATAL_ERROR "No NUMBER specified for SIMICS_PACKAGE_INFO.")
  endif()
  if(NOT PACKAGE_VERSION OR PACKAGE_VERSION STREQUAL "None")
    message(FATAL_ERROR "No VERSION specified for SIMICS_PACKAGE_INFO.")
  endif()
  if(NOT PACKAGE_TYPE)
    set(PACKAGE_TYPE addon)
  endif()
  if(NOT PACKAGE_BUILD_ID)
    set(PACKAGE_BUILD_ID 0)
  endif()
  if(NOT PACKAGE_BUILD_ID_NAMESPACE)
    set(PACKAGE_BUILD_ID_NAMESPACE custom)
  endif()

  if(DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    # CMake use-case; honor CMAKE_LIBRARY_OUTPUT_DIRECTORY
    set(root ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
  else()
    # Default Simics use-case; emit packageinfo into project root
    set(root ${SIMICS_PROJECT_DIR})
  endif()
  string(REPLACE " " "-" PACKAGE_NAME ${PACKAGE_NAME})
  configure_file(${Simics_DIR}/packageinfo.in
    ${root}/packageinfo/${PACKAGE_NAME}-${SIMICS_HOST_TYPE})
endfunction()

function(simics_add_documentation name)
  cmake_parse_arguments(DODOC "" "PREFIX;OUTPUT" "CSS;FLAGS;DEPENDS" ${ARGN})
  # NOTE: the core/config/project/doc.mk sets simics-user-build-id ; leave that
  # for end-user to pass in as FLAGS if needed

  if(NOT DODOC_PREFIX)
    set(DODOC_PREFIX doc_)
  endif()
  if(NOT DODOC_OUTPUT)
    set(DODOC_OUTPUT ${SIMICS_PROJECT_DIR}/${SIMICS_HOST_TYPE}/doc/html)
  endif()
  # The current Simics GNU Make wrapper adds simics.css by default
  if(NOT DODOC_CSS AND NOT DODOC_FLAGS)
    set(DODOC_CSS simics.css)
  endif()
  set(CSS_OPTION)
  foreach(css ${DODOC_CSS})
    list(APPEND CSS_OPTION --css ${css})
  endforeach()

  set(dodoc
    ${SIMICS_BASE_DIR}/${SIMICS_HOST_TYPE}/bin/dodoc${CMAKE_EXECUTABLE_SUFFIX})
  set(depfile ${CMAKE_CURRENT_BINARY_DIR}/${name}.dodoc.d)
  add_custom_command(
    OUTPUT ${DODOC_OUTPUT}/${name}/toc.json
    COMMAND ${dodoc} ${SIMICS_BASE_DIR}/src/docs/dodoc
      ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
      ${DODOC_FLAGS} ${CSS_OPTION} -o ${DODOC_OUTPUT}/${name}
      --dep-file ${depfile}
    DEPENDS ${DODOC_DEPENDS}
    DEPFILE ${depfile})
  add_custom_target(${DODOC_PREFIX}${name} DEPENDS ${DODOC_OUTPUT}/${name}/toc.json)
  set_target_properties(${DODOC_PREFIX}${name}
    PROPERTIES ADDITIONAL_CLEAN_FILES ${DODOC_OUTPUT}/${name})
endfunction()

function(simics_copy_file from)
  # Creates a custom command that copies *one* file
  cmake_parse_arguments(COPY "" "TO" "" ${ARGN})

  if(NOT COPY_TO)
    cmake_path(GET from FILENAME COPY_TO)
  endif()
  add_custom_command(
    OUTPUT ${COPY_TO}
    COMMAND ${CMAKE_COMMAND} -E copy ${from} ${COPY_TO}
    DEPENDS ${from})
endfunction()

include(${CMAKE_CURRENT_LIST_DIR}/options.cmake)
