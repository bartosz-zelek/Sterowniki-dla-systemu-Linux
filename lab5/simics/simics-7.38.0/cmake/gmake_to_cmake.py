#!/usr/bin/env python3

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

# Naive attempt att parsing a GNU Makefile and produce a CMakeLists.txt for the
# most common use-cases.
#
# NOTE: this is _not_ a complete tool for converting modules
#

# We need python 3.9 for str.removesuffix to work
if not hasattr(str, "removesuffix"):
    msg = """Need at least python 3.9 to run the converter.
           HINT: try to run it with mini-python"""  # aligned to Exception
    raise Exception(msg)


import argparse
import re
import sys
import tempfile
from pprint import pprint
from pathlib import Path

# pylint: disable=too-many-branches,too-many-locals,too-many-statements

copyright_header = """# © 2024 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

"""

def split_first_line(l):
    m = re.match(r'([^ :+]*)\s*[:+]?=\s*([^\\]*)(\\?)$', l.strip())
    if not m:
        # Sometimes MODULE_CFLAGS use \" mark-ups for -D flags;
        # but this is a special case we can use as fall-back
        m = re.match(r'([^ :+]*)\s*[:+]?=\s*(.*\\\".*\\\"[^\\]*)(\\?)$', l.strip())
        if m:
            return m.group(1), m.group(2), m.group(3) == '\\'
        # We also support custom rules, e.g. target: dep
        m = re.match(r'.*:\s.*$', l.strip())
        if m:
            print("  Ignoring line:", l.strip())
        else:
            print("  ERROR: unable to parse line:", l.strip())
        return None, None, False
    return m.group(1), m.group(2), m.group(3) == '\\'

def split_next_line(l):
    m = re.match(r'([^\\]*)(\\?)$', l.strip())
    if not m:
        m = re.match(r'(.*\\\".*\\\"[^\\]*)(\\?)$', l.strip())
        if m:
            return m.group(1), m.group(2) == '\\'
        print("  ERROR: unable to parse line:", l.strip())
        return None, False
    return m.group(1), m.group(2) == '\\'

def parse_makefile(src, dry_run, debug, skip_on_warnings=False):
    # Returns:
    # - a dict of token->value found in Makefile
    # - None if this Makefile could not be parsed at all
    # - False if Makefile could be parsed but contains warnings about
    #   unsupported logic and skip_on_warnings is True
    module = {}
    key = ''  # keep key outside of loop to consume more lines
    more_lines = False
    skip_lines = False
    warnings = False  # set to True if not able to parse Makefile cleanly
    # Parse the src GNU Makefile and create module dict
    for l in src.readlines():
        if not l.strip():
            if more_lines:
                more_lines = False
            continue
        if l.startswith('#') or l.startswith('include'):
            continue
        if l.startswith('if'):
            if not "MODULE_MAKEFILE" in l:
                print("  WARNING: conditional lines in Makefile, please review")
                warnings = True
            skip_lines = True
            continue
        if 'endif' in l:
            skip_lines = False
            continue
        if skip_lines:
            continue

        if not more_lines:
            (key, value, more_lines) = split_first_line(l)
        else:
            (value, more_lines) = split_next_line(l)

        if debug:
            print(f"  >> D: {key} ; {value} ; {more_lines}")

        if value:
            if '$' in value:
                print("  WARNING: references used, please review")
                warnings = True

            # EXTRA_MODULE_VPATH supports : as delimiter
            module.setdefault(key, []).extend(value.replace(':', ' ').split())

    # Sanity checks
    if more_lines or skip_lines:
        print(f"Failed to convert file, unexpected format: {src.name}")
        return None

    if not any([module.get('MODULE_CLASSES'),
                module.get('MODULE_COMPONENTS'),
                module.get('IFACE_FILES'),
                module.get('PYTHON_FILES')]):
        print(f"Failed to convert file, not a Simics module: {src.name}")
        return None

    if warnings and skip_on_warnings:
        print(f"Failed to convert file, warnings detected")
        return False

    if dry_run:
        pprint(module)
        sys.exit(0)

    return module

