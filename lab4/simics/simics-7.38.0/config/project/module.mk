# -*- Makefile ; coding: utf-8 -*-

# © 2015 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

#
# Makefile for user projects.
#
#
# <add id="module_mk_inputs">
#
# This document describes all variables that can be set before
# including <const>MODULE_MAKEFILE</const> to compile a Simics module.
#
# <section id="module_mk_inputs_dev_makefile"/>
# <section id="module_mk_inputs_config"/>
# <section id="module_mk_inputs_call"/>
# <section id="module_mk_quoting"/>
#
# </add>
#
#
# <add id="module_mk_inputs_dev_makefile">
# <name>Inputs from the Module's Makefile</name>
#
# This section lists all module-specific variables. They should be set in the
# module's own <file>Makefile</file>.
#
# <insert id="module_mk_inputs_dev_makefile_list"/>
#
# </add>
#
# <!-- separate list to be able to include it in Model Builder UG as well -->
#
# <add id="module_mk_inputs_dev_makefile_list">
# <name>no name needed</name>
#:: doc module_mk_inputs_dev_makefile_list {{
# <dl>
#
#   <dt><const><idx>DMLC_FLAGS</idx></const></dt> <dd>(optional) Module-specific
#   parameters for the DML dependency generator and compiler</dd>
#
#   <dt><const><idx>EXTRA_MODULE_VPATH</idx></const></dt> <dd>(optional) space-
#   or colon-separated list of module names (optionally followed by a
#   subdirectory: <tt><v>modulename</v>/some/path</tt>). The source
#   code directory of this module (or the corresponding subdirectory)
#   will automatically be added to <cmd>make</cmd>'s VPATH. The
#   directories will also be added to the include search path for DML
#   and C modules (using the <tt>-I</tt> options to the compilers).
#
#   The current project as well as any installed Simics packages will be
#   searched for the module source code. Note that only the first matching
#   source code directory found will be used, allowing the project to
#   override the version of an installed package.</dd>
#
#   <dt><const><idx>EXTRA_OBJ_FILES</idx></const></dt> <dd>(optional)
#   Additional object files to link into the module. The module build
#   will be dependent on these files, so additional rules can be
#   provided in the module's <file>Makefile</file> to build these
#   files before linking them.</dd>
#
#   <dt><const><idx>EXTRA_VPATH</idx></const></dt> <dd>(optional)
#   Directories to add to VPATH when building.</dd>
#
#   <dt><const><idx>IFACE_FILES</idx></const></dt> <dd>Header files to
#   compile for an interface module. If <const>IFACE_FILES</const>
#   contains <file><v>file-name</v>.h</file>, in a Simics module named
#   <tt><v>module-name</v></tt>, then the Python module
#   <tt>simmod.<v>module_name</v>.<v>file_name</v></tt> will be
#   created, containing the Python bindings for all interfaces and
#   types defined in <file><v>file-name</v>.h</file>. The filenames in
#   <const>IFACE_FILES</const> must not include any directory
#   components; if any files are to be found in another directory,
#   then that directory must be included in <tt>VPATH</tt> so the file
#   is found correctly; e.g., by setting the
#   <tt>EXTRA_MODULE_VPATH</tt> variable.
#
#   See also section <cite>Restrictions</cite> in
#   <cite>Defining New Interface Types</cite>
#   in Model Builder User's Guide for restrictions and usage
#   of Python bindings.</dd>
#
#   <dt><const><idx>MODULE_CFLAGS</idx></const></dt> <dd>(optional)
#   Parameters for the C and C++ preprocessor and compiler</dd>
#
#   <dt><const><idx>MODULE_CLASSES</idx></const></dt>
#   <dd>Space-separated list of configuration classes that this module
#   registers. This information is used by <fun>SIM_get_class()</fun>
#   to determine that this module should be automatically loaded when
#   one of the listed classes is requested.</dd>
#
#   <dt><const><idx>MODULE_COMPONENTS</idx></const></dt>
#   <dd>Space-separated list of components that this module
#   registers.</dd>
#
#   <dt><const><idx>MODULE_LDFLAGS</idx></const></dt> <dd>(optional)
#   Module-specific flags for the C and C++ linker.  Any particular
#   library linking (such as -lpthread, -lm, or -L to give paths to
#   search for libraries) can be included here. If the module has
#   dependencies on <const>LD_MODULE_PATH</const>, it is possible to
#   specify <arg>-rpath</arg> so that the module will contain the
#   paths that were valid at compilation time.</dd>
#
#   <dt><const><idx>PYTHON_FILES</idx></const></dt> <dd>Space-separated list of
#   Python source files to include. These Python files will be
#   copied and potentially compiled (see <const>COMPILE_PYC</const>) and placed
#   in a Python package specific to the module. If
#   a module <tt>my-module</tt> includes the file <file>file.py</file>
#   in <const>PYTHON_FILES</const>, then the resulting Python module
#   will be available as <tt>simmod.my_module.file</tt> in Simics. Two
#   filenames get special treatment if included in
#   <const>PYTHON_FILES</const>:
#   <ul><li><tt>simics_start.py</tt> is automatically imported while
#           Simics is launched.</li>
#       <li><tt>module_load.py</tt> is imported by Simics when the
#           Simics module is loaded.</li></ul>
#
#   The names <tt>checkpoint_update.py</tt> and <tt>__init__.py</tt>
#   are reserved for future use, and not allowed in the list of files.</dd>
#
#   <dt><const><idx>MODULE_USER_VERSION</idx></const></dt> <dd>(optional)
#   User supplied free-text string describing the module version. The
#   version string is available in Simics even without loading the module,
#   through the <cmd>list-modules</cmd> command or the
#   <fun>SIM_get_all_modules</fun> API function.</dd>
#
#   <dt><const><idx>SIMICS_API</idx></const></dt> <dd>(optional)
#   Simics API to use when compiling the module. See the <cite>Simics
#   Migration Guide</cite> for a description on how to compile old
#   modules with a new Simics version. Valid API settings are listed
#   in <file><v>[simics]</v>/<v>[host]</v>/include/api-versions.mk</file>.</dd>
#
#   <dt><const><idx>SRC_FILES</idx></const></dt> <dd>Source files to
#   compile in the module. C source file names must end in
#   <file>.c</file>; C++ source file names must end in
#   <file>.cc</file>, <file>.cpp</file>, <file>.cxx</file>
#   or <file>.C</file> (the last not allowed on Windows).
#   DML file names must have a <file>.dml</file>
#   suffix. Any <file>.py</file> files should be listed
#   in the <const>PYTHON_FILES</const> variable.</dd>
#
#   <dt><const><idx>SRC_IMAGES</idx></const></dt> <dd>(optional)
#   Images to copy directly in the <file>images</file> subdirectory in
#   $(TARGET_DIR)</dd>
#
#   <dt><const><idx>SYSTEMC</idx></const></dt> <dd>If set to 'yes', provides
#   compiler and linker flags that allow building the SystemC Library adapter
#   and SystemC devices. See the <cite>SystemC Library Programming Guide</cite>
#   for more information.</dd>
#
#   <dt><const><idx>SYSTEMC_CORE_CFLAGS</idx></const></dt>
#   <dd>(optional) Parameters for the C and C++ preprocessor and
#   compiler when using user-specified SystemC source.</dd>
#
#   <dt><const><idx>SYSTEMC_CORE_LDFLAGS</idx></const></dt>
#   <dd>(optional) SystemC core specific flags for the C and C++
#   linker. Any particular library linking can be included here.</dd>
#
#   <dt><const><idx>SYSTEMC_MODULE_CFLAGS</idx></const></dt>
#   <dd>(optional) Parameters for the C and C++ preprocessor and
#   compiler when compiling SystemC modules.</dd>
#
#   <dt><const><idx>THREAD_SAFE</idx></const></dt> <dd>If set to
#   <tt>yes</tt>, declare that the module is thread-safe.</dd>
#
#   <dt><const><idx>USE_CC_API</idx></const></dt> <dd>It can be set
#   to a specific version to select which version of C++ Device API
#   to use. Current supported versions are '1' and '2'. See the
#   <cite>C++ Device API Programming Guide</cite> for more information.</dd>
#
#   <dt><const><idx>SUPPRESS_DEVICE_INFO</idx></const></dt> <dd>If set to
#   'yes', suppress output of the <file>.xml</file> device info file
#   by the DML compiler.</dd>
#
#   <dt><const><idx>COMPILERS</idx></const></dt> <dd>An optional
#   list of compatible compilers, in order of preference. The allowed list
#   element values are <tt>gcc</tt> and <tt>cl</tt>, for MinGW and Visual
#   Studio respectively. On platforms other than Windows, <tt>cl</tt> is
#   ignored.</dd>
#
#   <dt><const><idx>COMPILE_PYC</idx></const></dt> <dd>If this is set
#   to <tt>1</tt>, then the files listed in <const>PYTHON_FILES</const>
#   are compiled, not copied.</dd>
# </dl>
#
# }}
# </add>
#
# 
# <add id="module_mk_inputs_config">
# <name>Configuration Parameters</name>
#
# This section lists all variables that are configured by
# <file>project-setup</file> or other scripts, generally identical for all
# modules. It is probably a good idea to reuse <file>config.mk</file> and its
# related files to set these.
#
# <dl>
#
#   <dt><const><idx>BLD_CFLAGS</idx></const></dt> <dd>Parameters for
#   the C compilation step (but not the dependency generation)</dd>
#
#   <dt><const><idx>BLD_CXXFLAGS</idx></const></dt> <dd>Parameters for
#   the C++ compilation step (but not the dependency generation)</dd>
#
#   <dt><const><idx>BLD_OPT_CFLAGS</idx></const></dt> <dd>Parameters
#   enabling optimization for the C compiler</dd>
#
#   <dt><const><idx>CC</idx></const></dt> <dd>C compiler</dd>
#
#   <dt><const><idx>CCLD</idx></const></dt> <dd>C compiler used for
#   linking</dd>
#
#   <dt><const><idx>CCLDFLAGS_DYN</idx></const></dt> <dd>Dynamic
#   linking flags for the C compiler used for linking</dd>
#
#   <dt><const><idx>CFLAGS</idx></const></dt> <dd>Parameters for the C
#   preprocessor and compiler</dd>
#
#   <dt><const><idx>CPP</idx></const></dt> <dd>C preprocessor</dd>
#
#   <dt><const><idx>CXX</idx></const></dt> <dd>C++ compiler</dd>
#
#   <dt><const><idx>CXX_INCLUDE_PATHS</idx></const></dt> <dd>Simics
#   C++ API include paths that are package specific, such where to
#   find Model Builder features and C++ API includes, but also includes
#   for specific packages. It is a :-separated list on Linux
#   and ;-separated on Windows.</dd>
#
#   <dt><const><idx>CXXFLAGS</idx></const></dt> <dd>Parameters for the
#   C++ preprocessor and compiler</dd>
#
#   <dt><const><idx>DEP_CC</idx></const></dt> <dd>C dependency generator</dd>
#
#   <dt><const><idx>DEP_CFLAGS</idx></const></dt> <dd>Parameters for
#   the C dependency generation step</dd>
#
#   <dt><const><idx>DEP_CXX</idx></const></dt> <dd>C++ dependency generator</dd>
#
#   <dt><const><idx>DEP_CXXFLAGS</idx></const></dt> <dd>Parameters for
#   the C++ dependency generation step</dd>
#
#   <dt><const><idx>DEP_DOUBLE_TARGETS</idx></const></dt> <dd>If the
#   C/C++ preprocessor supports it, generate double targets
#   dependencies, where both the dependency file and the object file
#   depends on all include files.</dd>
#
#   <dt><const><idx>DISAS</idx></const></dt> <dd>Disassembler</dd>
#
#   <dt><const><idx>DML_INCLUDE_PATHS</idx></const></dt> <dd>DML
#   compiler include paths that are package specific, such where to
#   find Model Builder includes, but also includes for specific packages.
#   It is a :-separated list on Linux and ;-separated on Windows.</dd>
#
#   <dt><const><idx>DMLC</idx></const></dt> <dd>The DML Compiler</dd>
#
#   <dt><const><idx>DMLC_OPT_FLAGS</idx></const></dt> <dd>Optimization
#   parameters for the DML compiler. Enabling debug mode typically
#   changes these flags from optimized build to debug build.</dd>
#
#   <dt><const><idx>HOST_TYPE</idx></const></dt> <dd>Simics host type
#   to use (for tools) and to compile for (target directory)</dd>
#
#   <dt><const><idx>INCLUDE_PATHS</idx></const></dt> <dd>C and C++
#   preprocessor and compiler include paths that are package specific,
#   such where to find Model Builder includes, but also includes for
#   specific packages. It is a :-separated list on Linux
#   and ;-separated on Windows.</dd>
#
#   <dt><const><idx>LDFLAGS</idx></const></dt> <dd>Parameters for the C and C++
#   linker</dd>
#
#   <dt><const><idx>LIBS</idx></const></dt> <dd>Libraries to link with
#   the module, including <file>libvtutils</file> and
#   <file>libsimics-common</file></dd>
#
#   <dt><const><idx>OBJEXT</idx></const></dt> <dd>Object files extension.</dd>
#
#   <dt><const><idx>PYTHON</idx></const></dt> <dd>Native Python executable</dd>
#
#   <dt><const><idx>PYTHON_BLD_CFLAGS</idx></const></dt> <dd>Build
#   flags for compiling C code linking with the Python library</dd>
#
#   <dt><const><idx>PYTHON_INCLUDE</idx></const></dt> <dd>Include
#   directives for Python header files</dd>
#
#   <dt><const><idx>SIMICS_BASE</idx></const></dt> <dd>Absolute path to the
#   Simics base package. If the path contains spaces or other special
#   characters, these are quoted to be understood by the native shell
#   (<tt>sh</tt> or <tt>CMD.EXE</tt>, depending on operating
#   system). This means that <cmd>make</cmd> itself may not be able to
#   understand the variable, so it can not reliably be used in a
#   prerequisite or as a parameter to a path manipulation <cmd>make</cmd>
#   function.</dd>
#
#   <dt><const><idx>SIMICS_MAJOR_VERSION</idx></const></dt> <dd>Major version of
#   Simics (such as "4.4")</dd>
#
#   <dt><const><idx>SIMICS_PROJECT</idx></const></dt> <dd>Path, absolute or
#   relative, to the Simics project</dd>
#
#   <dt><const><idx>SO_SFX</idx></const></dt> <dd>Shared module extension.</dd>
#
#   <dt><const><idx>USER_BUILD_ID</idx></const></dt> <dd>Build-id
#   value compiled in the module</dd>
#
#   <dt><const><idx>COMPILER</idx></const></dt> <dd>The preferred compiler,
#   used to build if the module is compatible with it, specified by its
#   <const>COMPILERS</const> setting. Allowed values on Windows are
#   <tt>gcc</tt> and <tt>cl</tt>, for MinGW and Visual Studio, respectively. On
#   other platforms, only <tt>gcc</tt> is allowed. If the preferred compiler is
#   not compatible with the module, then the first compiler listed in
#   <const>COMPILERS</const> is used. Modules are assumed to require
#   <tt>gcc</tt> if <const>COMPILERS</const> is empty.</dd>
#
# </dl>
#
# <section id="module_mk_inputs_config_win"/>
#
# </add>
#
# <add id="module_mk_inputs_config_win">
# <name>Windows-only</name>
#
# <dl>
#
#   <dt><const><idx>LINK</idx></const></dt> <dd>The Visual Studio linker</dd>
#
#   <dt><const><idx>MSVC</idx></const></dt> <dd>If 'yes', a Microsoft Visual
#   Studio compiler has been selected</dd>
#
#   <dt><const><idx>VCVARSPREFIX</idx></const></dt> <dd>Prefix to add in front
#   of commands to make sure the Visual Studio variables have been set</dd>
#
# </dl>
#
# </add>
#
# <add id="module_mk_inputs_call">
# <name>Module-specific Inputs</name>
#
# This section lists the variables that <file>module.mk</file> expects to be
# able to compile a given module.
#
# <dl>
#
#   <dt><const><idx>MOD_MAKEFILE</idx></const></dt> <dd>Path, absolute or
#   relative, to the module's Makefile.</dd>
#
#   <dt><const><idx>SRC_BASE</idx></const></dt> <dd>Directory, absolute or
#   relative, containing the module directory, so that
#   <const>$(SRC_BASE)/$(TARGET)</const> points at the absolute module
#   directory</dd>
#
#   <dt><const><idx>TARGET</idx></const></dt> <dd>Name of the
#   directory containing the module so that
#   <const>$(SRC_BASE)/$(TARGET)</const> points at the module
#   directory. It is also the name the module will have once
#   compiled.</dd>
#
#   <dt><const><idx>TARGET_DIR</idx></const></dt> <dd>Directory, absolute or
#   relative, in which the final module and associated files will be
#   put.</dd>
#
# </dl>
#
# </add>
#
# <add id="module_mk_quoting">
# <name>How spaces in paths are handled</name>
#
# The build system will work even if a simics package or the project
# itself is located in a path that contains spaces. This will cause
# some <cmd>make</cmd> variables to contain spaces, but in most cases,
# the author of a module Makefile does not need worry about this.
# Some new restrictions apply if one wishes to use a makefile in a
# system where paths contains spaces; however, these restrictions are
# met when variables are used normally. Different restrictions apply
# to different variables:
#
# <ul>
#
#   <li> Any space characters are quoted in the variable
#     <const>SIMICS_BASE</const>. On Windows, this means
#     that any space character is surrounded by double quotes. This
#     means that the native shell will decode the variables correctly,
#     but that <cmd>make</cmd> itself will not. I.e., the variables
#     can be reliably used in invocations of the
#     <const>$(shell)</const> function and in command invocations, but
#     not in prerequisites, include statements or file name
#     manipulation functions such as <const>$(wildcard)</const>. This
#     is typically not a problem, since the variables mostly are used
#     to invoke commands.
#
#     The same quoting is applied to variables such as
#     <const>PYTHON</const> that contain commands or scripts from
#     packages. One exception is <const>MODULE_MAKEFILE</const>, whose
#     quoting instead is tuned for
#     <cmd>make</cmd>. <const>MODULE_MAKEFILE</const> can be
#     understood by make directives, but not necessarily by the native
#     shell.</li>
#
#   <li> If the project path contains a space, all related variables
#     (<const>SIMICS_PROJECT</const>, <const>SRC_BASE</const>,
#     <const>TARGET_DIR</const> and <const>MOD_MAKEFILE</const>) will
#     use <file>../../../..</file> (<file>..\..\..\..</file> on
#     Windows) to denote the project path. This way, the paths are
#     guaranteed to never contain any special characters; hence they
#     can be understood both by <cmd>make</cmd> and by the native
#     shell. There are two known situations where this can cause
#     problems:
#     <ul>
#
#       <li>On Windows, forward slashes in relative paths do not work
#       when invoking <file>.bat</file> files. Hence, commands such as
#       <file>$(SIMICS_PROJECT)/simics.bat</file> will not work if a
#       project contains spaces. The easiest workaround is to
#       use backslashes as directory separator on Windows.</li>
#       <li>Some commands may require absolute paths as arguments. For
#       example, a command might change directory before using a path
#       argument. In this case, the easiest workaround is to call the
#       make function <const>realpath</const> or
#       <const>abspath</const> on the path before passing it as an
#       argument. This will also require additional quoting. For example,
#       <pre><small>$(shell cd C:\TEMP &amp; $(SIMICS_PROJECT)\simics.bat)</small></pre>
#       might have to be rewritten as follows to work in projects with spaces:
#       <pre><small>$(shell cd C:\TEMP &amp; "$(realpath $(SIMICS_PROJECT))\simics.bat")</small></pre></li>
#    </ul>
#   </li>
#
#   <li> Modules from <const>EXTRA_MODULE_VPATH</const> get special
#     treatment if they are found in a path with spaces: If a module
#     directory found in a project containing a space, the directory
#     will be added to <const>VPATH</const> as a relative path,
#     starting with <file>../../../..</file>. If a module directory
#     instead is found in a package containing a space, the module
#     directory will instead be copied into the build directory, and
#     the copy will be added to <const>VPATH</const>. This is done
#     because <cmd>make</cmd> does not support paths with space in
#     <const>VPATH</const>.</li>
#
# </ul>
# </add>

