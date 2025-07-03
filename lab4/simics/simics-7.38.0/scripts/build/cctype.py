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


import argparse
import os
import re
import subprocess
import sys
from pathlib import Path

import unittest
from unittest.mock import patch

class UnsupportedCompiler(Exception):
    pass


def run_compiler(compiler, args):
    env = os.environ.copy()
    env["LC_ALL"] = "C"
    try:
        cp = subprocess.run(compiler + args, check=True,
                            stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            env=env, encoding='utf-8')
    except OSError as e:
        raise UnsupportedCompiler(str(e)) from e
    except subprocess.CalledProcessError as e:
        raise UnsupportedCompiler(str(e)) from e
    return cp.stdout


def find(haystack, needle):
    return haystack.find(needle) != -1


def get_compiler_type(compiler):
    # First check for MSVC; we expect user to setup the environment before
    # invoking bin/make.bat and locating `cl` (or cl.exe) in PATH. This may or
    # may not work at the time when cctype.py is invoked and we don't really
    # care about the version. Let's just allow the Visual Studio compiler in
    # general
    if any(Path(e).name.lower() in {'cl', 'cl.exe'} for e in compiler):
        # We can try to be nice and run the compiler; it might work
        try:
            ccv = run_compiler(compiler, ["-v"])
        except UnsupportedCompiler:
            return ("msvc", 'unknown')
        version = re.compile(r"Version ([0-9.]+)").search(ccv).group(1)
        return ("msvc", version)

    # GCC and GCC variants below
    ccv = run_compiler(compiler, ["-v"])
    if any(c in ccv for c in ['gcc version', 'clang version', 'LLVM version']):
        # test for cygwin
        if sys.platform == "win32":
            ccdumpmachine = run_compiler(compiler, ["-dumpmachine"])
            if find(ccdumpmachine, 'cygwin'):
                raise UnsupportedCompiler(
                    "Only MingW/GCC is supported on Windows")

        # get gcc version, ignoring banner
        ccdumpversion = run_compiler(compiler, ["-dumpversion"])
        match = re.match(r"^(.* )*([\d\.]+)", ccdumpversion)
        if not match:
            raise UnsupportedCompiler("Unknown compiler")
        version = match.group(2)
        return ("gcc3", version)

    raise UnsupportedCompiler("Unknown compiler")


class TestBuildId(unittest.TestCase):
    @patch(__name__ + '.run_compiler')
    def test_get_compiler_type_mock(self, r):
        # Test that basic algorithm works with fake/mocked input. For
        # simplicity, the return value is combined for `-v` and `-dumpversion`
        # invocations
        for type in ['gcc', 'clang', 'LLVM']:
            r.return_value = f"yada yada {type} version something something 12.1.0 end"
            self.assertEqual(
                get_compiler_type(['this', 'that']), ('gcc3', '12.1.0'))

    @patch(__name__ + '.run_compiler')
    def test_get_compiler_type_msvc_mock(self, r):
        # For MSVC we look at the compiler input vector; and ignore if the
        # compiler is not currently in PATH
        for type in ['CL', 'cl', 'cl.exe']:
            r.side_effect = UnsupportedCompiler("failed to invoke")
            self.assertEqual(
                get_compiler_type(['this', type]), ('msvc', 'unknown'))

    def test_get_compiler_type(self):
        # Invoke local gcc decorated by time; two things that must always exist
        # (except it does not exist on Fedora when invoked through python's shell)
        if os.getenv("TC_CONAN") == "1":
            print("Subtest skipped in TC_CONAN=1")
            return
        ctype, cversion = get_compiler_type(['time', 'gcc'])
        self.assertEqual(ctype, 'gcc3')
        # The version of the local gcc is hard to predict; so we settle for any
        # type of version
        self.assertRegex(cversion, '[0-9][0-9.]*')


def get_parser():
    desc = 'Check compiler for Simics compatibility and print its type (or version)'
    parser = argparse.ArgumentParser(description=desc)
    parser.add_argument("compiler", nargs='+',
                        help="Compiler to test; can be decorated i.e. `ccache gcc`")
    parser.add_argument("-o", "--output", type=Path,
                        help="Create/touch file on success")
    g = parser.add_mutually_exclusive_group(required=True)
    g.add_argument("-t", "--type", action='store_true',
                   help="Print the type, e.g. 'gcc3'")
    g.add_argument("-v", "--version", action='store_true',
                   help="Print the version, e.g. '12.1.0'")
    return parser


def main():
    args = get_parser().parse_args()

    try:
        (t, v) = get_compiler_type(args.compiler)
        if args.type:
            print(t)
        if args.version:
            print(v)
    except UnsupportedCompiler as msg:
        cc = " ".join(args.compiler)
        print(f"Simics tested the compiler '{cc}'", file=sys.stderr)
        print(f"but the test failed with the following error: '{msg}'", file=sys.stderr)
        if sys.platform == "win32":
            print("""
For compiling Simics modules, you need to install the MinGW package
(as described in the Simics installation guide). By default, MinGW is
assumed to be installed in C:\\MinGW. If this is not the case, you can
change the path to the compiler by editing the file compiler.mk in
your project.""", file=sys.stderr)
        sys.exit(1)

    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text("")

if __name__ == "__main__":
    main()
