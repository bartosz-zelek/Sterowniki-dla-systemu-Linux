# Using CMake* with the Intel Simics Simulator

See the [Simics CMake* Reference Manual](../refmanual-cmake/README.html) for details
on the Intel Simics Simulator CMake package.

## Introduction to CMake

### Why CMake?

There are many reasons for using CMake instead of GNU Make. Some are
opinionated while others are not. The major upsides comparing these two are:

- CMake is a generator of build systems while GNU Make is just a build
  system. This allows CMake to split the build in two phases: configuration and
  build. And that allows CMake to apply more structure to the flow. It's also a
  lot faster as configuration only need to happen once.

- CMake is not self-modifying, unlike GNU Make, making the flow easier to
  understand and debug.  The flow is also more script-like with support for
  functions and macros making it easier to understand and develop.

- Variables in CMake functions are scoped so there is no information leakage 
  and contamination of the values of global variables. CMake also uses a property 
  system where properties can be set for each individual target, leading to 
  even less cross-target contamination and thus fewer bugs.

- CMake has a better and more structured way of tracking target dependencies;
  making it easy to control and understand the build flow.

- CMake consists of just a few basic build rules, making it easy to use. There
  are only `add_library`, `add_executable` and `add_custom_target` for adding
  things to build. The basic and common functionality of how to locate and
  invoke the compiler is driven through properties and features, with sensible
  defaults if not set. So for simple targets you don't have to write much code
  while for complicated targets you can still write complicated logic to
  satisfy the build requirements/dependencies.

- The CMake language is supported by many IDEs, providing
  both CLI and UI support. The script-like syntax and standardized split
  between code and data (e.g. the ConfigCache.txt that keeps all cached
  variables) makes it parseable by tools and IDEs. The built-in support to
  generate compile_commands.json allows IDEs to provide contextual C/C++
  support.

- CMake is cross-platform. Though you still have to handle OS-specific things,
  most of the basics are handled automatically or provided through standard
  mechanisms.

- CMake supports and highly encurages out-of-source-tree builds. And because of
  this it also handles multiple coexisting build trees out-of-the-box, which is
  useful to separate debug and release builds for example.

### What is CMake

CMake is a build system generator. It defines targets (modules, libraries,
executables, custom) and their dependencies. The dependency scope can be private,
transitive (interface), or both (public). This is defined in `CMakeLists.txt`
files starting from the root and adding more files as they are consumed by CMake.

The build system is generated into a build tree by invoking CMake. The path to
the source tree root, the path to the build tree and the type of build system
to generate (Ninja*, GNU Make, etc) are the only required inputs. Additional
configuration parameters such as the build type (Debug or Release) and where to
locate the compiler etc. can be provided from CLI or through some UI.

