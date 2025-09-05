# Simics CMake Reference Manual

The Simics CMake package provides functions and macros for building Simics
modules and light-weight add-on packages.

The design goal of the Simics CMake package is to not reinvent functionality or
wrap it when the standard CMake API can be used directly.

See [Using CMake in Simics](../cmake/README.html) for detailed usage, BKMs and
current limitations.

## Basic Usage

To load the Simics CMake package into the current CMake project, use the
`find_package()` command:

```
    find_package(Simics REQUIRED)
```
or
```
    find_package(Simics REQUIRED CONFIG NO_DEFAULT_PATH HINTS ${SIMICS_BASE}/cmake)
```

The former usage requires that `CMAKE_PREFIX_PATH` has been set to
`${SIMICS_BASE}/cmake` and the later usage explicitly provides the path
(not trusting any default paths).

The Simics.cmake file contains the bulk of the implementation and provides
these utility functions:
- `simics_find_and_add_modules()` - Locates Simics CMake modules and adds them to the CMake project
- `generate_dml_compile_commands_json` - Generates the `dml_compile_commands.json` file
- `simics_add_module()` - Adds a Simics module to the CMake project.
- `simics_copy_python_files` - Utility to copy python files from other modules
- `simics_add_sanitizers` - Adds support for building with sanitizers
- `simics_add_test()` - Adds a Simics test to the CMake project; so that it can be run with CTest
- `simics_package()` - Creates a package info file; so that the Simics project used to build the Simics modules can be used as an add-on package from some other Simics project.
- `simics_add_dead_methods_check` - Adds post build check for dead DML methods
- `simics_add_documentation` - Adds documentation; wrapper around Simics' `dodoc` tool
- `simics_copy_file` - Creates a custom command that copies a file


## Functions

### simics_global_cmake_properties()

**NOTE:** *This function is invoked automatically by `find_package(Simics)`; listed for reference only.*

Sets up the global properties required by Simics such as:
- Builds with position independent code (`-fPIC`)
- Builds with GNU extensions for both C and C++

### simics_project_setup()

**NOTE:** *This function is invoked automatically by `find_package(Simics)`; listed for reference only.*

The main setup function that configures the build-tree for building Simics
modules with CMake. It detects and sets up the Simics project required by the
build. The following modes are supported:

1. User provides the path to a Simics project via the `SIMICS_PROJECT_DIR`
   variable, and the CMake project gets associated with this Simics
   project. This mode overrides/takes precedence over (2), so it does not
   matter if user runs from within a Simics project or not.

2. Simics project exists at the root of the CMake project, pointed to by the
   `CMAKE_CURRENT_SOURCE_DIR` variable. This is the traditional Simics setup,
   and is automatically detected.

3. Simics.cmake will implicitly create a Simics project in the root of the
*build tree*, pointed to by the `CMAKE_CURRENT_BINARY_DIR` variable, if a path
is not provided (1) and project could not be detected (2). This requires that
Simics.cmake is found within a Simics Base package, which is the common case.

For all cases 1-3 the Simics modules are emitted into the
`${SIMICS_HOST_LIB_DIR}` folder as expected by Simics,
e.g. `<project>/linux64/lib`. This can be overridden by providing the standard
`CMAKE_LIBRARY_OUTPUT_DIRECTORY` cache variable, in which case the modules are
emitted to this directory instead.