$(eval _makequote=$(value _MAKEQUOTE))
$(eval _shellquote=$(call _noinvalid,$(1))$(value _SHELLQUOTE))

_6_API := $(filter 6,$(SIMICS_API))

all:

ifneq ($(_IS_WINDOWS),)
export SHELL=cmd.exe
endif

_BATCH_SUFFIX := $(if $(_IS_WINDOWS),.bat,)
_EXE_SUFFIX := $(if $(_IS_WINDOWS),.exe,)

# Signal an error if a path contains unsupported characters. A
# character is deemed unsupported if escaping for it works differently
# in command invocations and in make's file references.
_noinvalid=$(if $(filter & ' < > $$ | ",$(1)),$(error Invalid characters in filename $(1)))
# ' this quote is here to make Emacs syntax highlighting happy.

# rm_or_move: On Windows, returns a shell command that gets the arg (a
#   file) out of the way. This must be done for a file that is not
#   writable, typically because it is open.
# _quote_test_expected: used for unit test below
ifeq ($(_IS_WINDOWS),)
_rm_or_move:=
_quote_test_expected=a"("\ ")"b%\ \ c/d\\e
else
_rm_or_move=$(if $(wildcard $(1)),$(PYTHON) $(SIMICS_BASE)\\scripts\\build\\rm_or_move.py $(1))
_quote_test_expected=a"( )"b%%"  "c\d\e
endif

ifneq ($(call _makequote,abc def\g),abc\ def$(if $(_IS_WINDOWS),\,\\)g)
$(error unit test failed)
endif

_quote_test_str=a( )b%  c/d\e
ifneq ($(call _shellquote,$(_quote_test_str)),$(_quote_test_expected))
$(error unit test failed)
endif
_M_SIMICS_BASE := $(call _makequote,$(_RAW_SIMICS_BASE))

# The separator between paths in path lists such as VPATH and *_INCLUDE_PATHS
_PATHSEP:=$(if $(_IS_WINDOWS),;,:)

# We need to substitute : for space in order to be able to use Make's
# functions that operate on words. So first we need to substitute
# actual spaces in paths for something else. We pick -//- because if
# it would already occur in the list, it can safely be replaced with
# -/- to avoid converting it back to a space in the end.
_ESC:=-//-
# Given $(1) as a _PATHSEP-separated list of paths (such as INCLUDE_PATH),
# convert it to a space-separated list where each path is prefixed
# with $(2) and postfixed with $(3), and where original spaces and
# backslashes are (shell-)escaped
_mangle_path_list = $(foreach p,$(subst $(_PATHSEP), ,$(subst $() ,$(_ESC),$(subst -//-,-/-,$(call _noinvalid,$(1))$(1)))),$(2)$(call _shellquote,$(subst $(_ESC), ,$(p)))$(3))

ifneq ($(call _mangle_path_list,a$(_PATHSEP)b,x,y).\
$(call _mangle_path_list,$(_PATHSEP)$(_PATHSEP)a$(_PATHSEP)$(_PATHSEP)b$(_PATHSEP)$(_PATHSEP),x,y).\
$(call _mangle_path_list,a a\$(_PATHSEP)b-//-b),\
xay xby. xay xby. $(if $(_IS_WINDOWS),a" "a\ b-\-b,a\ a\\ b-/-b))
$(error unit test failed)
endif

_SIMICS_START_SCRIPT:=simics$(_BATCH_SUFFIX)

ABS_MODULE_DIR:=$(SRC_BASE)/$(TARGET)

ifeq ($(SYSTEMC),yes)
USE_CC_API = 2
BLD_CFLAGS := $(filter-out -std=gnu23,$(BLD_CFLAGS))
MINGW_BLD_CFLAGS := $(filter-out -std=gnu23,$(MINGW_BLD_CFLAGS))
MINGW_BLD_OPT_CFLAGS := $(filter-out -std=gnu23,$(MINGW_BLD_OPT_CFLAGS))
PYTHON_BLD_CFLAGS := $(filter-out -std=gnu23,$(PYTHON_BLD_CFLAGS))
endif

# TODO(ah): Handle this gracefully now that v1 has been removed
ifdef USE_CC_API
  ifeq (yes,$(strip $(USE_CC_API)))
    $(error USE_CC_API selects which version of C++ Device API to use. It should be either 1 or 2)
  else
    ifeq (1,$(strip $(USE_CC_API)))
      SRC_FILES += simics-api.cc
      EXTRA_MODULE_VPATH += c++-api-v1
    else
      ifneq (2,$(strip $(USE_CC_API)))
        $(error Unsupported value for USE_CC_API($(USE_CC_API)). Should be either 1 or 2)
      else
        SRC_FILES += after.cc conf-class.cc conf-object.cc event.cc utility.cc
        # This tech-preview feature requires c++17
        ifeq (yes,$(strip $(USE_CC_MODELING_API)))
          SRC_FILES += bank.cc bank-instrumentation-subscribe-connection.cc fmt/format.cc field.cc hierarchical-object.cc register.cc
          MSVC_BLD_CXXFLAGS += /std:c++17
        endif
        # The memory management feature is optional and by default enabled
        ifneq (no,$(strip $(USE_CC_MEMORY_MANAGEMENT)))
          ifneq ($(SYSTEMC),yes)
            ifeq ($(D),1)
              # SIMICS-21235 std::string constructor may be inlined and
              # bypassing the overloaded new. Use it only in the DEBUG build
              SRC_FILES += overload-new-and-delete.cc
            endif
          endif
        endif
        # Need put c++-api ahead since both c++-api and SCL has event.cc
        EXTRA_MODULE_VPATH := c++-api:$(EXTRA_MODULE_VPATH)
        CCLD := $(CXX)
      endif
    endif
  endif
endif

# Whether we recognize .C as suffix for C++ source files.
# It is not done on Windows because %.C may match files ending in ".c";
# see bug 22508.
_ALLOW_DOT_C:=$(if $(_IS_WINDOWS),,yes)

_UNSUPPORTED_SRC_FILES:=$(filter-out %.cc %.cpp $(if $(_ALLOW_DOT_C),%.C) %.cxx %.c %.dml,$(SRC_FILES))
ifneq ($(_UNSUPPORTED_SRC_FILES),)
 $(error SRC_FILES contains files with unsupported file suffixes: $(_UNSUPPORTED_SRC_FILES))
endif

_SUPPORTED_COMPILERS := $(if $(_IS_WINDOWS),gcc cl,gcc)

ifneq ($(COMPILERS),)
  _COMPILERS := $(filter $(_SUPPORTED_COMPILERS),$(COMPILERS))
  $(if $(_COMPILERS),,$(error No supported compiler '$(_SUPPORTED_COMPILERS)' among given compilers '$(COMPILERS)'))
  _COMPILER := $(if $(findstring $(COMPILER), $(_COMPILERS)),$(COMPILER),$(firstword $(_COMPILERS)))
else
  _COMPILER:=gcc
endif
ifneq ($(findstring cl, $(_COMPILER)),)
  CC:=cl
endif

# Backwards compatibility: honour legacy MSVC choice at top level
ifeq ($(CC),cl)
MSVC := yes
CXX := $(CC)
BLD_CFLAGS += $(MSVC_BLD_CFLAGS)
BLD_OPT_CFLAGS = $(MSVC_BLD_OPT_CFLAGS)
BLD_CXXFLAGS += $(MSVC_BLD_CXXFLAGS)
LDFLAGS += $(MSVC_LDFLAGS)
LIBS += $(MSVC_LIBS)
else
BLD_CFLAGS += $(MINGW_BLD_CFLAGS)
BLD_OPT_CFLAGS = $(MINGW_BLD_OPT_CFLAGS)
BLD_CXXFLAGS += $(MINGW_BLD_CXXFLAGS)
LDFLAGS += $(MINGW_LDFLAGS)
LIBS += $(MINGW_LIBS)
endif

ifeq ($(SYSTEMC),yes)
    include $(_M_SIMICS_BASE)/config/project/systemc.mk
endif

ifeq ($(COFLUENT),yes)
    include $(_M_SIMICS_BASE)/config/project/cofluent.mk
endif

ifneq ($(EXTRA_MODULE_VPATH),)
EXTRA_MODULE_VPATH:=$(shell $(PYTHON) \
    $(SIMICS_BASE)/scripts/build/expand_module_vpath.py \
        $(if $(SIMICS_PACKAGE_LIST),--package-list $(SIMICS_PACKAGE_LIST)) \
	--module-dirs $(call _shellquote,$(MODULE_DIRS)) \
	--project $(SIMICS_PROJECT) \
        $(call _shellquote,$(EXTRA_MODULE_VPATH)) \
        $(foreach dir,$(ABS_MODULE_DIR) $(EXTRA_VPATH), \
	  --check-path-length $(dir)))
ifeq ($(EXTRA_MODULE_VPATH),)
$(error Expansion of EXTRA_MODULE_VPATH failed)
endif
endif

# EXTRA_VPATH_IN_SIMICS is used for backward compatibility only. Use
# EXTRA_MODULE_VPATH instead
VPATH_IN_SIMICS=$(foreach vp,$(subst :, ,$(EXTRA_VPATH_IN_SIMICS)),	\
	$(shell $(call _shellquote,$(abspath $(SIMICS_PROJECT)/bin/lookup-file)) -d -f $(vp)))

override VPATH := . $(ABS_MODULE_DIR) $(EXTRA_VPATH) $(VPATH_IN_SIMICS)	\
	$(EXTRA_MODULE_VPATH) $(VPATH)

ifeq ($(_IS_WINDOWS),)
# Only : is allowed as path separator. In 4.6, space worked but was
# undocumented, while 4.8 is more strict. We try to detect this and yield a
# warning, because this seems to be a popular problem when migrating to 4.8.
# In order to manually extend INCLUDE_PATHS, please do:
#   INCLUDE_PATHS := $(INCLUDE_PATHS):/whatever/path
# Also, remember to use ; as separator when building on Windows.
$(foreach v,INCLUDE_PATHS DML_INCLUDE_PATHS CXX_INCLUDE_PATHS,\
  $(if $(findstring $() /,$($(v))),$(warning Only colons can be used to separate paths in $(v). It seems that space has been used as a separator: $($(v)))))
endif

# Handle different API versions
SIMICS_API:=$(strip $(SIMICS_API))
include $(_M_SIMICS_BASE)/$(HOST_TYPE)/include/api-versions.mk

ifeq (latest,$(SIMICS_API))
SIMICS_API:=$(LATEST_API_VERSION)
endif

ifneq ($(SIMICS_API),$(LATEST_API_VERSION))
API_WARN:=api_warning
SIMICS_API_NAME:=$(SIMICS_API)
endif

ifeq (,$(SIMICS_API))
API_WARN:=api_warning
SIMICS_API_NAME:=default ($(DEFAULT_API_VERSION))
SIMICS_API:=$(DEFAULT_API_VERSION)
endif

ifneq ($(filter $(API_VERSIONS),$(firstword $(SIMICS_API))), $(SIMICS_API))
$(error Unknown Simics API version $(SIMICS_API))
endif

ifeq ($(SIMICS_API),6)
BLD_CFLAGS := $(filter-out -DNDEBUG,$(BLD_CFLAGS))
endif

# Since API version 5, neither VPATH nor EXTRA_VPATH are added to the include
# search path for C, CXX or DMLC. See bug 18937 for more information.
EXTRA_FLAG_PATHS += .				\
		    $(ABS_MODULE_DIR)		\
		    $(VPATH_IN_SIMICS)		\
		    $(EXTRA_MODULE_VPATH)

# If the user hasn't given a visibility option we default it to hidden.
# GCC_VISIBILITY is only set if the compiler is GCC.
ifeq ($(findstring -fvisibility,$(MODULE_CFLAGS)),)
MODULE_CFLAGS += $(GCC_VISIBILITY)
endif

_COMMON_CFLAGS = -DHAVE_MODULE_DATE -DSIMICS_$(subst .,_,$(SIMICS_API))_API    \
		 $(call _mangle_path_list,$(INCLUDE_PATHS),-I)		       \
		 $(patsubst %, -I%,$(subst $(_PATHSEP), ,$(EXTRA_FLAG_PATHS)))

CFLAGS += $(MODULE_CFLAGS) $(_COMMON_CFLAGS)
CFLAGS += -I$(DMLC_DIR)/dml/include

CXXFLAGS += $(MODULE_CFLAGS) $(_COMMON_CFLAGS)

_IFACE_CFLAGS = $(PYTHON_BLD_CFLAGS) $(_COMMON_CFLAGS)

DMLC_FLAGS += $(DMLC_OPT_FLAGS)

LDFLAGS += $(MODULE_LDFLAGS)

DMLC_FLAGS += --simics-api=$(SIMICS_API)

CXXFLAGS += $(call _mangle_path_list,$(INCLUDE_PATHS),-I,/$(SIMICS_API))
CXXFLAGS += $(call _mangle_path_list,$(CXX_INCLUDE_PATHS),-I,/$(SIMICS_API))

SRC_FILES:=$(strip $(SRC_FILES))

# DML_FILES used to contain the DML source files, but today SRC_FILES can do
# that as well, so it isn't documented any more
DML_FILES += $(filter %.dml,$(SRC_FILES))
DML_FILES:=$(strip $(DML_FILES))

ifeq ($(MSVC),yes)
ifneq (,$(strip $(DML_FILES)))
$(error Modules containing DML files cannot use the MSVC compiler)
endif
ifneq (,$(strip $(IFACE_FILES)))
$(error Interface modules cannot use the MSVC compiler)
endif
endif

IFACE_FILES:=$(strip $(IFACE_FILES))

ifneq (,$(IFACE_FILES))

ifneq (,$(filter-out %.h, $(IFACE_FILES)))
$(error IFACE_FILES must be *.h: $(filter-out %.h, $(IFACE_FILES)))
endif

_PYIFACE_C_FILES += $(patsubst %.h,pyiface-%-wrappers.c,$(notdir $(IFACE_FILES)))
_PYIFACE_C_FILES += $(patsubst %.h,pyiface-%-trampolines.c,$(notdir $(IFACE_FILES)))
_IFACE_INTERFACE_LISTS := $(patsubst %.h,pyiface-%-interface-list,$(IFACE_FILES))

pyiface-%-wrappers.c pyiface-%-trampolines.c pyiface-%-interface-list: pyifaces-%.i pyifaces-%.t $(PYWRAPGEN_BIN)
	$(info PYWRAP  $<)
	$(PYWRAPGEN) -W $(SIMICS_BASE)/$(HOST_TYPE)/bin/pywrap/ 	     \
	        $(foreach V,						     \
		$(subst $(_PATHSEP), ,$(VPATH)),			     \
		-W $(V))						     \
		-n simmod.$(MODULE_PYNAME).$(subst -,_,$*)		     \
		-t pyifaces-$*.t $< -o pyiface-$*

$(foreach stem,$(IFACE_FILES:.h=),pyiface-$(stem)-wrappers.d pyiface-$(stem)-wrappers.$(OBJEXT)): $(foreach stem,$(IFACE_FILES:.h=),pyiface-$(stem)-trampoline-data.h)

pyiface-%-trampoline-data.h: pyiface-%-trampolines.od
	$(info GEN     $@)
	$(PYTHON3) $(SIMICS_BASE)/scripts/build/analyze-trampolines.py $(HOST_TYPE) < $< > $@

_MODULE_ID_PYIFACE_FLAGS = $(foreach stem,$(IFACE_FILES:.h=),\
--iface-py-module $(subst -,_,$(stem))) \
$(foreach list,$(_IFACE_INTERFACE_LISTS),--py-iface-list $(list)) \
--py-ver 3 --py-minor-ver 9

endif # IFACE_FILES

# If a source file has the form dir/file.c, then make's pattern
# matching will cause generated .d and .o files to be on the form
# dir/file.{d,o}. Therefore, we must create the directory 'dir'.  For
# performance reasons we avoid creating the directory if it already
# exists, and we avoid to even verify that . exists.
_NEEDED_SUBDIRS=$(filter-out .,$(sort $(dir $(SRC_FILES) $(DML_FILES))))
_=$(foreach dir,$(_NEEDED_SUBDIRS),$(if $(wildcard $(dir)),,$(shell $(MKDIRS) $(dir))))
$(if $(strip $(_)),$(error Unexpected mkdir output: $(_)))

MODULE_OBJS=                                                   \
    $(patsubst %.cc,%.$(OBJEXT),$(filter %.cc,$(SRC_FILES)))   \
    $(patsubst %.cpp,%.$(OBJEXT),$(filter %.cpp,$(SRC_FILES))) \
    $(if $(_ALLOW_DOT_C), \
      $(patsubst %.C,%.$(OBJEXT),$(filter %.C,$(SRC_FILES))),) \
    $(patsubst %.cxx,%.$(OBJEXT),$(filter %.cxx,$(SRC_FILES))) \
    $(patsubst %.c,%.$(OBJEXT),$(filter %.c,$(SRC_FILES)))     \
    $(patsubst %.dml,%-dml.$(OBJEXT),$(DML_FILES))

ifneq (,$(IFACE_FILES))
_MODULE_OBJS_PY = $(_PYIFACE_C_FILES:.c=.$(OBJEXT)) $(_MODULE_ID).$(OBJEXT)
else
MODULE_OBJS += $(MODULE_ID).$(OBJEXT)
endif

# module name used for python command files
MODULE_PYNAME= $(subst +,__,$(subst -,_,$(TARGET)))

# shared object file for the module
ifneq (,$(IFACE_FILES))

_MODULE_SHLIB = $(TARGET_DIR)/$(TARGET).$(SO_SFX)

MODULE_SHLIB = $(_MODULE_SHLIB)
_MODULE_SHLIB_GDB = $(MODULE_SHLIB)

else

MODULE_SHLIB = $(TARGET_DIR)/$(TARGET).$(SO_SFX)
_MODULE_SHLIB_GDB = $(MODULE_SHLIB)

endif

ifeq ($(filter -g,$(DMLC_FLAGS)),-g)
ifneq ($(strip $(DML_FILES)),)
MODULE_GDBFILE = $(_MODULE_SHLIB_GDB)-gdb.py
endif
endif

# _FLAKE8_ENABLE is internal and is provided for experimentation only
ifeq ($(_FLAKE8_ENABLE),1)
# Default flags for flake8. For now, we completely disable style checks
# done by pycodestyle (--ignore=E,W). We also disable the following
# pyflake errors (too many to fix):
# - F401: 'module' imported but unused;
# - F403: ‘from 'module' import *’ used; unable to detect undefined names;
# - F405: name may be undefined, or defined from star imports: 'module'
# - F811: redefinition of unused 'name' from line N
# - F841: local variable 'name' is assigned to but never used
# TODO: F811 and F841 enabling may require reasonable effort.
_FLAKE8_FLAGS ?= --ignore=E,W,F401,F403,F405,F811,F841

_FLAKE8_CHECK=$(PYTHON) -m flake8 $(_FLAKE8_FLAGS) $(1)
else
_FLAKE8_CHECK=
endif

COMPILE_PYC ?= 0

define PY_COMPILE =
$(info PYC     $(notdir $(2)))
$(call _rm_or_move,$(2))
$(call _FLAKE8_CHECK,$(1))
$(PYTHON3) $(SIMICS_BASE)/scripts/copy_python.py $(if $(findstring 1,$(COMPILE_PYC)),--compile) $(1) $(2)
endef

ifneq (,$(IFACE_FILES))
_MODULE_ID = module_id
MODULE_ID = $(_MODULE_ID)
else
MODULE_ID = module_id
endif

_SIMMOD_DIR := $(TARGET_DIR)/python-py3/simmod/$(MODULE_PYNAME)

ifneq ($(filter-out %.py,$(PYTHON_FILES)),)
  $(error Invalid value of PYTHON_FILES, may only contain *.py: $(PYTHON_FILES))
endif
ifneq ($(filter startup.py modload.py,$(PYTHON_FILES)),)
# modload/startup are undocumented aliases, which worked in 4.8 for
# compatibility reasons. In 5, the names are unrecognized; this error
# helps finding lingering occurrences early.
$(error please rename startup.py -> simics_start.py, modload.py -> module_load.py)
endif
ifneq ($(filter __init__.py checkpoint_update.py,$(PYTHON_FILES)),)
  $(error use of reserved filename in PYTHON_FILES: $(PYTHON_FILES))
endif

all: $(addprefix $(_SIMMOD_DIR)/,$(PYTHON_FILES))

$(_SIMMOD_DIR)/%.py: %.py $(MOD_MAKEFILE) $(CONFIG_USER)
	$(call PY_COMPILE,$<,$@)

_MODULE_ID_SCRIPT = $(SIMICS_BASE)/scripts/build/module_id.py
ifneq ($(_CORE_PROJECT_BUILD),)
USER_BUILD_ID_FLAGS = --core-build
else
USER_BUILD_ID_FLAGS = $(if $(USER_BUILD_ID),--user-build-id $(USER_BUILD_ID),)
endif

CONFIG_USER=$(wildcard $(SIMICS_PROJECT)/config-user.mk)

.PHONY: all

ifneq ($(THREAD_SAFE),yes)
THREAD_SAFE:=no
endif

ifneq ($(MODULE_USER_VERSION),)
USR_VER_OPT=--user-version $(MODULE_USER_VERSION)
endif

# Single module defined by a .so target file
all: $(API_WARN) $(MODULE_SHLIB) $(MODULE_GDBFILE)

api_warning:
	@echo 'Using the Simics $(SIMICS_API_NAME) API for $(TARGET) module'

.DELETE_ON_ERROR: $(MODULE_SHLIB)

$(_MODULE_SHLIB): $(_MODULE_OBJS_PY)

$(_MODULE_SHLIB): LDFLAGS += $(PYTHON_LDFLAGS)

$(MODULE_SHLIB): $(MODULE_OBJS) $(EXTRA_OBJ_FILES)
	$(info CCLD    $(@F))
	$(call _rm_or_move,$@)
ifeq ($(MSVC),yes)
	$(VCVARSPREFIX) set LINK= & $(LINK) /nologo /subsystem:console	\
		/dll /out:$(@F) /def:$(EXPORTMAP) $(LDFLAGS)		\
		$(LIBS) $^
else
	$(CCLD) $(CCLDFLAGS_DYN) $^ -o $(@F) $(LDFLAGS) $(LIBS)
endif
	$(if $(_IS_WINDOWS),$(PYTHON) $(SIMICS_BASE)/scripts/build/mv.py,mv) $(@F) $@

ifneq ($(MODULE_GDBFILE),)
$(MODULE_GDBFILE): $(MODULE_SHLIB)
	$(info GEN     $(@F))
	$(PYTHON) $(SIMICS_BASE)/scripts/build/gdb_script.py $@ $(MODULE_CLASSES)
endif

ifneq ($(MSVC),yes)
depfiles=$(strip $(MODULE_OBJS:.$(OBJEXT)=.d) $(DML_FILES:.dml=.dmldep))

ifneq (,$(IFACE_FILES))
depfiles+=$(strip $(_MODULE_OBJS_PY:.$(OBJEXT)=.d))
depfiles+=$(_MODULE_ID).d
else
depfiles+=module_id.d
endif

-include $(depfiles)
endif

cls=$(subst ; ,;,$(patsubst %,%;,$(MODULE_CLASSES)))
cmps=$(subst ; ,;,$(patsubst %,%;,$(MODULE_COMPONENTS)))

ifneq (,$(IFACE_FILES))

.PRECIOUS: pyifaces-%.c

pyifaces-%.c: %.h
	$(info GEN     $@)
	$(PYTHON3) $(SIMICS_BASE)/scripts/build/gen_pyifaces_c.py $@ $(SIMICS_BASE)/$(HOST_TYPE)/bin/pywrap/py-typemaps.c $<

.PRECIOUS: pyifaces-%.i

pyifaces-%.i: pyifaces-%.c pyifaces-%.d
	$(info IFACE-CPP $@)
	$(CPP) $(_IFACE_CFLAGS) -DPYWRAP $< >$@

pyifaces-%.t: %.h
	$(info GEN     $@)
	$(PYTHON3) $(SIMICS_BASE)/scripts/build/gen_pyifaces_t.py $@ $(SIMICS_BASE)/$(HOST_TYPE)/bin/pywrap/py-typemaps.c $<

# Must disable ICF (Identical Code Folding) for trampolines to work on GCC >= 5.0
_cc_version:=$(shell $(PYTHON) $(SIMICS_BASE)/scripts/build/cctype.py --version $(CC))
ifeq ($(filter 4.%,$(_cc_version)),)
_DISABLE_ICF=-fdisable-ipa-icf
endif

# should always be compiled with optimization.
# Disable stack protection explicitly, as it's enabled by default on some
# platforms
pyiface-%-trampolines.$(OBJEXT): PYTHON_BLD_CFLAGS += -O2 $(_DISABLE_ICF) -fno-stack-protector

pyiface-%-trampolines.od: pyiface-%-trampolines.$(OBJEXT)
	$(info DISAS   $@)
	$(DISAS) $< >$@

PYWRAPGEN_BIN:=$(SIMICS_BASE)/$(HOST_TYPE)/bin/pywrapgen$(_EXE_SUFFIX)
ifeq ($(_IS_WINDOWS),)
PYWRAPGEN:=LD_LIBRARY_PATH=$(SIMICS_BASE)/$(HOST_TYPE)/bin:$(SIMICS_BASE)/$(HOST_TYPE)/sys/lib $(PYWRAPGEN_BIN)
else
PYWRAPGEN:=set PATH=$(SIMICS_BASE)/$(HOST_TYPE)/bin;%PATH% & $(PYWRAPGEN_BIN)
endif

ifneq ($(SIMGEN_CONFIG),)
# we use SIMGEN_CONFIG as a marker if we are in a repo based build
# if so, we pass an additional -W argument to pywrapgen, because
# recent mingw will resolve the symlink to the pywrap dir and
# hence we need to also pass the resolved location as a -W
# argument to ensure that files in that directory are not skipped
# by pywrapgen
PYWRAPGEN:=$(PYWRAPGEN) -W $(SIMGEN_CONFIG)/../../win64/bin/pywrap/
endif

_IFACE_OBJECTS = $(_PYIFACE_C_FILES:.c=.$(OBJEXT))			       \
		 $(patsubst %.h,pyifaces-%.$(OBJEXT),$(notdir $(IFACE_FILES)))
_IFACE_DEPS = $(_IFACE_OBJECTS:.$(OBJEXT)=.d)

$(_IFACE_OBJECTS): %.$(OBJEXT): %.c $(MOD_MAKEFILE) $(CONFIG_USER)
	$(info CC      $@)
	$(CC) $(_IFACE_CFLAGS) -c $< -o $@

$(_IFACE_DEPS): %.d: %.c $(MOD_MAKEFILE) $(CONFIG_USER)
	$(info IFACE-DEP $@)
	$(DEP_CC) $< $(_IFACE_CFLAGS) -M -MP -std=gnu99 -MF $@

endif # IFACE_FILES

# Call init_local if SRC_FILES contains C sources
_INIT_LOCAL_FLAG := $(if $(strip $(filter %.cc %.cpp %.cxx %.c,$(SRC_FILES)) $(EXTRA_OBJ_FILES)),--user-init-local)
_DML_DEV_FLAGS := $(foreach f,$(DML_FILES:.dml=-dml),--dml-dev $(f))

$(_MODULE_ID).c: MODULE_ID_PYIFACE_FLAGS = $(_MODULE_ID_PYIFACE_FLAGS)

$(_MODULE_ID).c: $(_IFACE_INTERFACE_LISTS)

$(addsuffix .c, $(MODULE_ID)): $(CONFIG_USER) $(MOD_MAKEFILE)
	$(info GEN     $@)
	$(PYTHON) $(_MODULE_ID_SCRIPT) 					\
	   --c-module-id --output $@					\
	   --module-name $(TARGET)					\
	   --classes "$(cls)"						\
	   --components "$(cmps)"					\
	   --host-type $(HOST_TYPE)					\
	   --thread-safe $(THREAD_SAFE)					\
	   $(MODULE_ID_PYIFACE_FLAGS)			                \
	   $(_INIT_LOCAL_FLAG)						\
	   $(_DML_DEV_FLAGS)						\
	   $(USR_VER_OPT)						\
	   $(USER_BUILD_ID_FLAGS)                                       \
	   $(INIT_C_WRAPPERS)

$(addsuffix .$(OBJEXT), $(MODULE_ID)): $(addsuffix .c, $(MODULE_ID)) $(MOD_MAKEFILE) $(CONFIG_USER)
	$(info CC      $@)
ifeq ($(MSVC),yes)
	$(VCVARSPREFIX) $(CC) $(_COMMON_CFLAGS) $(BLD_CFLAGS) /c $(patsubst %.$(OBJEXT),%.c,$@) /Fo$@ /FS
else
	$(CC) $(_COMMON_CFLAGS) $(BLD_CFLAGS) -c $(patsubst %.$(OBJEXT),%.c,$@) -o $@
endif

# --- compilation rules ---

%.$(OBJEXT): %.c $(MOD_MAKEFILE) $(CONFIG_USER)
	$(info CC      $@)
ifeq ($(MSVC),yes)
	$(VCVARSPREFIX) $(CC) $(CFLAGS) $(BLD_CFLAGS) /c $< /Fo$@
else
	$(CC) $(CFLAGS) $(BLD_CFLAGS) -c $< -o $@
endif

ifeq ($(MSVC),yes)
_CXXRULE=$(VCVARSPREFIX) $(CXX) $(CXXFLAGS) $(BLD_CXXFLAGS) /c $< /Fo$@
else
_CXXRULE=$(CXX) $(CXXFLAGS) $(BLD_CXXFLAGS) -c $< -o $@
endif

%.$(OBJEXT): %.cc $(MOD_MAKEFILE) $(CONFIG_USER)
	$(info CXX     $@)
	$(_CXXRULE)

%.$(OBJEXT): %.cpp $(MOD_MAKEFILE) $(CONFIG_USER)
	$(info CXX     $@)
	$(_CXXRULE)

%.$(OBJEXT): %.cxx $(MOD_MAKEFILE) $(CONFIG_USER)
	$(info CXX     $@)
	$(_CXXRULE)

ifneq ($(_ALLOW_DOT_C),)
%.$(OBJEXT): %.C $(MOD_MAKEFILE) $(CONFIG_USER)
	$(info CXX     $@)
	$(_CXXRULE)
endif

# --- dependency rules ---

ifeq ($(DEP_DOUBLE_TARGETS),yes)
DEP_CFLAGS_TARGETS=-MT $@ -MT $(basename $@).$(OBJEXT)
else
DEP_CFLAGS_TARGETS=-MT $(basename $@).$(OBJEXT)
endif

%-dml.d: %-dml-cdep.c
	$(info DEP     $@)
	$(DEP_CC) $< $(DEP_CFLAGS) $(CFLAGS) -MT $@ -MT $*-dml.$(OBJEXT) -MF $@

%.d: %.c
	$(info DEP     $@)
	$(DEP_CC) $< $(DEP_CFLAGS) $(DEP_CFLAGS_TARGETS) $(CFLAGS) -MF $@

%.d: %.cc
	$(info DEP     $@)
	$(DEP_CXX) $< $(DEP_CXXFLAGS) $(DEP_CFLAGS_TARGETS) $(CXXFLAGS) -MF $@

%.d: %.cpp
	$(info DEP     $@)
	$(DEP_CXX) $< $(DEP_CXXFLAGS) $(DEP_CFLAGS_TARGETS) $(CXXFLAGS) -MF $@

%.d: %.cxx
	$(info DEP     $@)
	$(DEP_CXX) $< $(DEP_CXXFLAGS) $(DEP_CFLAGS_TARGETS) $(CXXFLAGS) -MF $@

ifneq ($(_ALLOW_DOT_C),)
%.d: %.C
	$(info DEP     $@)
	$(DEP_CXX) $< $(DEP_CXXFLAGS) $(DEP_CFLAGS_TARGETS) $(CXXFLAGS) -MF $@
endif

.SECONDARY: $(DML_FILES:.dml=-dml-cdep.c)
%-dml-cdep.c: %.dmldep ;
%.dmldep: %.dml
	$(info DML-DEP $@)
	$(DMLC) $(DMLC_FLAGS) --dep $@ $< $*-dml

######## DML
_PROJ_DML_API := $(wildcard $(DMLC_DIR)/dml/api)
DMLC_FLAGS += $(if $(_PROJ_DML_API),-I$(_PROJ_DML_API)/$(SIMICS_API))
DMLC_FLAGS += $(call _mangle_path_list,$(DML_INCLUDE_PATHS),-I,/$(SIMICS_API))
# For old API versions, include a version of the DML standard lib that
# contains e.g. PCI common code
DMLC_FLAGS += -I$(DMLC_DIR)/dml
DMLC_FLAGS += -I.

ifneq ($(SUPPRESS_DEVICE_INFO),yes)
DMLC_FLAGS += --info
endif

DMLC_FLAGS += $(if $(DMLC_COVERITY),--coverity)

DMLC_FLAGS += $(DMLC_FLAGS_USER)

DMLC_FLAGS += $(patsubst %, -I%,$(subst $(_PATHSEP), ,$(EXTRA_FLAG_PATHS)))

.SECONDARY: $(foreach suffix,-dml.c -dml.h -dml-struct.h -dml-protos.c -dml-protos.h -dml.xml,$(DML_FILES:.dml=$(suffix)))

%-dml-protos.c %-dml-protos.h %-dml.h %-dml-struct.h: %-dml.c ;

ifneq ($(SUPPRESS_DEVICE_INFO),yes)
%-dml.xml: %-dml.c ;
endif

_DMLC_P_FLAG=$(if $(DMLC_PORTING_TAG_FILE),-P $(DMLC_PORTING_TAG_FILE),)

%-dml.c: %.dml
	$(info DMLC    $@)
	$(DMLC) $(DMLC_FLAGS) $< $*-dml $(_DMLC_P_FLAG)

ifneq ($(DML_FILES),)
ifneq ($(SUPPRESS_DEVICE_INFO),yes)
# Copy .xml files to new names, based on class names
all: $(TARGET)-devinfo-copied
$(TARGET)-devinfo-copied: $(DML_FILES:.dml=-dml.xml)
	$(PYTHON) $(SIMICS_BASE)/scripts/build/copy_device_xml.py --marker=$@ --copy-to=$(TARGET_DIR) $?
endif
endif

# This variable is unsupported, but relied upon by some users. See bug 21173.
ifneq ($(_RAW_EML_PACKAGE),)
ifneq ($(SUPPRESS_DEVICE_INFO),yes)
include $(call _makequote,$(_RAW_EML_PACKAGE)/config/project/module-eml.mk)
endif
endif

# copy images
ifneq ($(SRC_IMAGES),)
all: $(addprefix $(TARGET_DIR)/images/,$(SRC_IMAGES))

$(TARGET_DIR)/images/%: %
	$(info Copying $(@F))
	$(MKDIRS) $(TARGET_DIR)/images
	$(PYTHON) $(SIMICS_BASE)/scripts/build/atomic_copy.py \
		$(call _shellquote,$<) $(call _shellquote,$@)
endif

########

-include $(SIMICS_PROJECT)/module-user.mk
-include $(SIMICS_PROJECT)/mod-build-hook.mk