Once the build system has been generated, it can be run through the build
system runner (Ninja, GNU Make, etc) directly from the build tree "just like
normal" *or* through the CMake do-it-all binary (i.e. `cmake --build <path to
build tree> --target <target>`).

### The key to understanding CMake

The best way to understand how CMake works is to think of everything as
targets. A target is what you build, for example an executable or a library. It
probably depends on a few libraries and those are also targets. It might depend
on a header-only library and that is also a target. All targets are linked (not
necessarily by a linker, but conceptually) using `target_link_libraries`. The
scope, properties and features of each target defines how it will interact with
other targets.

## Minimum requirements

Currently the minimum required version of CMake is 3.22.

The requirement comes from our use of [ENVIRONMENT_MODIFICATION](https://cmake.org/cmake/help/latest/prop_test/ENVIRONMENT_MODIFICATION.html) in `simics_add_test()` function.

There is no minimum required version of Ninja, but it has been tested with
1.11.1 on both Linux* and Microsoft* Windows*.

### Recommended version

The latest is the greatest, as later versions not only contains new features
but also bugfixes and improved performance. Thus the recommended version for
both CMake and Ninja is the latest version.

Running with CMake of at least 3.24 is preferred as it adds the `--fresh`
command line option that allows you to re-run the configuration *without* any
of the cached variables.

Installing CMake locally should be easy but if access to the host's package
management system is not granted or it does not provide a recent enough
version; you can install CMake and Ninja in a local folder or home directory
using the pip package management system in Python*.

## Current limitations

These are the known limitations:
- Does not work for SystemC* (yet)
- When using GNU Make as target build system generator on Windows, spaces in
  paths does not work. Using Ninja as the build target generator does support
  spaces in paths. 

## Recommendations

- Use Ninja as target generator, because
    - Ninja can be used on both Linux and Windows
    - Ninja can easily be installed using pip (along with CMake)
    - Ninja does not have any problems with spaces on Windows
    - Ninja can build all types of modules on Windows, while Visual Studio can only be used to build a subset. Thus using Visual Studio is not officially supported.
    - Ninja is faster than GNU Make
- Use CTest* as the test runner. Do not use the old Simics `test-runner` test runner.
    - CTest can run tests in parallel, and is thus much faster
    - CTest can easily run only a subset of tests
    - CTest can easily re-run only the failed tests
- Read up on CMake before you start using it
    - See next chapter

## Working with CMake: recommended reading and references

These sources of reference will give you everything you need:

- See [Simics CMake Reference Manual](../refmanual-cmake/README.html) for details
on the Intel Simics Simulator CMake package.

- The Intel Simics Simulator base package contains converted modules to use as
  examples. E.g. the `sample-device-*` modules are easy and straightforward 
  and covers the most common usage.

- Links to the official documentation:
    - <https://cmake.org/cmake/help/latest/index.html>
- An Introduction to Modern CMake (tutorial):
    - <https://cliutils.gitlab.io/modern-cmake/>
- Dos and don'ts:
    - <https://gist.github.com/mbinna/c61dbb39bca0e4fb7d1f73b0d66a4fd1>

## Creating an Intel Simics project with CMake support
To create a new Intel Simics project configured for building modules using
CMake:

```shell
$ [simics]/bin/project-setup my-simics-project
```

Where `[simics]` is the location of the Intel Simics simulator base package
installation. 

This will create a basic `CMakeLists.txt` file in the root of the project,
ready to be used by CMake to build the modules within the project. See [More
details on how it works](README.html#more-details-on-how-it-works) for more
details.

### Updating an existing project from GNU Make to CMake
To upgrade an existing project to use CMake (i.e., there is no `CMakeLists.txt`
at the top level in the project), run `bin/project-setup` with the
`--with-cmake` and `--force` flags from the root of the project:

```shell
$ bin/project-setup --with-cmake --force
```

Please note that only converted modules will be built by CMake. To build
modules not yet converted, use GNU Make.

## Adding modules to the Intel Simics project

This works the same as with Make-based projects. 
Use `project-setup` with `--copy-module`,
`--device`, or other options to create new modules.

Please note that `--copy-module` might copy modules that have not yet been
ported to CMake, in which case they must be ported before they can be built
with CMake. See the porting chapter below.

## Building modules

### General usage

CMake is not a build system in itself, it is a generator of build systems.

Thus the traditional GNU Make driven build process is, by design, split in two
parts when working with CMake:
- Configuration phase ; in this phase you select the generator (i.e. target
build system), the compiler to use, configure paths and options etc. The
configuration phase generates a build system into a designated directory
referred to as the build tree.
- Build phase ; the generated build system can be built directly from inside
the build tree directory using the targeted builder (e.g. Ninja or GNU
Make). Or it can be built indirectly from the project root using `cmake --build
<build tree>`. Changes to configuration generally require the user to re-run
the configuration phase, though some changes will automatically trigger a
re-configuration.

It's important to make this distinction. It allows CMake to split the
collection of static properties from dynamic properties collected during each
build. This allows the build to run much faster.  The split between
configuration and build is also the reason why CMake frowns upon globbing;
globbing for locating files should be avoided if possible.

The Intel Simics Simulator expects to locate built modules in the "host lib"
folder in the project, e.g. `[project]/linux64/lib`. This is where the Intel
Simics Simulator CMake targets emit the results of the build. The intermediate
files remains in the build tree configured by the user.  Unlike the old
GNU-Make-driven flow, a CMake-driven flow can have multiple build trees.  For
example, this allows the user to separate debug and release builds.

### Build modules using the standard CMake program

**NOTE:** *This is the recommended approach*

The simplest option (after getting used to it) is probably to use the `cmake`
program from CLI. On Linux with bash-completions enabled you can tab-complete
the options (but not the targets) and it contains a lot more than just the
`--build` option making it a very useful tool to master.

This is how to bootstrap (i.e. generate a build system) and build everything:

```shell
$ cmake -S . -B bt -G Ninja -DCMAKE_BUILD_TYPE=Release
$ cmake --build bt
```

Where:
- `-S .` ; points to where the top-level `CMakeLists.txt` is located (i.e. the project)
- `-B bt` ; points to where the build tree should be created.  This can be
  anywhere, inside the project or outside of it.
- `-G Ninja` ; select which generator to use (Ninja is the fastest)
- `-D<flag>=<value>` ; sets a build flag. 
   - The `CMAKE_BUILD_TYPE` flag must be set.

See `cmake --build bt --help` for more details on how to build with CMake. Here
is a list of useful options:
- `--target X Y Z` ; builds only X Y and Z targets
  - To build all Intel Simics Simulator modules registered with CMake, build
the `simics-modules` target.
  - To list all modules, build the `list-simics-modules` target.
- `--verbose` ; adds verbosity, shows what you build
- `--clean-first` ; removes artifacts before building, useful to force a rebuild

There is also the `ccmake` program, which is an ncurses frontend to configuring
the build directory:
```shell
$ ccmake bt
```

### Build modules using tiny GNU Make wrapper

**NOTE:** *This is not the recommended approach, only listed for reference*

During the initial transition from GNU Make to CMake, before getting accustomed
to the `cmake` tool, it might feel more convenient to continue with a
make-driven flow:

```shell
$ make
$ make <target>
```

*NOTE:* this depends on the following files and local changes:

- The `cmake-wrapper.mk` file has been copied to the project
  (as is currently done by `project-setup`)
- The `config-user.mk` file contains the following line (must be added manually):
> `-include cmake-wrapper.mk`

The GNU Make wrapping, provided by `cmake-wrapper.mk`, handles the following:

- fetches common flags from the Intel Simics simulator `*.mk` files
- sets up different build directories based on type:
  - `D=1` enables a debug build and this is done in the `build-debug` folder
  - `D=0` enabled a release build and this is done in the `build-release` folder
- makes it easy to setup paths to `CC`, `CXX`, `CMAKE` and `NINJA` and
  reconfigure the build on forced rebuilds.

*LIMITATIONS:*

- standard Intel Simics simulator GNU make targets 
  like `clean-`, `clobber-`, `test-`, etc., do not work.
- support for this mode may be removed without a deprecation phase, so it's
  better to learn how to use `cmake` directly as shown in the previous section.

### Build modules using explicit invocation of the generated build system

Not covered by this documentation. It's recommended to run through the `cmake --build` indirection.

### Build modules using your favourite IDE that supports CMake

Not covered by this documentation.

## Testing modules

The Intel Simics Simulator CMake package adds support for running tests via CTest and
enables this support by default.

To run all available tests for the build tree `bt`:
```shell
$ ctest --test-dir bt
```

Useful parameters to control how the tests are run:
- `-j N` ; run N tests in parallel, will speed up the testing significantly
- `-R <regexp>` ; only run tests matching regexp
- `-E <regexp>` ; exclude tests matching regexp
- `-L <regexp>` ; only run tests with labels matching regexp
- `-N`          ; do *not* run any tests, just show what would have run

Intel Simics simulator module tests are added to CTest via the `simics_add_test()` function. 
See [Simics CMake Reference Manual](../refmanual-cmake/README.html#simics_add_test) for details.

Tests are automatically prefixed with `namespace::` where namespace is the
module directory name. The test name and namespace can be customized if
desired. Namespacing allows the user to filter the tests using `-R` and
`-E`.

*NOTE:* A common beginner's mistake when migrating from an Intel Simics simulator GNU-Make-driven
project to a CMake-driven project is to think of `ctest` in terms of `make test`. 
But it's not. Think of `ctest` as invoking `bin/test-runner` directly, 
i.e. you must first run CMake to build the modules before you can test them with CTest.

For the same reasons, CTest will not trigger a rebuild of the binaries if sources are modified.

Please note that CTest does not read the `CMakeLists.txt` files, so if they are modified 
(i.e. new tests are added) then one must re-run CMake to generate the information CTest needs.


## More details on how it works

CMake expects the root of the source directory (passed to `cmake`) to contain a
top-level `CMakeLists.txt` file that defines the CMake project. This file sets
up compiler settings (but not the compiler!) and defines what libraries and
executables to build. Typically the configuration for these libraries and
executables are located in separate `CMakeLists.txt` files added to the project
by calls to `add_subdirectory()`.

The Intel Simics Simulator CMake package provides the `simics_add_module()` function, and some
other helpful functions described below, to help the user define what to build
in the project. See [Simics CMake Reference Manual](../refmanual-cmake/README.html#simics_add_test) 
for details and basic usage of all functions.

In the Intel Simics Simulator, the expected (i.e. traditional) structure is that 
each module (or set of closely related modules) lives in its own directory containing a 
`CMakeLists.txt` file.  Module tests are typically in a `test/` subdirectory with
yet another `CMakeLists.txt` file that configures and lists the module tests. 
The tests are added by the module and the module is added by the top-level (or some
intermediate) `CMakeLists.txt` file. Unlike the old GNU-Make-driven build flow,
the CMake-driven flow does not make any assumptions on where to locate
modules and tests; it simply follows the chain of `CMakeLists.txt` files.

For convenience, the initial `CMakeLists.txt` created by `project-setup` will
load the Intel Simics Simulator CMake package from the associated Intel Simics
Simulator Base package installation, and locate all
modules in subdirectories of the project root as described below.

In order to use the Intel Simics Simulator CMake package it must be added to 
the project configuration in the top-level `CMakeLists.txt` file:
```
find_package(Simics REQUIRED)
```

This requires that `CMAKE_PREFIX_PATH` has been set to 
`${SIMICS_BASE}/cmake` where `SIMICS_BASE` is the absolute path 
to the Intel Simics Simulator Base package installation.  
The Simics project comes with a default `CMakeLists.txt` file that provides this path explicitly:
```
find_package(Simics REQUIRED CONFIG NO_DEFAULT_PATH HINTS ${SIMICS_BASE}/cmake)
```

The default top-level `CMakeLists.txt` file generated by `project-setup` invokes the
`simics_find_and_add_modules()` function which:

- searches for all modules within the `modules` subdirectory and adds them to the
  CMake project
- searches for all modules in the Intel Simics Simulator Base installation, 
  all add-on packages configured for the Intel Simics project, and add
  them to the CMake project given that they have not already been added from the
  `modules` subdirectory.

*LIMITATIONS:*

- This assumes that the CMake setup only defines one module per directory,
  which is the traditional/legacy GNU Make invariant. In a CMake project it's
  possible to provide multiple modules from the same directory. The
  `simics_find_and_add_modules` really only compares directory names, so if a
  local copy exists the corresponding directory in the add-on package will not
  be loaded regardless of its content.
- Modules added automatically from packages this way are marked as
  `EXCLUDE_FROM_ALL` so they don't pollute the 'all' target. They can still be
  built by specifying the target(s) explicitly.

*NOTE:* CMake will take care of providing the paths of the modules to build to the compiler. 
This means that unlike the GNU-Make-driven flow, modules do not need to be copied into the 
project in order to build them. 

As a user you are not required to use the top-level `CMakeLists.txt` provided by
project-setup. Nor are you required to call `simics_find_and_add_modules`, as
the basic functionality is provided by the Intel Simics Simulator CMake package. 
In fact, you are not even required to create an Intel Simics project. 
See `simics_project_setup()`
in the [Reference Manual](../refmanual-cmake/README.html#simics_project_setup)
for a detailed description of the supported modes. Using CMake provides a lot
more flexibility compared to the old GNU-Make-driven module build system.

## Partially converted project

In a partially converted project there is no good way to build the not-yet-ported 
modules with CMake.  Invoking `make` as a submake from CMake might work
but will have poor performance. It's better and much more straightforward to
continue to build these modules with GNU Make until ported. To avoid building
the ported modules with GNU Make you have two options:

1. Simply remove the `Makefile` from a module once it has been ported to CMake

2. Put the following lines into the `config-user.mk` file in the root of the 
Intel Simics project:

    ```
    # List of modules ported to CMake
    PORTED_TO_CMAKE=module-a module-b module-c
    ```

## Converting an existing GNU Makefile to CMakeLists.txt

Conversion follows these three steps.

1. Run the `gmake-to-cmake` converter to get a good starting point. For
example:

    ```shell
    $ ./bin/gmake-to-cmake modules/AT24Cxxx
    ```

2. Make note of any warnings or errors shown during conversion, for example

    ```shell
    WARNING: MODULE_CFLAGS used, please review:
    ```

3. Make adjustments as necessary by comparing the old `Makefile` and the generated
   `CMakeLists.txt`. 

Please note that the converter is not meant to handle all types of input, and
it only detects and reports a small set of problems. It should be used as a
starting point only, as writing a tool that understands GNU Make is
out-of-scope for the Intel Simics Simulator CMake package.

For trivial modules, such as the `sample-device-*` modules, the converter works
and can be trusted.  But for more complicated modules that use GNU Make logic,
conditional code, generates files, expands and filters variables to construct
new lists, etc etc; the user must conduct a manual review.

For shared common code and other directories that do not define a Simics module,
it's probably easier to start from scratch with an empty `CMakeLists.txt` or use
some existing common code as template.

The following sub-sections labeled A..F provides details and examples of how to
solve some of the common problems with constructing a `CMakeLists.txt` file for
the Intel Simics Simulator.

<section class="not-numbered">

### A) Makefile is using variable references

Most of the time it's better to expand these indirections and use explicit
names for classes and source files. Where indirections are warranted CMake does
support variables via `set(...)` function. CMake has an extensive library of
utility functions that operates on variables and lists to handle the most
common problems.

### B) Makefile is using wildcard to locate files

Most of the time it's better to explicitly list all the files so CMake can
track their dependencies properly. CMake does have support for path pattern matching via
`file(GLOB ...)` function but these pattern matches are only run during configuration
phase and not between consecutive builds; which means you have to explicitly
reconfigure the project if new modules are added to the CMake project. To
mitigate this, CMake's `file(GLOB ...)` has a `CONFIGURE_DEPENDS` option that
causes the pattern match to be re-evaluated on every build. There is a cost involved of
course, so use with caution and avoid if possible.

### C) Makefile is referencing files from common code via EXTRA_MODULE_VPATH

Please note that this section is about *common* code. See (D) below for
referencing files from other modules. Importing other DML files is covered by (D2).

There are two distinct ways common code is used by modules, and they need
different solutions.

#### C1) Makefile is not passing custom defines to the common code

In this case, it's always better to let the other module build a static library
and add a dependency on that library target. This is done using
`add_library(...)` and `target_link_libraries(...)`:

In module A (the user):
```
target_link_libraries(A PRIVATE B)
```

In module B (the provider):
```
add_library(B STATIC 1.c 2.c 3.c ...)
```

The conversion of module A is handled by the converter, but the conversion of
module B has to be done manually. In order to build module B it's likely that
include paths must be added explicitly, and this is done by
`target_include_directories(...)`. Paths to the Intel Simics Simulator standard 
includes are otherwise added to module A via the `simics_add_module(...)` function,
but B cannot depend on `Simics::Simics` as it's a STATIC. Instead we add a dependency
on `Simics::includes`.

In module B:
```
target_include_directories(B
  INCLUDE .)