The `simics_project_setup()` function also sets up a few [targets](#targets) and [variables](#variables).

### generate_dml_compile_commands_json()

If put at the end of the top-level `CMakeLists.txt` (after having added all the
Simics modules), and the `CMAKE_EXPORT_COMPILE_COMMANDS` option has been
enabled, it will generate a `dml_compile_commands.json` file in the build
directory. This file can be used by the VSCode DLS (DML Language Server)
plugin.

Please note that, for practical reasons, the `--` and `-` prefixes of each DMLC
flag have been stripped prior to adding them to the .json file.

### simics_add_module()

Adds a Simics module to the project using the specified source files.

Module sources can be a mix of C, C++, DML and Python files.

Please note that even though it is possible to provide classes, components and
interfaces using a single module this should be avoided. The traditional way to
model classes, components and interfaces is to wrap them in separate modules;
as this makes it easier to locate and re-use the components and interfaces
later on. Typically interfaces are used between two classes provided by two
different modules; so it makes sense to keep the interface as a separate entity
for testing. Similarly components provide a higher level abstraction to
creating Simics configurations, and while it might seem useful to provide the
component as part of the module that provides the class(es) it's usually better
to provide the component as a separate entity.

Providing multiple interfaces or components or classes within the same module
is not a problem if it makes sense to do so.

Multiple calls to `simics_add_module()` can be made from within the same
`CMakeLists.txt` file. Each call defines a new module.

#### Usage
```
    simics_add_module(<module-name>
        {CLASSES | COMPONENTS | INTERFACES} <name> ...
        [SOURCES <file> ...]
        [PYWRAP <file> ...]        
        [IFACE_FILES <file> ...]
        [SIMICS_API <5|6|latest>]
        [INIT_LOCAL]
    )
```

#### Arguments
- module-name ; is the name of the module to register. This also creates a CMake
target with the same name, in the form of a `MODULE` type library (see [cmake.org:add_library](https://cmake.org/cmake/help/latest/command/add_library.html#normal-libraries) for details).

- `CLASSES`, `COMPONENTS` and `INTERFACES` ; must list all the corresponding
classes, components and interfaces provided by the module. This is added as
meta-data to the Simics module and is used by Simics to locate a suitable
module to load when looking for a specific class, component or interface.

- `SOURCES` ; lists all the sources that go into creating the classes,
components or interfaces. Please note that only top-level DML files that
contain the `device`-statement should be added for DML modules.

- `PYWRAP` ; Any header that contains types or functions that should be
python-wrapped must be listed. Headers that contain `SIM_INTERFACE` or
`SIM_PY_ALLOCATABLE` or `SIM_CUSTOM_ATOM` can be listed in `SOURCES` instead as
they will be automatically detected.

- `IFACE_FILES` ; _DEPRECATED_ use `SOURCES` or `PYWRAP` instead.

- `SIMICS_API` ; sets the API used by this module and should be provided, but if
it's not provided it defaults to `latest`; which means it will be the version
of the associated Simics Base package.

- `INIT_LOCAL` ; must be given if a module is providing its own `init_local` entry
point.

#### Example usages (from our sample modules)
```
    simics_add_module(sample-device-cpp
        CLASSES sample_device_cpp
        SOURCES sample-device.cc
        INIT_LOCAL)
```
```
    simics_add_module(sample-device-c
        CLASSES sample-device-c
        SOURCES sample-device.c
        INIT_LOCAL)
```
```
    simics_add_module(sample-device-dml
        CLASSES sample_device_dml
        SOURCES sample-device-dml.dml)
```
```
    simics_add_module(sample-device-mixed
        CLASSES sample_device_mixed
        SOURCES sample-device-mixed.dml sample-device-mixed.c
        INIT_LOCAL)
```
```
    simics_add_module(sample-device-python
        CLASSES sample_device_python
        SOURCES sample_device_python.py)
```
```
    simics_add_module(sample-interface
        INTERFACES sample
        IFACE_FILES sample-interface.h)
```
```
    simics_add_module(sample-comp
        COMPONENTS sample_comp
        SOURCES sample_comp.py)
```

### simics_add_sanitizers()

Annotates a module/target to use sanitizers.

Sanitizers can be built into a module by passing compiler and linker flags to the build. The following flags are currently supported:
- `-fno-omit-frame-pointer`
- `-fsanitize=address`
- `-fsanitize=undefined`

In addition, the `RPATH` must also be set to locate the sanitizer libs from within the corresponding toolchain used to build the module:
- `-Wl,-rpath,${SAN_DIR})` 

The `simics_add_sanitizers()` function adds these compiler and linker flags to the target module based on the values of the following options:

- `USE_ASAN` ; Enables ASAN flags when building modules annotated by `simics_add_sanitizers()`. Default=OFF
- `USE_UBSAN` ; Enables UBSAN flags when building modules annotated by `simics_add_sanitizers()`. Default=OFF
- `ASAN_STACK_USE_AFTER_RETURN` ; Same as `USE_ASAN` but also enables 'stack-use-after-return'. Default=OFF


### simics_add_test()

Adds a Simics test to the project using the specified test name or file, so
that the test can be run through CTest; see examples below.

The `simics_add_test()` function wraps the standard CMake `add_test()` function. User can add tests by name (i.e. `s-<name>.py`) or file (i.e. `s-name.py`) and the function then automatically adds the current module's folder name as namespace so the tests can easily be filtered by CTest.

The tests are run through Simics in batch-mode.

#### Usage
```
    simics_add_test(<test-name | test-file>
        [CWD <path>]
        [SYS_PATH <path> ...]
        [ENV <VAR=value> ...]
        [NAMESPACE <namespace>]
        [NAME <name>]        
        [PYTEST]
        [LABELS label ...]
    )
```

#### Arguments
- test-name or test-file ; is the name of the test. Simics tests follow the `s-<name>.py` naming convention. The filename can also be given as relative or absolute path, in which case the name portion will be extracted and used as name for the CTest.

- `CWD` ; changes the current-working-directory used by Simics when running the test. Defaults to `${CMAKE_CURRENT_SOURCE_DIR}` if not provided. Useful if the tests to run are located in some common folder outside of the current test suite.

- `SYS_PATH` ; appends the given path(s) to Python's sys.path before invoking the test. Useful when tests need common code located elsewhere. Please note that `CWD` should be used if test can run from the common path without modifications to sys.path.

- `ENV` ; sets the given environment variables to the given values. Useful for passing configuration to common test-code, such as class name.

- `NAMESPACE` ; sets the namespace instead of deriving it from the suite's parent's parent folder name.

- `NAME` allows the caller to change the given test name. The module namespace will still be added/prefixed. Useful when tests are added from shared folders and end up having the same names.

- `PYTEST` runs the test through `pytest` instead of Simics. Assumes `pytest` can be found in `PATH`, or the path to pytest must be provided through the `PYTEST_BINARY` cache variable.

- `LABELS` ; A list of labels that is directly passed on to the `LABELS` property of the created test via `set_tests_properties`.

#### Example usages
```
    simics_add_test(sanity)
```
```
    function(cmn_add_tests class module)
      set(cwd ${CMAKE_CURRENT_FUNCTION_LIST_DIR})
      file(GLOB tests CONFIGURE_DEPENDS "${cwd}/s-*.py")
      foreach(test ${tests})
        set(env "CMN_CLASS=${class};CMN_MODULE=${module}")
        simics_add_test(${test} CWD ${cwd} ENV ${env})
      endforeach()
    endfunction()
```
```
    set(sample ${SIMICS_REPO_ROOT}/src/devices/sample-device-python)
    simics_add_test(sys-path SYS_PATH "${sample};..")
```
```
    simics_add_test(py-test.py PYTEST NAME py-test-renamed.py)
```

#### Example invocations of CTest
```
$ ctest --preset release -j20 -R sample-device
Test project /disk1/ah/simics-pr/bt/release
    Start 127: sample-device-c++::reg
    Start 132: sample-device-python::sample-python
    Start 128: sample-device-c++::sanity
    Start 131: sample-device-mixed::sample-mixed
    Start 130: sample-device-dml::sample-device-dml
    Start 126: sample-device-c++::multi
    Start 129: sample-device-c::sample-c
1/7 Test #129: sample-device-c::sample-c ..............   Passed    0.47 sec
2/7 Test #130: sample-device-dml::sample-device-dml ...   Passed    0.48 sec
3/7 Test #131: sample-device-mixed::sample-mixed ......   Passed    0.48 sec
4/7 Test #126: sample-device-c++::multi ...............   Passed    0.48 sec
5/7 Test #128: sample-device-c++::sanity ..............   Passed    0.49 sec
6/7 Test #132: sample-device-python::sample-python ....   Passed    0.50 sec
7/7 Test #127: sample-device-c++::reg .................   Passed    0.54 sec

100% tests passed, 0 tests failed out of 7

Total Test time (real) =   0.56 sec
```

### simics_test_suite_begin() and simics_test_suite_end()

The `simics_test_suite_begin()` and `simics_test_suite_end()` macros marks the
beginning and end of a Simics test suite. All tests added by calls to
`simics_add_test()` between these two calls will be added to the same suite.

A suite will automatically create a scratch clean-up test fixture and add this
fixture as a requirement to all the tests. This will guarantee that clean-up
will run before any tests will run, even when the user runs just a subset of
the tests.

This is useful, and a requirement, for tests creating files such as checkpoints
within the scratch folder.

#### Usage
```
    simics_test_suite_begin()
```
```
    simics_test_suite_end()
```

#### Arguments

There are currently no arguments.

#### Example usages
```
    simics_test_suite_begin()
        simics_add_test(checkpoint-create)
        simics_add_test(checkpoint-load)
    simics_test_suite_end()
```

### simics_copy_python_files()

Copy files from another module or CMake target into the current module's simmod
directory. These files should not be added to the module's `SOURCES` parameter.

#### Usage
```
    simics_copy_python_files(<module>
        FROM <other-target>
        FILES <file> ...
    )
```

#### Arguments
- module ; the module to copy selected python files into.

- `FROM` ; the source module (or target) to copy selected python files from. The root path is extracted from the target's `SOURCE_DIR` property.

- `FILES` ; the file or files to copy


### simics_find_and_add_modules()

Locates modules in the current project, from Simics Base and any associated
add-on packages and then adds them to the current project by invoking
`add_subdirectory(... EXCLUDE_FROM_ALL)`. This allows local modules to add
external modules as taget link dependencies. Because these external modules are
excluded from the 'all' build target they are not built by default; but can be
built if explicitly provided as targets.

This means that unlike the old GNU Make driven Simics project, the user can
build modules in a project directly without first copying them into the
`modules/` sub-directory.

By default, `simics_find_and_add_modules()` will search both the local project,
in Simics Base and all add-on packages; but the user can opt-out of this
and only search in the project's modules/* by passing `IN_PROJECT` as
argument. If passing `IN_PACKAGES` the function will only search in the
packages and not in the project.

### simics_package()

Creates a Simics package info file in the 'packageinfo' sub-directory to make the
package selectable from another project using the Simics addon manager.

Please note that package number and version must be handled by the caller, the
function is just blindly adding the provided info as metadata to the package
info file.

#### Usage
```
    simics_package(
        NAME <simics-package-name>
        NUMBER <simics-package-number>
        VERSION <simics-package-version>
        [TYPE {base|addon}]
        [BUILD_ID <build-id-number>]
        [BUILD_ID_NAMESPACE <build-id-namespace-name>]
    )
```

#### Arguments
- `NAME <simics-package-name>` ;
  Simics package name. Spaces will be converted to hyphens.
- `NUMBER <simics-package-number>` ;
  Unique number of the Simics package.
- `VERSION <simics-package-version>` ;
  Simics package version.
- `TYPE {base|addon}` ;
  Type of package. Defaults to 'addon' if not provided.
- `BUILD_ID <build-id-number>` ;
  Package build-id. Defaults to 0 if not provided.
- `BUILD_ID_NAMESPACE <build-id-namespace-name>` ;
  Package build-id namespace. Defaults to 'custom' if not provided.

#### Example usage
```
    simics_package(NAME "My Simics Package" NUMBER 4200 VERSION 7.0.0)
```

### simics_add_dead_methods_check()

Annotates a module/target to run a post build command that checks if the module
contains any dead DML methods.

For the command run, the CMake cache variable `USE_DML_DEAD_METHODS_CHECK` must
be set during configuration of the project.

The command by default only checks for dead methods in DML files that is part of
the current module and resides in the same folder where the module is defined.

#### Usage
```cmake
    simics_add_dead_methods_check(<target>
        [EXTRA_SOURCES <path(s)>]
    )
```

#### Arguments

- target ; the target to add the post build command to.

- `EXTRA_SOURCES` ;
  Optional list of directories and/or files that should be included in the dead
  code analysis of the target's module. This would typically be some common code
  or a library written somewhere else.

#### Example usage

```cmake
simics_add_dead_methods_check(sample-pcie-device
    EXTRA_SOURCES $<TARGET_PROPERTY:common-code-target,SOURCE_DIR>
)
```

### simics_add_documentation()

Adds a target that builds documentation using the Simics `dodoc` tool. The
target is *not* added to the `ALL` target, unlike simics_add_module, so an
explicit call to `add_dependencies` is needed. The target name will be prefixed
with `PREFIX` to avoid name clashes between the documentation target and module
targets.

This function is just a wrapper around the `dodoc` tool distributed with Simics
Base. Most likely larger projects will create their own wrapper, e.g.:
```cmake
function(add_documentation name)
  cmake_parse_arguments(ARG "" "" "INPUT_DIRS;DEPENDS" ${ARGN})
  simics_add_documentation(${name}
    FLAGS --css simics.css ${ARG_INPUT_DIRS}
    DEPENDS ${ARG_DEPENDS})
  add_dependencies(simics-docs-all doc_${name})
endfunction()
```

See `dodoc --help` or the Simics documentation for more details.

If any files should be copied or generated the caller is responsible for
writing the rules to do so using CMake's `add_custom_command()`. The generated
files must then be listed in the `DEPENDS` argument to make sure they are
generated before `dodoc` runs.

By default `dodoc` will generate a dep-file containing all files that is part
of the `toc.json` along with the `toc.json` itself. Thus it is not necessary to
add these (existing) files to `DEPENDS`.

#### Usage
```
    simics_add_documentation(<name>
        [PREFIX <prefix>]        
        [OUTPUT <path>]
        [CSS <css> ...]
        [FLAGS <flag> ...]        
        [DEPENDS <path> ...]        
    )
```

#### Arguments
- name ; the name of the CMake target and also the name of the subfolder within the `OUTPUT` folder.

- `PREFIX` ; the target name will be `${PREFIX}${name}`. If not set, defaults to 'doc_'

- `OUTPUT` ; the root folder for generated docs. If not set, defaults to `<project>/<host>/doc/html`

- `CSS` ; passes the listed css files to `dodoc` as `--css <file>` flags. If not set and `FLAGS` is also not set, defaults to `simics.css`.

- `FLAGS` ; additional flags passed to `dodoc`. If not set, defaults to the empty string. The `CMAKE_CURRENT_SOURCE_DIR` and `CMAKE_CURRENT_BINARY_DIR` are always passed to `dodoc`, in that order, as input directories.

- `DEPENDS` ; adds all listed files (or target) as dependencies for the dodoc invocation, making sure they are created/copied/generated before `dodoc` is invoked. If not set, remains empty.

### simics_copy_file()

Creates a custom command that copies a file from anywhere to anywhere.

#### Usage
```
    simics_copy_file(<from> [TO <path>])
```

#### Arguments
- from ; the full path of the file to copy.

- `TO` ; the name of the target file. It can be both absolute or relative path. If relative, it will be relative the `CMAKE_CURRENT_BINARY_DIR`. If not set, defaults to a relative file with the same name as `from`.


## Targets

The Simics CMake package provides the following `IMPORTED` `INTERFACE` targets:
- `Simics::C++`
- `Simics::C++v1` (for legacy support, will be removed in Simics 7)
- `Simics::DML`
- `Simics::Python`
- `Simics::Simics`
- `Simics::includes`

and these utility targets:
- `${MODULE_NAME}`
- `${MODULE_NAME}::headers`
- `${MODULE_NAME}::imports`
- `${MODULE_NAME}::includes`
- `simics-modules`
- `list-simics-modules`
- `simics-asan`

and these internal targets (please ignore, listed for completeness only):
- `${MODULE_NAME}-copy-python-file`
- `${MODULE_NAME}-artifacts`

The `Simics::Simics` is the basic Simics library and all CMake Simics modules
depend on this target. `Simics::DML` and `Simics::Python` are automatically
added as dependencies for .dml and .py files. User must explicitly add
`Simics::C++` as target dependency to use the Simics C++ API. The
`Simics::C++v1` is provided as legacy support for older models. Non-modules
(e.g. `STATIC` libraries) that invokes the Simics API must add a target link
dependency on `Simics::includes`.

If B is a Simics module and module A depends on headers from B, then A should
add a target link dependency on `B::includes` (or the aliases `::headers` or
`::imports`). A cannot depend directly on B because B is of library-type
`MODULE` which prohibits it from being linked directly to other libraries.

The `simics-modules` target builds all registered modules, while
`list-simics-modules` simply prints all modules to stdout.

The `simics-asan` is just a convenience layer for launching Simics with
`LD_PRELOAD` and `ASAN_OPTIONS` set. See ['Sanitization with ASAN' in 'Using CMake in
Simics'](../cmake/README.html#sanitization-with-asan-and-ubsan) for details on
using ASAN on Simics modules.


## Variables

### Cached (can be set by user after configuration)
- SIMICS_API - Default Simics API used if not set by module
- SIMICS_BASE_DIR - Path to Simics Base package
- SIMICS_BUILD_ID - Build-id value compiled into the module
- SIMICS_EXTRACT_LIMITATIONS_PY - Can be set to `OFF` to bypass extraction of limitations
- SIMICS_DMLC_DIR - The path to where the DML compiler was built; defaults to Simics Base
- SIMICS_DMLC_PYTHON - The python interpreter used to invoke DMLC; defaults to mini-python

### Read-only (should not be set by user, but can be used/read)
- SIMICS_DMLC - the resulting invocation of the DML compiler
- SIMICS_EXECUTABLE - Simics executable
- SIMICS_HOST_LIB_DIR - Simics project <host>/lib directory
- SIMICS_HOST_TYPE - Simics host type (linux64 or win64)
- SIMICS_PROJECT_DIR - Path to the associated Simics project
- SIMICS_PYTHON_EXECUTABLE - Simics Python (i.e. mini-python) executable
- SIMICS_SYS_LIB - Simics build variable
- SIMICS_VERSION - Simics (full) version
- SIMICS_VERSION_MAJOR - Simics major version
- SIMICS_VERSION_MINOR - Simics minor version
- SIMICS_VERSION_PATCH - Simics patch version

### Internal (should not be used by user)
- ASAN_OPTIONS - ASAN runtime options
- LIBASAN_PATH - Path to libasan
- SANITIZERS_SUPPORTED - Sanitizer flags supported
- SAN_DIR - Directory of sanitizers
- SIMICS_ANALYZE_TRAMPOLINES_PY - Simics build variable
- SIMICS_COPY_DEVICE_XML_PY - Simics build variable
- SIMICS_COPY_PYTHON_PY - Simics build variable
- SIMICS_GEN_DML_COMMANDS_JSON_PY - Simics DML commands json generator
- SIMICS_INC - Simics build variable / Use `Simics::includes` target instead
- SIMICS_LIB - Simics build variable / Use `Simics::Simics` target instead
- SIMICS_MODULE_ID_PY - Simics build variable
- SIMICS_PYTHON_INC - Simics build variable / Use `Simics::Python` target instead
- SIMICS_PYWRAP_DIR - Simics build variable

## Tips and tricks

### C/C++ modules

- Unlike DML modules, the C and C++ modules must define the `init_local()`
entrypoint for the Simics module:
    ```
    extern "C" void init_local() {
        ...
    }
    ```

- See [C++ Device API v2](../cc-device-api/index.html) for details on using the C++ API.

- Add a target dependency on `Simics::C++` to build and link with Simics C++ API
library. Add target dependency on `Simics::C++v1` for legacy v1 support.

### DML modules

- Use `SIMICS_DMLC_FLAGS` property to set extra flags for DML compiler. The
property can be set globally, per directory, per module and per DML source
file.

   Since there is no generator expression for source file properties (as of CMake
3.28); the source file property is extracted during configuration phase and
thus must be set *before* invoking `simics_add_module()`

   **NOTE:** *Multiple flags must be separated by semi-colon*

   Please note that CMake does not inherit properties if explicitly set, meaning that if the `TARGET` property is set in a module any flags passed as `DIRECTORY` or `GLOBAL` property must be explicitly handled by the module itself.

### Simics API

Use the `SIMICS_API` directory property to change the default API used by all
targets in the current directory and all sub-directories. The module property
can be used to override the default directory property.

```
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY SIMICS_API 5)
```

The API version for a module is resolved in the following order:
- If `SIMICS_API` target property is not set; is it inherited from directory or
  global properties.
- If `SIMICS_API` is provided by `simics_add_module()` function; it sets the
  target property and overrides any directory or global properties.

Please note that only `simics_add_module` supports setting `SIMICS_API` to
`latest`. If set to `latest` the current Simics major version of the associated
Simics Base is used.

### Initialization order dependency

When a class attempts to reference another class within the same module, such as by initializing a port with `init_as_subobj` in DML or directly calling `SIM_get_class()`, careful attention must be paid to the initialization order.

In C/C++ and Python, the order can typically be managed within the `init_local` function. However, in DML, this is not possible, nor is it feasible when classes originate from different languages (i.e., with distinct entry points). The build system currently defines the initialization order as follows:
1. Custom C interface wrappers (experimental feature, not used by Simics CMake)
2. Python interface wrappers
    - Implicitly included through `SOURCES` as headers files with `SIM_INTERFACE` calls; or
    - Explicitly included through `PYWRAP` as headers files with custom types or functions to pythonwrap
3. DML generated initialization functions, specified in `SOURCES` as `.dml` files
4. Explicit `init_local()` function, included through `SOURCES` as source file when `INIT_LOCAL` is set.
5. Code run from `module_load.py`

To some degree, dependencies can be managed by altering the sequence of files specified in `SOURCES`. For instance, if `a.dml` depends on a class defined in `b.dml`, rearranging the order from `a.dml b.dml` to `b.dml a.dml` will establish the correct dependency order and ensure proper functionality.

Classes from separate modules are automatically and correctly resolved by the Simics module loading system. Therefore, placing dependent classes in a different module can resolve issues with initialization order. In CMake, this is easily achieved by adding an additional `simics_add_module()` call within the same or a different `CMakeLists.txt`.
   

### THREAD_SAFE

By default all modules should be thread safe in Simics. But if they are not they can be tagged using the `SIMICS_MODULE_NOT_THREAD_SAFE` directory property:

```
set_directory_properties(PROPERTIES SIMICS_MODULE_NOT_THREAD_SAFE TRUE)
```

This currently affects the `--thread-safe` flag passed to the `module_id.py` tool used to generate the `module_id.c` file linked into the Simics module. The setting will be part of the module metadata. When loading a module that is not thread-safe into Simics the `disable-multimachine-accelerator` command will be automatically invoked to disable multi-threading. The user can still enable multi-threading by explicitly invoking `enable-multimachine-accelerator`; so this annotation is just a convenience layer.

**NOTE:** *Simics is phasing out the use of THREAD_SAFE, but currently there is no good replacement.*

### Simics tests

To alter how tests interact with Simics you can use the standard
`set_tests_properties()` function with one of the standard test properties. See
[cmake.org:properties on
tests](https://cmake.org/cmake/help/latest/manual/cmake-properties.7.html#properties-on-tests)
for more details.

For example; this is how to toggle that a test is always failing:
```
set_tests_properties(bp-manager::bm-commands PROPERTIES WILL_FAIL TRUE)
```

And this is how to introduce test dependencies:
```
set_tests_properties(ich10-lan-v2::lan-checkpoint2
  PROPERTIES DEPENDS ich10-lan-v2::lan-checkpoint)
```

**NOTE:** *If you update the environment the `ENVIRONMENT_MODIFICATION`
property must be used and not the `ENVIRONMENT` property. The later will
overwrite the variables set by the `simics_add_test()` function.*