def convert(src, dst, root_path, name, indent=2, dry_run=False, debug=False,
            skip_on_warnings=False):
    module = parse_makefile(src, dry_run, debug, skip_on_warnings)
    if not module:
        return module  # propagate False or None to caller

    warnings = False   # set to True if something is off

    dst.write(f"simics_add_module({name}\n")
    if module.get('MODULE_CLASSES'):
        dst.write(" " * indent + "CLASSES " + ' '.join(module['MODULE_CLASSES']) + "\n")
    if module.get('MODULE_COMPONENTS'):
        dst.write(" " * indent + "COMPONENTS " + ' '.join(module['MODULE_COMPONENTS']) + "\n")

    # All these type of files are added to the same SOURCES in CMake
    files = sorted(module.get('SRC_FILES', []))
    files += sorted(module.get('PYTHON_FILES', []))
    files += sorted(module.get('IFACE_FILES', []))
    # Before we write SOURCES we validate that all files exists. Typically
    # CMake wants to build a static library and add it as a link dependency
    # instead of rebuilding the source files over and over; so fiddling with
    # VPATH and adding the files to SOURCES would be wrong. We also scan the
    # files for the presence of init_local, added to dst file a bit further
    # down below.
    interfaces = []
    sources = []
    user_init_local = False
    iface_pattern = re.compile(r'SIM_INTERFACE\((.*)\)')
    for fname in files:
        fpath = root_path / fname
        if fpath.exists():
            if fname in module.get('IFACE_FILES', []):
                with open(fpath, encoding='utf8') as f:
                    for l in f.readlines():
                        m = iface_pattern.match(l)
                        if m:
                            interfaces.append(m.group(1))
            else:
                sources.append(fname)
            if fname in module.get('SRC_FILES', []):
                with open(fpath, encoding='utf8') as f:
                    for l in f.readlines():
                        if l.find("init_local(") >= 0:
                            user_init_local = True
                            break
        else:
            print("  WARNING: file listed in Makefile does not exist:", fname)
            warnings = True

    # A common pattern is to add .dml files, and to use references doing so to
    # avoid specifying the same names in both MODULE_CLASSES and
    # SRC_FILES. Instead of learning to parse GNU Make syntax; we scan all .dml
    # files and look for top-level 'device' statements.
    for f in sorted(Path(root_path).glob("*.dml"), reverse=True):
        if f.name not in sources and re.search(r'\ndevice\s+.*;', f.read_text()):
            # we want .dml files to be inserted before .py files
            sources.insert(0, f.name)

    if interfaces:
        all_ifaces = ' '.join(interfaces)
        dst.write(" " * indent + f"INTERFACES {all_ifaces}\n")
    if sources or module.get('IFACE_FILES'):
        all_sources = sources
        if module.get('IFACE_FILES'):
            all_sources += sorted(module['IFACE_FILES'])
        all_files = ' '.join(sources)
        dst.write(" " * indent + f"SOURCES {all_files}\n")
    if user_init_local:
        dst.write(" " * indent + "INIT_LOCAL\n")
    if module.get('SIMICS_API'):
        dst.write(" " * indent + f"SIMICS_API {module['SIMICS_API'][0]}\n")
    dst.write(")\n")
    if module.get('EXTRA_MODULE_VPATH'):
        # Some modules (generic-sdmmc-card) add itself to VPATH, and this is
        # not supported in CMake
        # Also, unify dual format (replace :)
        moduletype = 'dml' if "".join([d for d in sources if 'dml' in d]) else 'c'
        subtype = {'c': 'includes',  # ignore headers
                   'dml': 'imports'}[moduletype]
        extra_modules = [f'{e}::{subtype}' for e in module['EXTRA_MODULE_VPATH']
                         if not e == name]
        # Strip subtype from some known common libraries
        if f'dml-lib::{subtype}' in extra_modules:
            extra_modules.remove(f'dml-lib::{subtype}')
            extra_modules.append('dml-lib')
        dst.write(f"target_link_libraries({name}\n")
        dst.write(" " * indent + "PRIVATE " + ' '.join(extra_modules) + "\n")
        dst.write(")\n")
    if module.get('USE_CC_API'):
        # To simplify the converter logic, add an extra `target_link_libraries`
        # section here. The end-user can merge this with EXTRA_MODULE_VPATH if
        # needed
        dst.write(f"target_link_libraries({name}\n")
        dst.write(" " * indent + "PRIVATE Simics::C++\n")
        dst.write(")\n")
    cflags = module.get('MODULE_CFLAGS')
    if cflags:
        warnings = True
        print("  WARNING: MODULE_CFLAGS used, please review:")
        print(cflags)
        defines=[]
        for e in [e[2:] for e in cflags
                  if e.startswith('-D')]:
            defines.append(e)
        if defines:
            dst.write("add_compile_definitions(")
            dst.write(" ".join(defines))
            dst.write(")\n")
    ldflags = module.get('MODULE_LDFLAGS')
    if ldflags:
        warnings = True
        print("  WARNING: MODULE_LDFLAGS used, please review:")
        print(ldflags)
        # Add support for some common LDFLAGS used in Simics Base repo
        if '-llink' in ldflags:
            dst.write(f"target_link_libraries({name}\n")
            dst.write(" " * indent + "PRIVATE link\n")
            dst.write(")\n")

    dmlc_flags = module.get('DMLC_FLAGS')
    if dmlc_flags:
        warnings = True
        print("  WARNING: DMLC_FLAGS used, please review:")
        print(dmlc_flags)
        dst.write(f"set_target_properties({name}\n")
        dst.write(" " * indent + "PROPERTIES SIMICS_DMLC_FLAGS ")
        for f in dmlc_flags:
            dst.write(f"{f}")
        dst.write("\n)\n")

    return not warnings

