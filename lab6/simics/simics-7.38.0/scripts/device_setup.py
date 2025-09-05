# -*- python -*-

# © 2010 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

#pylint: disable=consider-using-f-string

import sys
import os
import re
import shutil
import codecs
import locale
import fnmatch

from os.path import (join, isfile, isdir)
from argparse import SUPPRESS

from simicsutils.host import is_windows
from simicsutils.internal import ensure_text
from project import (find_mingw_dir,
                     find_model_builder_path,
                     find_dodoc_path)

# make sure we are running with native Python
if sys.platform == "cygwin":
    print("setup-device is meant to be run with a native Python installation,"
          " but this looks like Cygwin's Python.")
    sys.exit(1)

RESERVED_NAMES = ('all',
                  'api_warning',
                  'clean',
                  'clean-*',
                  'clobber-*',
                  'gnumakefile',
                  'test')

def pr(s, options):
    if options.dry_run:
        sys.stdout.write("[dry-run] ")
        sys.stdout.flush()
    sys.stdout.buffer.write((s + '\n').encode('utf-8', 'surrogateescape'))

def pr_verbose(text, options):
    if options.verbose:
        pr(text, options)

def pref_unicode(s):
    return ensure_text(s, locale.getpreferredencoding())

def print_columns(cells, prefix='', padding='  ', width=80):
    if not cells:
        return

    widths = [ len(cell) for cell in cells ]

    # come up with a conservative initial estimate for the number of
    # rows to display
    if padding:
        columns = ((width - max(widths) - len(prefix) + len(padding) - 1)
                   // len(padding))
        columns = max(1, columns)
        rows = max(1, (len(cells) + columns  - 1) // columns)
    else:
        rows = 1
    columns = (len(cells) + rows - 1) // rows

    # find the smallest number of rows needed
    while columns >= 1:
        sizes = [ 0 ] * columns
        #pylint: disable=consider-using-enumerate
        for cell in range(len(cells)):
            col = cell // rows
            sizes[col] = max(sizes[col], len(cells[cell]))
        tot_width = sum(sizes) + len(prefix) + len(padding) * (columns - 1)
        if tot_width <= width:
            break

        if columns > rows:
            rows += 1
            columns = (len(cells) + rows - 1) // rows
        else:
            columns -= 1
            if columns < 1:
                break
            rows = (len(cells) + columns - 1) // columns

    # print the result
    columns = (len(cells) + rows - 1) // rows
    for row in range(rows):
        pad = prefix
        for col in range(columns):
            cell = row + col * rows
            if cell >= len(cells):
                break
            sys.stdout.write(pad)
            cell = cells[cell]
            sys.stdout.write(cell)
            if row + (col + 1) * rows >= len(cells):
                continue
            sys.stdout.write(' ' * (sizes[col] - len(cell)))
            pad = padding
        print()

def systemc_library_path_exists(options):
    return isdir(options.lookup_file.lookup_path_in_packages(
        join("src", "extensions", "systemc-library")))

def isctlm_library_path_exists(options):
    return isdir(options.lookup_file.lookup_path_in_packages(
        join("src", "extensions", "systemc-isctlm-awareness", "awareness",
             "isctlm")))

def empty_instrumentation_path_exists(options):
    return isdir(options.lookup_file.lookup_path_in_packages(
        join("src", "extensions", "empty-instrumentation-tool")))

def empty_instrumentation_filter_path_exists(options):
    return isdir(options.lookup_file.lookup_path_in_packages(
        join("src", "extensions", "empty-instrumentation-filter")))

def is_valid_class_name(module):
    return (bool(re.match("[A-Za-z][A-Za-z0-9_-]*$", module))
            and not any(fnmatch.fnmatchcase(module.lower(), v)
                        for v in RESERVED_NAMES))

def sdb_path(options):
    return options.lookup_file.lookup_path_in_packages("sdb")

def sdb_path_exists(options):
    return isdir(sdb_path(options))

def copy_sdb(options, project):
    source_dir = sdb_path(options)
    target_dir = join(project.path, "sdb")
    if isdir(target_dir):
        pr(("Error: target dir for sdb '%s' already exists" % target_dir),
           options)
        sys.exit(1)
    if not options.dry_run:
        copy_dir(options, target_dir, source_dir, False)

def print_modules(options, package_paths):
    possible = set()
    for p in package_paths:
        for subdir in ("devices", "extensions", "components", "cpu"):
            source_dir = join(p, "src", subdir)
            try:
                possible.update(d for d in os.listdir(source_dir)
                                if isdir(join(source_dir, d)))
            except OSError:
                pass
    if possible:
        pr("Source code is available for the following module%s:" % (
                's' if len(possible) > 1 else ''), options)
        print_columns(sorted(possible), prefix='  ')
    else:
        pr("Source code is not available for any modules", options)

def create_copied_modules(options, project, package_paths):
    failed = []
    for mod in options.copied_modules:
        # find the source directory
        try:
            source_dir = next(filter(
                None, (options.lookup_file.lookup_path_in_packages(
                        join("src", subdir, mod)) for subdir in [
                        "devices", "extensions", "components", "cpu"])))
        except StopIteration:
            failed.append(mod)
        else:
            copy_module_files(options, project, source_dir, mod, '')

    if failed:
        pr("Failed finding the source code for %s:" % (
                'these modules' if len(failed) > 1 else 'this module'), options)
        pr(' '.join(failed), options)
        print_modules(options, package_paths)

def create_copied_tests(options, project, package_paths):
    failed = []
    for test in options.copied_tests:
        print('copying', test)
        # find the source directory
        source_dir = options.lookup_file.lookup_path_in_packages(
            join("test", test))
        if not source_dir:
            failed.append(test)
        else:
            copy_test_files(options, project, source_dir, test)

    if failed:
        pr("Failed finding %s:" % (
                'these tests' if len(failed) > 1 else 'this test'), options)
        pr(' '.join(failed), options)
        possible = []
        for p in package_paths:
            source_dir = join(p, "test")
            try:
                possible.extend([d for d in os.listdir(source_dir) if (isdir(join(source_dir, d))
                               and isfile(join(source_dir, d, "SUITEINFO")))])
            except OSError:
                pass
        if possible:
            possible.sort()
            pr("The following test%s available:" % (
                    's are' if len(possible) > 1 else ' is'), options)
            print_columns(possible, prefix='  ')
        else:
            pr("No tests available", options)

def copy_file(options, target_dir, source_dir, source_file, translate):
    source = join(source_dir, source_file)
    dest = join(target_dir,
                translate(source_file) if translate else source_file)
    if not options.dry_run:
        if translate:
            with codecs.open(source, "r", "utf-8") as s:
                content = s.read()
            content = translate(content)
            with codecs.open(dest, "w", "utf-8") as d:
                d.write(content)
        else:
            shutil.copy(source, dest)
    pr_verbose("Wrote: %s" % dest, options)

def copy_dir(options, target_dir, source_dir, translate):
    if not options.dry_run:
        os.makedirs(target_dir)
    for source_file in os.listdir(source_dir):
        if source_file == ".svn":
            continue
        if source_file.endswith(".pyc"):
            continue
        full_source = join(source_dir, source_file)
        if isdir(full_source):
            target_basename = (translate(source_file) if translate
                               else source_file)
            copy_dir(options, join(target_dir, target_basename),
                     full_source, translate)
        if isfile(full_source):
            copy_file(options, target_dir, source_dir, source_file, translate)

def copy_module_files(options, project, source_dir, module, dir_suffix,
                      translate=False):
    target_dir = join(project.path, "modules", module + dir_suffix)
    if isdir(target_dir):
        pr("Error: module %s, directory %s already exists. Aborting." %
           (module, target_dir), options)
        sys.exit(1)
    pr_verbose("Creating module directory: %s" % target_dir, options)
    copy_dir(options, target_dir, source_dir, translate)

def copy_test_files(options, project, source_dir, test,
                    translate=False):
    target_dir = join(project.path, "test", test)
    if isdir(target_dir):
        pr("Ignoring test suite %s, "
           "directory %s already exists." % (test, target_dir), options)
        return
    pr_verbose("Creating test suite: %s" % target_dir, options)
    copy_dir(options, target_dir, source_dir, translate)

def copy_module(options, project, module, source_parent, source_module,
                dir_suffix, replace_name):
    if replace_name is None:
        replace_name = source_module
    if not is_valid_class_name(module):
        name_list = ', '.join(RESERVED_NAMES)

        pr("Invalid device name '%s'. Device names must consist of"
           " letters, digits, underscores or dashes, and not start"
           " with a digit. There are also a few reserved names: %s."
           % (module, name_list), options)
        sys.exit(1)

    idbase = module.replace('-', '_')
    idbase_source = replace_name.replace('-', '_')

    pr_verbose("Creating module skeleton for module '%s', "
               "C identifier base = '%s'" % (module, idbase), options)

    source_dir = join(options.model_builder_path, "src", source_parent, source_module)
    if not isdir(source_dir):
        # Module could be distributed by another package, find that package
        relative_module_path = join("src", source_parent, source_module)
        source_dir = options.lookup_file.lookup_path_in_packages(
            relative_module_path)

        if not source_dir:
            pr("Error: could not find %s in any of the installed packages."
               " Please verify that the installed packages have not been"
               " corrupted." % relative_module_path, options)
            sys.exit(1)

    def convert(s):
        return (s.replace(replace_name, module)
                .replace(idbase_source, idbase)
                .replace(idbase_source.upper(), idbase.upper())
                .replace("if(SIMICS_ENABLE_TESTS_FROM_PACKAGES)", "")
                .replace("endif()", ""))
    copy_module_files(options, project, source_dir, module, dir_suffix,
                      convert)
    pr(("Skeleton for module '%s' successfully created."
        % (module + dir_suffix)), options)

#:: doc mb-usage {{
# {{include project_setup.py#synopsis}}
#
# ## DESCRIPTION
# This section only includes the Model Builder specific functionality of
# `project-setup`. The documentation for the basic functionality can be found
# in the *Simics Reference Manual*.
#
# ## OPTIONS
#
# <div class="dl">
#
# - <span class="term">`--device=DEVICE_NAME, --dml-device=DEVICE_NAME`</span>
#     Generate skeleton code for a device, suitable to use as a starting point
#     for implementing your own device. The default implementation language is
#     DML. See the *--c-device* and *--py-device* options for creating devices
#     using other languages. If the device already exists, this option is
#     ignored. To recreate the skeleton, remove the device directory.
# - <span class="term">`--c-device=DEVICE_NAME`</span>
#     Similar to *--device*, but creates a device using C instead of DML.
# - <span class="term">`--c++-device=DEVICE_NAME`</span>
#     Similar to *--device*, but creates a device using C++ instead of DML.
# - <span class="term">`--py-device=DEVICE_NAME`</span>
#     Similar to *--device*, but creates a device using Python and the *pyobj*
#     module instead of DML.
# - <span class="term">`--py-device-confclass=DEVICE_NAME`</span>
#     Similar to *--device*, but creates a device using Python and the
#     *confclass* module instead of DML.
# - <span class="term">`--component=COMPONENT_NAME`</span>
#     Similar to *--device*, but creates a component skeleton in Python.
# - <span class="term">`--copy-module=DEVICE_NAME`</span>
#     Copies the source code of a module into the project. The files will be
#     copied from the Simics installation. If the module already exist in your
#     project, you must manually delete it first.
# - <span class="term">`--list-modules`</span>
#     List the modules with source code that can be copied into the project
#     using the --copy-module argument.
# - <span class="term">`--osa-tracker=TRACKER_NAME`</span>
#     Generate skeleton code for an OS Awareness tracker, suitable to use as a
#     starting point for implementing your own OS Awareness tracker. If the
#     module already exists, this option is ignored. To recreate the skeleton,
#     remove the module directory. The skeleton will be created in a directory
#     named after the TRACKER\_NAME and suffixed with '-tracker'.
# - <span class="term">`--instrumentation-tool=TOOL_NAME`</span>
#     Generate skeleton code for an instrumentation tool, suitable to use as a
#     starting point for implementing your own tool. If the module already
#     exists, this option is ignored. To recreate the skeleton, remove the
#     module directory. The skeleton will be created in a directory named after
#     the TOOL\_NAME.
# - <span class="term">`--instrumentation-filter=FILTER_NAME`</span>
#     Generate skeleton code for an instrumentation filter, suitable to use as
#     a starting point for implementing your own filter. If the module already
#     exists, this option is ignored. To recreate the skeleton, remove the
#     module directory. The skeleton will be created in a directory named after
#     the FILTER\_NAME.
# </div>
# }}

def py_device_help(py_class):
    return ("Similar to --device, but creates a device using Python and the "
            f"'{py_class}' class instead of DML.")

def add_py_device_argument(parser_modeling_options, name, dest, py_class,
                           help_msg):
    parser_modeling_options.add_argument(
        name, action="append", type=pref_unicode, metavar="DEVICE_NAME",
        dest=dest, default=[], help=help_msg)

def device_setup_options(parser):
    parser_modeling_options = parser.add_argument_group("Device Options")

    if is_windows():
        parser_modeling_options.add_argument(
            "--mingw-dir",
            dest="mingw_dir", metavar="DIR",
            help=("Use this MinGW installation directory instead of searching"
                  " the default locations."))


    parser_modeling_options.add_argument("--device", "--dml-device",
                      action="append",
                      type=pref_unicode,
                      metavar="DEVICE_NAME",
                      dest="dml_modules",
                      default=[],
                      help=
        "Generate skeleton code for a device, suitable to use as a starting"
        " point for implementing your own device. The default implementation"
        " language is DML. See the --c-device and --py-device options for"
        " creating devices using other languages. If the device already"
        " exists, this option is ignored. To recreate the skeleton, remove"
        " the device directory.")

    parser_modeling_options.add_argument("--c-device",
                      action="append",
                      type=pref_unicode,
                      metavar="DEVICE_NAME",
                      dest="c_modules",
                      default=[],
                      help=
        "Similar to --device, but creates a device using C instead of DML.")

    parser_modeling_options.add_argument("--c++-device",
                      action="append",
                      metavar="DEVICE_NAME",
                      dest="cc_modules",
                      default=[],
                      help=
        "Similar to --device, but creates a device using C++ instead of DML.")

    add_py_device_argument(parser_modeling_options, "--py-device",
                           "py_modules", "pyobj", py_device_help('pyobj'))
    add_py_device_argument(parser_modeling_options, "--py-device-confclass",
                           "py_modules_confclass", "confclass",
                           py_device_help('confclass'))

    parser_modeling_options.add_argument("--interface",
                      action="append",
                      type=pref_unicode,
                      metavar="INTERFACE_NAME",
                      dest="interface_modules",
                      default=[],
                      help=
        "Creates skeleton code for a module that defines a new interface type.")

    parser_modeling_options.add_argument("--component",
                      action="append",
                      type=pref_unicode,
                      metavar="DEVICE_NAME",
                      dest="components",
                      default=[],
                      help=
        "Similar to --device, but creates a component skeleton in Python.")

    parser_modeling_options.add_argument("--blueprint",
                      action="append",
                      type=pref_unicode,
                      metavar="DEVICE_NAME",
                      dest="blueprints",
                      default=[],
                      help=
        "Similar to --device, but creates a blueprint skeleton in Python.")

    parser_modeling_options.add_argument("--sc-device",
                      action="append",
                      metavar="DEVICE_NAME",
                      dest="sc_modules",
                      default=[],
                      help=
        "Similar to --device, but creates a SystemC device suitable for"
        " integration with Simics devices using an adapter.")

    parser_modeling_options.add_argument("--sc-only-device",
                      action="append",
                      metavar="DEVICE_NAME",
                      dest="sc_only_modules",
                      default=[],
                      help=
        "Similar to --device, but creates a SystemC device that is meant to"
        " run in a SystemC-only environment (no connection to devices via the"
        " Simics API)")

    parser_modeling_options.add_argument("--isctlm-device",
                        action="append",
                        metavar="DEVICE_NAME",
                        dest="isctlm_modules",
                        default=[],
                        help=
        "Similar to --device, but creates a ISCTLM device suitable for"
        " integration with Simics devices using an adapter.")

    parser_modeling_options.add_argument("--copy-module",
                      action="append",
                      type=pref_unicode,
                      metavar="MODULE_NAME",
                      dest="copied_modules",
                      default=[],
                      help=
        "Copies the source code of a module into the project. The files will"
        " be copied from the Simics installation. If the module already"
        " exists in your project, you must manually delete it first.")

    parser_modeling_options.add_argument("--copy-device",
                      action="append",
                      type=pref_unicode,
                      metavar="MODULE_NAME",
                      dest="copied_modules",
                      default=[],
                      help=SUPPRESS)

    parser_modeling_options.add_argument("--list-modules",
                      action="store_true",
                      default=False,
                      dest="list_modules",
                      help=
        "List the modules with source code that can be copied into the project"
        " using the --copy-module argument.")

    parser_modeling_options.add_argument("--copy-test",
                      action="append",
                      type=pref_unicode,
                      metavar="MODULE_NAME",
                      dest="copied_tests",
                      default=[],
                      help=
        "Copies the test suite into the project. The suite will be copied"
        " from the Simics installation. If the test already exist in your"
        " project, you must manually delete it first.")

    parser_modeling_options.add_argument("--copy-sdb",
                      action="store_true",
                      default=False,
                      dest="sdb",
                      help=
        "Copy the SDB files from the installation into the project directory."
        " SDB is an easy way of running x86 host binaries inside virtual Linux"
        " system (QSP), and requires the experimental SDB package (7020) to be"
        " installed.")

    parser_modeling_options.add_argument("--osa-tracker",
                        action="append",
                        metavar="TRACKER_NAME",
                        dest="osa_tracker",
                        default=[],
                        help=
        "Generate skeleton code for an OS Awareness tracker in C. The name of"
        " the skeleton will be suffixed with '-tracker'.")

    parser_modeling_options.add_argument("--instrumentation-tool",
                        action="append",
                        metavar="TOOL_NAME",
                        dest="instrumentation_tool",
                        default=[],
                        help=
        "Generate skeleton code for an instrumentation tool in C.")

    parser_modeling_options.add_argument("--instrumentation-filter",
                        action="append",
                        metavar="FILTER_NAME",
                        dest="instrumentation_filter",
                        default=[],
                        help=
        "Generate skeleton code for an instrumentation filter in C.")

def copy_skeletons_list(options):
    # By default (replace_name = None) the name of the empty skeleton and the
    # name of the new module will be used for renaming files and replacing file
    # contents to create the module based on the skeleton. It is possible to
    # use a different pattern than the name of the empty skeleton by providing
    # a replace_name.
    return [
        (skel_parent, skel, module, dir_suffix, replace_name)
        for (skel_parent, skel, dir_suffix, replace_name, modules) in [
            ("devices", "empty-device-dml", '', None, options.dml_modules),
            ("devices", "empty-device-c", '', None, options.c_modules),
            ("devices", "empty-device-cc", '', None, options.cc_modules),
            ("devices", "empty-device-tlm2", '', None, options.sc_modules),
            ("devices", "empty-device-sc-only", '', None,
             options.sc_only_modules),
            ("devices", "empty-device-isctlm", '', None,
             options.isctlm_modules),
            ("extensions", "empty-interface", '-interface', None,
             options.interface_modules),
            ("devices", "empty-device-pyobj", '', None,
             options.py_modules),
            ("devices", "empty-device-confclass", '', None,
             options.py_modules_confclass),
            ("components", "empty-components", '', None, options.components),
            ("components", "empty-blueprint", '', None, options.blueprints),
            ("extensions", "empty-software-tracker", '-tracker',
             'empty-software', options.osa_tracker),
            ("extensions", "empty-instrumentation-tool", '',
             'empty-instrumentation-tool', options.instrumentation_tool),
            ("extensions", "empty-instrumentation-filter", '',
             'empty-instrumentation-filter', options.instrumentation_filter),
        ]
        for module in modules ]

def device_setup(options, project, package_paths):
    options.mingw_path = None
    options.model_builder_path = None
    options.module_building_enabled = True

    options.model_builder_path = find_model_builder_path(package_paths)
    if not options.model_builder_path:
        options.module_building_enabled = False

    options.dodoc_pkg = find_dodoc_path(package_paths)

    if is_windows():
        options.mingw_path = find_mingw_dir(
            options.simics_root, options.mingw_dir or project.get_mingw_dir())
        if options.mingw_dir and options.mingw_dir != options.mingw_path:
            pr("%s does not contain a working gcc" % (options.mingw_dir,),
               options)
            sys.exit(1)

    copy_skeletons = copy_skeletons_list(options)
    if (not options.module_building_enabled and
        (copy_skeletons or options.copied_modules or options.list_modules)):
        pr("No Model Builder product was found, so no options related"
           " to building device models are available"
           " (applies to --copy-module, --list-modules, --c-device,"
           " --c++-device, --dml-device,"
           " --py-device, --interface-module, and --osa-tracker).", options)
        sys.exit(1)
    if ((options.sc_modules or options.sc_only_modules)
        and not systemc_library_path_exists(options)):
        pr("No SystemC Library package was found, so no options related"
           " to building SystemC device models are available"
           " (applies to --sc-device and --sc-only-device).", options)
        sys.exit(1)
    if options.isctlm_modules and not isctlm_library_path_exists(options):
        pr("No Intel SystemC package was found, so no options related"
           " to building ISCTLM device models are available"
           " (applies to --isctlm-device).", options)
        sys.exit(1)
    if (options.instrumentation_tool and
        not empty_instrumentation_path_exists(options)):
        pr("No Instrumentation Preview package was found, so no options related"
           " to building instrumentation tools are available"
           " (applies to --instrumentation-tool).", options)
        sys.exit(1)
    if (options.instrumentation_filter and
        not empty_instrumentation_filter_path_exists(options)):
        pr("No Instrumentation Preview package was found, so no options related"
           " to building instrumentation filters are available"
           " (applies to --instrumentation-filter).", options)
        sys.exit(1)
    if (options.sdb and not sdb_path_exists(options)):
        pr("No SDB package (7020) was found (applied to --copy-sdb).",
           options)
        sys.exit(1)

def device_modules(options, project, package_paths):
    """handle list/copy/create modules"""
    copy_skeletons = copy_skeletons_list(options)
    if options.module_building_enabled and options.list_modules:
        print_modules(options, package_paths)
    if options.module_building_enabled:
        for (skel_parent, skel, module, dir_suffix, replace_name) \
            in copy_skeletons:
            copy_module(options, project, module, skel_parent, skel,
                        dir_suffix, replace_name)

        create_copied_modules(options, project, package_paths)
        create_copied_tests(options, project, package_paths)

def device_module_sdb(options, project):
    if options.sdb:
        copy_sdb(options, project)