target_link_libraries(B
  PRIVATE Simics::includes)
```

In the snippet above keywords `PRIVATE` and `INCLUDE` are used to control the scope
and transitivity of these configurations. `PRIVATE` means it only applies to the
current target. `INCLUDE` means is only applies to targets that depend on the current
target. Configuration that should apply to both the current target and targets that
depend on the current target, must use the `PUBLIC` keyword. 
The use of `.` in `target_include_directories` is expanded within B to the current
source path of B, but added to the include paths of A.

#### C2) Makefile is passing custom defines to the common code, or is just referencing the headers

In this case, unlike (C1), the referenced source files (if any) _must_ be built
by module A to honor the module specific defines. To achieve this in CMake we
define an `INTERFACE` library instead of a `STATIC` library. `INTERFACE`
libraries do not produce any output; they are used to pass values and can be
used as target dependencies:

In module A:
```
target_link_libraries(A PRIVATE B)
add_compile_definitions(DEVICE_NAME=A)
```

In module B:
```
add_library(B INTERFACE)
target_sources(B INTERFACE 1.c 2.c 3.c ...)
target_include_directories(B INTERFACE .)
```

Here `INTERFACE` means module A (the user) and the sources listed by module B
(the provider) are added to module A. The obj files produced are put into
module A's build directory and will not be re-used by any other module that
also depends on module B. Please note that since the files are built by A and A
gets Simics include paths added by `simics_add_module` there is no need to
explicitly depend on `Simics::includes` here; unlike in (C1).

An `INTERFACE` type library does not have to provide any sources, it can just
provide include directories. This is useful in (D2) below. Specifically for DML
modules common code is typically shared this way:

```
add_library(cmn-common INTERFACE)
target_include_directories(cmn-common INTERFACE .)
add_library(cmn-common::imports ALIAS cmn-common)
```

### D) Makefile is referencing files from other modules via EXTRA_MODULE_VPATH

There are two type of files: source files and header files.

#### D1) Source files

To share source files between modules a `STATIC` type library as described in
(C1), or an `INTERFACE` type library as described in (C2), must be created and
given a unique name. By convention the NAME given to `simics_add_module` is the
module name and cannot be re-used:

In module A
```
target_link_libraries(A PRIVATE B::shared)
```

In module B
```
add_library(B-shared STATIC event-queue.c)
target_link_libraries(B-shared PRIVATE Simics::includes)
add_library(B::shared ALIAS B-shared)
```
or
```
add_library(B-shared INTERFACE)
target_include_directories(B-shared INTERFACE .)
target_sources(B-shared INTERFACE foo.c)
add_library(B::shared ALIAS B-shared)
```

'B-shared' can be any name not already present in the CMake configuration. It
is recommended to provide an alias to clearly indicate that 'shared' is a
target in the B module.

#### D2) Header files

*NOTE:* Most DML files are imported and thus are considered to be header
 files. Only the top-level DML file, the one that contains the `device `
 statement, is considered to be a source file.

The `simics_add_module` function automatically provides an `INTERFACE` type
library, as described in C2, in addition to the `MODULE` type library; adding the
current module directory as target include directory. The following three
aliases can be used for this `INTERFACE` type library:
`<MODULE_NAME>::includes`, `<MODULE_NAME>::headers` and
`<MODULE_NAME>::imports`. They all work the same and differ only by name, to
provide some syntactic sugar matching the language used by the Simics module.

In module A:

```
target_link_libraries(A PRIVATE B::includes)
```

In module B: no changes needed as the `INTERFACE` type library is auto-generated.

It might be tempting to use the 'MODULE_NAME' directly in
`target_include_directories`, but this does not work.  Intel Simics Simulator modules 
must be fully isolated entities without runtime dependencies and thus are created with
`MODULE` library type. This prevents CMake from linking an Intel Simics Simulator module to
anything else.

### E) Makefile is generating files based on other files

In order to generate or copy files, use the standard `add_custom_command`
function. Either invoke the generator as part of `COMMAND` or use one of the many built-in commands of the `cmake` binary; for example `cat`:

```
add_custom_command(
  OUTPUT combined.c
  COMMAND ${CMAKE_COMMAND} -E cat a.c b.c > ${CMAKE_CURRENT_BINARY_DIR}/combined.c
  DEPENDS a.c b.c
)
```

See `cmake -E --help` for more details on what can be done with the built-in
`-E` option.

The `add_custom_command` can run multiple commands, see help for more details.

In order to trigger the custom command, make sure to add the generated files
(listed in `OUTPUT`) to the `SOURCES` parameter of the `simics_add_module`
function. Generated files should be output in the current binary
directory. `simics_add_module` searches first the current source directory and
then the current binary directory for Python and DML files.

If the generated files are not top-level DML files, but rather files imported from other DML files, then they should not be added to `SOURCES`. In this case combine `add_custom_target` with `add_dependencies` to make sure the custom command triggers. For example:

```
simics_add_module(test
  CLASSES a
  SOURCES a.dml
)
add_custom_command(
  OUTPUT b.dml
  DEPENDS ${generator} b.json
  COMMAND ${generator} ${CMAKE_CURRENT_SOURCE_DIR}/b.json -o b.dml
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
add_custom_target(b_dml DEPENDS b.dml)
add_dependencies(test b_dml)
```

### F) Copy Python files from other modules

Sometimes a module needs to re-use Python files from other modules as part of
its own simmod structure. This can be done using standard CMake functions, but
since it's fairly common a convenience method has been provided:

```
simics_copy_python_files(ICH10 FROM ICH FILES ich_commands.py ich_updaters.py)
```

See `simics_copy_python_files()` documentation in the [Reference
Manual](../refmanual-cmake/README.html#simics_copy_python_files) for more
details on usage.

</section>

## General notes / Tips and tricks

The CMake+Ninja combo is the best/fastest for CLI based development. More
powerful IDEs might leverage CMake differently. See CMake documentation for
details. Here are a couple of tips to get started.

### CMake vs. Ninja vs. CTest

Please note that these are three different and disjoint tools targeting three
different tasks:

- `cmake` ; the configuration tool that generates a build system. Typically you
  must re-run the configuration tool when the configuration changes.

- `ninja` ; the build system runner, can be invoked through `cmake
  --build`. Tracks changes in the dependencies and rebuilds only what's
  needed. Typically does *not* track changes in configuration.

- `ctest` ; the test runner. Distributed with CMake. CMake can generate the test
  system, similar to how it generates the build system, but it cannot run any
  tests. Does *not* track any changes to dependencies nor configuration, it's
  just a runner of the test system. Thus when adding one more test you
  typically need to invoke `cmake` to update the test system before you can run
  it with `ctest`.

### Access the built-in CMake help

`cmake --help` contains _everything_ you need. Especially `--help-command` to
learn more about each command. The bash shell supports tab-completion
out-of-the-box so it's easy to navigate. Of course, there is also the
[cmake.org](https://cmake.org/cmake/help/latest/index.html) website.

### Use built-in CMake commands to stay portable

The `cmake -E` utility provides portable ways to do many file operations such as `cat` 
and should be used over if-conditional code:

```
COMMAND ${CMAKE_COMMAND} -E cat a.py b.py > ${CMAKE_CURRENT_BINARY_DIR}/module_load.py
```

See `cmake -E --help` or the online documentation for more details.

### Keep things as local and "targeted" as possible

For example, and as the documentation also states, use the
`target_include_directories` instead of `include_directories` etc. The
`target_`-prefixed versions of their counterpart require one of the
`INTERFACE`, `PUBLIC` or `PRIVATE` keywords to define the scope of the command:

- `PRIVATE` ; only applies to the scope of the target
- `INTERFACE` ; applies to the scope of whoever uses/depends on the target
- `PUBLIC` ; applies to both 

Use target properties and avoid globals.

### Use the CMake API as intended

In the GNU-Make-driven Intel Simics Simulator build system, 
all flags added to `MODULE_CFLAGS`
was passed to the compilation step and all flags added to `MODULE_LDFLAGS` was
passed to the linking step. The CMake API provides functions at a finer
granularity for expressing these things:

- `target_include_directories()`
- `target_compile_definitions()`
- `target_compile_options()`
- `target_link_directories()`
- `target_link_libraries()`
- `target_link_options()`

The converter does not try to be clever and solve this problem; it just warns
about it. For clarity it is important that flags are passed using a combination
of these function calls. Please note that the user must also classify the scope
of the flags, i.e. `PRIVATE`, `INTERFACE` or `PUBLIC`.

### Use log-level to differentiate messages

CMake has defined a set of log-levels that should be used to differentiate
messages. The most important ones are:
* `FATAL_ERROR` ; CMake Error, stop processing and generation.
* `WARNING` ; CMake Warning, continue processing.
* (none) or `NOTICE` ; Important message printed to stderr to attract user's attention.
* `STATUS` ; The main interesting messages that project users might be interested in.
* `VERBOSE` ; Detailed informational messages intended for project users.
* `DEBUG` ; Detailed informational messages intended for developers working on the project itself as opposed to users who just want to build it.
* `TRACE` ; Fine-grained messages with very low-level implementation details. Messages using this log level would normally only be temporary and would expect to be removed before releasing the project.

The log-level to use can quickly be changed by passing `--log-level` to `cmake`.

See
[cmake.org:message()](https://cmake.org/cmake/help/latest/command/message.html) for
more details and more log levels.

### Tracing support

Correct use of message log-levels can improve debuggability, but should that
not be enough there is always the sledge hammer!

```shell
$ cmake --trace ...
```

This emits *a lot* of details to stdout about what cmake is doing when
processing the `CMakeLists.txt` files. To limit the output to only a few files of
interest, add the `--trace-source` option.

See [cmake.org:--trace](https://cmake.org/cmake/help/latest/manual/cmake.1.html#cmdoption-cmake-trace) for more details.

### Printf-debugging

CMake has built-in utility functions for printf-debugging:
* `cmake_print_properties()`
* `cmake_print_variables()`

See [cmake.org:CMakePrintHelpers](https://cmake.org/cmake/help/latest/module/CMakePrintHelpers.html) for details.
    
### Don't forget to build static libraries

In general, and hence by default, you want to build shared libraries as they
are easier to share among your build targets. But in the Intel Simics Simulator
module system, this breaks module isolation and must be avoided. 
So remember to pass `STATIC` when building helper
libraries, i.e.: 

```
add_library(<target> STATIC ... )
```

### Must build with -fPIC

By default the Intel Simics Simulator CMake configuration will set
`CMAKE_POSITION_INDEPENDENT_CODE` to `ON` which enables all `STATIC` library
builds to pass the `-fPIC` flag to the compiler. If this is not the case, it
can be selected with `target_compile_options(<target> PRIVATE -fPIC)`

### To share headers between modules, create an INTERFACE library

See D2 (for modules) or C2 (for common code) for more details.

Note that shared/imported .dml files counts as headers in this case.

### To share files, create a STATIC library

See D1 (for modules) or C1 (for common code) for more details.

In the GNU-Make-driven build system, code sharing was done by adding the
`other.c` file to the `SRC_FILES` variable in the `Makefile` of the module where
it was going to be used. Causing the same files to be built over and over
multiple times. Though this is still possible to do by explicitly providing the
absolute path to the file, it is not the recommended approach in CMake.

E.g. instead of doing this:
```
target_sources(versatile-devices
  PRIVATE ${SIMICS_PROJECT_DIR}/src/extensions/keycodes-common/keycodes.c)
```

you should be doing this:
```
target_link_libraries(versatile-devices
  PRIVATE keycodes-common ...)
```

where `keycodes-common` defines a `STATIC` library like this:
```
add_library(keycodes-common STATIC keycodes.c)
target_include_directories(keycodes-common PUBLIC . ...)
```

Please note that special flags and defines set by the target where this static
library is used, do not propagate into the build of the static library. Such
flags must also be set on the static library *or* the source files should be
compiled as part of the "user" target as described by (C2).  For example, to
build for `SIMICS_API=6` one must pass `target_compile_definitions(<target>
SIMICS_6_API)`.

### Set RPATH

The Intel Simics Simulator CMake package provides functions and targets to 
build Intel Simics simulator modules. These modules are meant to be dynamically 
loaded by the Intel Simics Simulator during a simulation run, and as
such can rely on the simulator to have loaded all the libraries a module depends on,
e.g. vtutils and python. External dependencies should be avoided, to allow
the module to be relocatable to other hosts.

Other binaries built by the same project, such as utilities and unit tests, might
require `RPATH` being set though and this can be done on-demand by each target
by setting the `BUILD_RPATH` property:

```
set_target_properties(generate-dml-from-xml
    PROPERTIES BUILD_RPATH $ORIGIN/libs:${SIMICS_LIB}:${SIMICS_SYS_LIB})
```

Passing explicit linker options also works, but should be avoided:
```
target_link_options(${MODULE_NAME} PRIVATE -Wl,-rpath,${SIMICS_SYS_LIB})
```

Projects that build mostly other things can setup RPATH globally in the
top-level `CMakeLists.txt` using the `CMAKE_BUILD_RPATH` cache variable. See
official CMake documentation for more details.

### Setting properties per source file

Sometimes a set of flags only apply to a subset, or just one, of the files that
make up a target. Setting the flags on the target might then be suboptimal as
it would affect everything built within that target. Here one could use
`set_source_files_properties` to alter properties per source file.

For example, if a compilation unit does not compile with `-O3` the optimization
can be reduced per unit.

Another example is, if `_FORTIFY_SOURCE=2` has been enabled then that requires
`__OPTIMIZE__ > 0` ; so if the current optimization level is to be reduced to
zero one must also undefine `_FORTIFY_SOURCE`:
```
set_source_files_properties(zuc.c
    PROPERTIES COMPILE_OPTIONS "-O0;-U_FORTIFY_SOURCE")
```

### Selecting compiler and build system runner

If GCC or Ninja are not in the user's `PATH`, they must be set up explicitly.

As described on the [cmake
wiki](https://gitlab.kitware.com/cmake/community/-/wikis/FAQ#how-do-i-use-a-different-compiler)
CMake will honor the `CC` and `CXX` environment variables if set, and store the
values into `CMAKE_<compiler>_COMPILER` cache variables. Setting the cache
variables directly from the command line using `-D` or indirectly through
presets is also an option.

To point out Ninja (or GNU Make) use the `CMAKE_MAKE_PROGRAM` cache variable.

As these are all cache variables, the settings are persistent between
invocations of `cmake` thus they only need to be passed once.

For example, to configure with explicit paths to GCC and Ninja:
```shell
$ cmake -S . -B bt -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM='/path/to/ninja/bin/ninja' -DCMAKE_C_COMPILER='/path/to/gcc/bin/gcc'
```

### Use presets

Settings can be cumbersome to pass as command line arguments each time a new
build tree is created. Though it's possible to write a simple shell script that
invokes `cmake` with the options you use, taking advantage of the built-in
presets feature is a much better alternative.

There are two files to consider:
- `CMakePresets.json` ; the main file intended to be submitted into the version control system
- `CMakeUserPresets.json` ; local presets only applicable to the current user. 
  This file should ideally not be submitted to the root of the repo, but rather the user 
  should copy it from somewhere to bootstrap the project.

Presets are fully supported by Integrated Development Environments (IDEs) such as 
Visual Studio* Code (VSCode) making it easy to select different presets through drop-down widgets.

The preset files can contain multiple presets and the presets can share common sections.

See [cmake.org:presets]( https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html) for more details.

### Don't forget to reconfigure the project when things are not working

CMake has two type of variables, the more traditional variables that are set
each time you run and the cached variables that are persistent between runs.

The cached variables can be inspected and changed by manipulating the
`CMakeCache.txt` file in the build tree root directly or using some tool or IDE
to do so.

The cached variables are updated if explicitly set using the `-D` command line
flag but there is also an option to ignore all cached variables during a
reconfiguration by using the `--fresh` flag.

### Setting the project build-id for all modules

The Intel Simics Simulator build-id for the modules built are 
controlled through the `SIMICS_BUILD_ID` cached variable.
Being a cached variable you have many options to set it:
- Set it using `-D` from CLI.
- Set it in your presets file.
- Set it from the top-level `CMakeLists.txt` file *before* importing the Intel Simics 
  Simulator CMake package.

## Differences between the old GNU-Make-based and the new CMake-based build systems
There are some important differences between the GNU-Make-based build system and the
CMake-based build system.  They affect how you set up the build specifications for
modules, and how you invoke the build and tests. 

* `EXTRA_MODULE_VPATH` vs. `target_link_libraries`
  * See (C) and (D) in the [Converting an existing GNU Makefile to CMakeLists.txt](README.html#converting-an-existing-gnu-makefile-to-cmakelists-txt) chapter.

* `MODULE_CFLAGS`, `MODULE_LDFLAGS`, ... vs. `target_compile_options`, `target_link_options`, ...
  * See [Use the CMake API as intended](README.html#use-the-cmake-api-as-intended) for more details

* `D=1` vs. `-DCMAKE_BUILD_TYPE=Debug` and `D=0` vs. `-DCMAKE_BUILD_TYPE=Release`
  * What used to be a command line argument to `make` is now a cached variable in
    CMake passed directly through `-D` or indirectly through presets when
    configuring the build tree. The setting then becomes persistent for the build
    tree. To change build type you have to reconfigure the build tree.

* `V=1` vs. `--verbose`
  * Unlike `D=1` for debug, enabling verbosity is still a command-line argument in CMake. It
    is not cached so it must be passed on each invocation of `cmake --build`.

* `make test` vs. invoking `ctest`
  * In CMake, the modules must first be built before they can be tested by
    CTest. And since CTest is just a test runner any changes to the modules or the
    CMake configuration will not trigger a rebuild ro reconfiguration. This must be
    handled manually by the user before re-running the tests.

* On Windows host, GNU Make can be invoked using `bin/make.bat`. 
  * With CMake, the expectation is that you have `cmake` in your `PATH`

## Things to note when working on Windows

### Paths

When passing paths to CMake, use forward slashes. This holds true for paths in the `CMakePresets.json` file and paths passed
directly using `-D` on the command line alike.

### Installing CMake, Ninja, and GCC

On Windows, building Intel Simics Simulator modules requires a GCC toolchain, CMake binary, and Ninja binary. 
This can all be downloaded from [winlibs.com](https://winlibs.com).

Here are the steps to take:
- Download the toolchain from winlibs with MinGW 14.1.0 and some other tools. Make sure to select the one built with POSIX threads.
- Unzip to some place that you like, here we use `C:\Users\dummy\winlibs-14.1.0` as an example.
- Install the Intel Simics Simulator via the Intel Simics Package Manager (ISPM)
- Create a project via the ISPM or via direct invocation of `project-setup.bat` (remember to pass `--with-cmake`).
    - ISPM does not add `--with-cmake`, so the project must be upgraded by invoking `bin\project-setup.bat --with-cmake --force` from within the project root.
- Open `cmd.exe` and enter the project (HINT: you can start a new command shell in the project you created using ISPM)
- Setup `PATH` to add the winlibs `bin` folder: `set PATH=C:\Users\dummy\winlibs-14.1.0\bin;%PATH%`
- Create or copy some CMake based module into the project (e.g. using `bin\project-setup.bat`)
- Configure a build tree using CMake: `cmake -S . -B bt -DCMAKE_BUILD_TYPE=Release -GNinja`
- Build everything: `cmake --build bt`

As noted above, the expectation is that CMake is in your path in order to do builds. 


## Coverage with GCOV/LCOV

To configure a build tree for generating GCOV coverage the following two
conditions must be met:

1. Add the following line to top-level `CMakeLists.txt` file:

    ```
    include(${SIMICS_BASE_DIR}/cmake/coverage.cmake) 
    ```

2. Configure a build tree with `USE_COVERAGE=1` and provide the paths 
   to the required tool binaries using the `LCOV`, `GCOV` and
   `GENHTML` cache variables if the binaries are not already in `PATH`. 
   See below for details.

The `coverage.cmake` adds the following targets:
- `init-coverage` ; to initialize the coverage collection
- `reset-coverage` ; to reset the coverage counters and start over
- `capture-coverage` ; to capture new/more coverage after running tests
- `generate-coverage-report` ; generate an HTML report
- `filter-coverage` ; internal target
- `generate-coverage-report-internal` ; internal target


### How it works

Configuring a build with `USE_COVERAGE=1` will enable the following flags for coverage:
```
add_compile_options(--coverage -g -Og)
add_link_options(--coverage -Wl,--dynamic-list-data)
```

Then a normal build will produce the GCOV instrumented binaries and a normal
run (or sequence of runs) of some tests/work-loads will generate the aggregated
coverage data.

Once coverage data has been generated, a coverage report can be generated via
the `generate-coverage-report` utility target. The HTML report is generated in `<build-tree>/coverage/index.html`

See example below for details and step-by-step on all of this.

### Configuration

The targets are enabled only if an LCOV binary is found in `PATH` or if the path
to LCOV is provided in the `LCOV` cache variable.

Assumptions and cache variables used by `coverage.cmake`:
- If LCOV is not in your `PATH`, then set the `LCOV` cache variable:
- If GCOV is not in your `PATH`, then set the `GCOV` cache variable. GCOV is typically found next to your GCC compiler and thus automatically found.
- If GENHTML is not in your `PATH`, then set the `GENHTML` cache variable. GENHTML is typically found next to the `lcov` binary in LCOV package and thus automatically found.

### Example

1. Configuration:

    ```shell
    $ cmake -S . -B btc -G Ninja -DCMAKE_BUILD_TYPE=Debug -DUSE_COVERAGE=1 -DLCOV=/usr/itm/lcov/1.16/bin/lcov
    ```

    Using `btc` here instead of `bt` to keep all the coverage isolated from normal
    builds, which is generally a good idea as the coverage build is very special
    and should only be used for collecting coverage.

2. Build the modules

    ```shell
    $ cmake --build btc
    ```

3. Initialize a baseline. This is important, and documented in the LCOV manual,
so that "to ensure that the percentage of total lines covered is correct even
when not all source code files were loaded during the test.":

    ```shell
    $ cmake --build btc --target init-coverage
    ```

4. Run some tests to generate coverage data.

    ```shell
    $ ctest --test-dir btc -j40
    ```

5. Generate the report

    ```shell
    $ cmake --build btc --target generate-coverage-report
    ```

6. Optionally (for internal use): generate coverage including the Intel Simics Simulator libraries:

    ```shell
    $ cmake --build btc --target generate-coverage-report-internal
    ```

7. Inspect the report in `btc/coverage/index.html`

8. To clear/reset the coverage counters between reports:

    ```shell
    $ cmake --build btc --target reset-coverage
    ```

## Sanitization with ASAN and UBSAN

Sanitizers are a great way to find additional bugs during run-time. A typical
flow would be to compile targets used in tests with ASAN and UBSAN, to get
tests with a higher bug coverage. More information about what type of bugs ASAN
and UBSAN can reveal can be found at <https://github.com/google/sanitizers>. Keep
in mind that sanitizers are not supported on Windows.

To add ASAN/UBSAN conditional compilation for an Intel Simics Simulator module, you 
need to call the `simics_add_sanitizers` function with your module name as the parameter.
Example:

```cmake
simics_add_module(my-device
  CLASSES  my_device
  SOURCES my-device.dml
  SIMICS_API 6
)
simics_add_sanitizers(my-device)
```

This will mark the `my-device` module as a module that should be compiled with
sanitizers when sanitizers are enabled. To enable compilation with sanitizers,
you should set the following CMake variables accordingly. Note that you can
enable each option individually or all at once.

| Variable                     | Enables                                    |
| ---------------------------- | ------------------------------------------ |
| USE_UBSAN                    | UndefinedBehaviorSanitizer                 |
| USE_ASAN                     | AddressSanitizer                           |
| ASAN_STACK_USE_AFTER_RETURN  | Stack-use-after-return (enables USE_ASAN)  |
| LSAN_SUPPRESSION_FILE        | A suppression file for LSAN for false positive memory leaks |
| LSAN_MALLOC_CONTEXT_SIZE     | LSAN Malloc context size |


These cache variables can be set during CMake configuration like so:

```shell
    $ cmake --build btc -DUSE_ASAN=1 -DUSE_UBSAN=1 -DASAN_STACK_USE_AFTER_RETURN=1
```
or enabled after configuration by using ccmake or similar CMake configuration
tool. The tests added with `simics_add_test()` would then have to be executed with
`ctest` for the sanitization to apply.

Keep in mind that these variables will have an affect for all CMake build types
(Debug, Release etc).

### ASAN Considerations

ASAN, on average, adds a 2x slowdown. It also adds a RAM overhead, along with
longer compilation times. It therefore makes sense to use ASAN when verifying
the model's correctness, such as running tests. It might not make as much sense
to use ASAN compiled models for the purpose of verifying software that
interfaces with the model.

It also makes sense to use high level of compilation optimization when compiling
sanitized modules, since that minimizes the slowdown. Compiling with debug
information is not mandatory for ASAN to trigger, but is needed to get useful
information in the stack trace.

`ASAN_STACK_USE_AFTER_RETURN` adds even more performance and memory overhead,
but as the name suggests can find use of pointers pointing to stack allocated
object after a function return. See <https://github.com/google/sanitizers/wiki/AddressSanitizerUseAfterReturn> for more information.

### Sanitized modules in CLI

If you want to interface with your Intel Simics Simulator modules compiled 
with sanitizers from the Intel Simics Simulator CLI, you can build the 
CMake `simics-asan` target. Example:

```shell
$ cmake --build bt --target simics-asan
```

This launches the Intel Simics Simulator with `LD_PRELOAD` and ASAN options
setup correctly, as required by all modules built with ASAN.

## Checking for dead DML methods

Using `simics_add_dead_methods_check()` on a target will add a post build step
that will check if the module contains any dead DML methods. This will only
happen if the `USE_DML_DEAD_METHODS_CHECK` option is enabled during
configuration.

By default, the check will only apply to source code that belongs to the current
module. However, the `simics_add_dead_methods_check()` has an argument
`EXTRA_SOURCES` that expands the scope of the dead methods analysis. This
argument can be used to scan common code.

Dead DML methods are methods that have been implemented but are never called.
One example would be an implemented `post_init()` method in an attribute, but the
attribute never instantiates the `post_init` template. This would result in a
`post_init` method that is never invoked.