def add_test_suite(dst, testdir, indent, simics_base=False):
    dst.write("add_subdirectory(test)\n")
    with open(testdir / "CMakeLists.txt", 'w', encoding='utf8') as d:
        spaces = ""
        if simics_base:
            d.write(copyright_header)
            d.write("if(SIMICS_ENABLE_TESTS_FROM_PACKAGES)\n")
            spaces = " " * indent

        # NOTE: wrapping of tests.py is for the user to handle, so just warn about it
        if (testdir / "tests.py").exists():
            print("  WARNING: tests.py detected, needs manual porting of test logic")
        for test in sorted(testdir.glob("s-*.py")):
            t = str(test.name).removeprefix("s-").removesuffix(".py")
            d.write(spaces + f"simics_add_test({t})\n")
        for test in sorted(testdir.glob("s-*.simics")):
            t = str(test.name).removeprefix("s-").removesuffix(".simics")
            d.write(spaces + f"simics_add_test({t})\n")
        if simics_base:
            d.write("endif()\n")

def outer_convert(path, indent, dry_run, debug, skip_tests,
                  simics_base=False, skip_on_warnings=False):
    assert isinstance(path, Path)
    assert path.is_dir()

    src = path / "Makefile"
    dst = path / "CMakeLists.txt"
    module_name = path.absolute().name
    print(f"Converting {module_name} ...")
    with open(src, 'r', encoding='utf8') as s:
        if dry_run:
            # Just parse in dry-run. Don't try to convert as that will create
            # 'dst' file which is not consistent wtih dry-run behavior.
            parse_makefile(s, dry_run, debug)
        else:
            # Write to temporary file; then copy to dst if successful
            with tempfile.TemporaryFile(mode='w+') as tmp:
                if simics_base:
                    tmp.write(copyright_header)
                clean = convert(s, tmp, path, module_name, indent=indent,
                                dry_run=dry_run, debug=debug,
                                skip_on_warnings=skip_on_warnings)
                if not clean and skip_on_warnings:
                    print("> skiped, warnings detected")
                    sys.exit(1)
                elif clean is None:  # failure to parse Makefile
                    with open(dst, 'w', encoding='utf8') as d:
                        d.write("*** Unable to convert this file\n")
                        sys.exit(1)
                else:
                    if not skip_tests and (path / "test" / "SUITEINFO").exists():
                        add_test_suite(tmp, path / "test", indent, simics_base)
                    with open(dst, 'w', encoding='utf8') as d:
                        tmp.seek(0)
                        d.write(tmp.read())

    print("> done. Please verify that the result matches the Makefile")

def main():
    parser = argparse.ArgumentParser(
        description="Convert a Simics device module's GNU Makefile into a"
        + " CMake CMakeLists.txt file. The new file is stored in the same"
        + " directory")
    parser.add_argument('path', type=Path)
    parser.add_argument('--dry-run', action="store_true",
        help="Don't write CMakeLists.txt, just parse Makefile and dump it to stdout")
    parser.add_argument('--debug', action="store_true")
    parser.add_argument('--skip-tests', action="store_true")
    parser.add_argument('--indent', type=int, default=2,
                        help="Amount of indentation, defaults to 2")
    parser.add_argument('--simics-base', action="store_true",
                        help="Add copy-right and other Simics Base specific things")
    parser.add_argument('--skip-on-warnings', action="store_true",
                        help="Skip creating the CMakeLists.txt file on warnings")
    args = parser.parse_args()

    path = args.path
    if "Makefile" in str(path):
        path = path.parent
    assert (path / "Makefile").exists()

    outer_convert(path, indent=args.indent,
                  dry_run=args.dry_run, debug=args.debug,
                  skip_tests=args.skip_tests,
                  simics_base=args.simics_base,
                  skip_on_warnings=args.skip_on_warnings)

if __name__ == '__main__':
    main()
