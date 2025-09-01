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
import pathlib
import py_compile
import shutil
import sys
import tempfile
import unittest

def compile_python(dst):
    try:
        py_compile.compile(dst, dst.with_suffix('.pyc'), doraise=True)
        return None
    except py_compile.PyCompileError as wrapped_exc:
        exc = wrapped_exc.exc_value
        if isinstance(exc, SyntaxError):
            return f'{exc.filename}:{exc.lineno}: error: {exc}'
        else:
            return f'{wrapped_exc.file}:1: error: {exc}'

def copy_python(src, dst):
    if src != dst:
        if os.getenv('PY_SYMLINKS'):
            # Remove broken symlinks before creating new ones
            if dst.is_symlink() and not dst.exists():
                dst.unlink()
            if not dst.is_symlink():
                if dst.exists():
                    dst.unlink()
                # Create a symlink beside the .pyc. This is a simple hack
                # that avoids the need to re-run make after editing a
                # python file. This can be convenient in an interactive
                # development cycle, but because of some rare race
                # conditions it should never be used in batch builds.
                os.symlink(src.resolve(), dst)
        else:
            if dst.is_symlink():
                dst.unlink()
            shutil.copy(str(src), str(dst))

class test_python_compile(unittest.TestCase):
    def setUp(self):
        self.d = pathlib.Path(tempfile.mkdtemp())
    def tearDown(self):
        shutil.rmtree(self.d)
    def testSuccess(self):
        import inspect
        src = self.d / 'foo.py'
        with open(src, 'w') as f:
            f.write('x=3')
        pyc = src.with_suffix('.pyc')
        def test_compile(src, pyc):
            err = compile_python(src)
            self.assertIsNone(err)
            sys.path.insert(0, str(pyc.parent))
            mod = __import__(pyc.stem)
            self.assertEqual(mod.x, 3)
            self.assertEqual(inspect.getfile(mod), str(src))
            sys.path.pop(0)

        test_compile(src, pyc)

    def testFailure(self):
        src = self.d / 'badindent.py'
        with open(src, 'w') as f:
            f.write('\n\n 0')
        err = compile_python(src)
        self.assertIsNotNone(err)
        self.assertEqual(
            err, f'{src}:3: error: unexpected indent (badindent.py, line 3)')

        src = self.d / 'null.py'
        with open(src, 'w') as f:
            f.write('\n\n\0')
        err = compile_python(src)
        expected_err = (f'{src}:1: error: source code string cannot contain'
                        ' null bytes')
        self.assertEqual(err, expected_err)
        with open(src, 'w') as f:
            f.write('raise Exception("xyz")')

def main(argv):
    parser = argparse.ArgumentParser(
        description='copy .py and optionally compile it to .pyc')
    parser.add_argument('src', help="source .py file")
    parser.add_argument('dest', nargs='?',
                        help="destination .py/.pyc file")
    parser.add_argument('--compile', action='store_true', default=False,
                        help="compile to .pyc?")
    args = parser.parse_args()
    if args.dest:
        if not args.dest.endswith('.py'):
            parser.error("dest file must end with '.py'")
        dst = pathlib.Path(args.dest)
    else:
        if not args.compile:
            parser.error("dest file or --compile must be provided")
        dst = pathlib.Path(args.src)
    src = pathlib.Path(args.src)

    os.makedirs(dst.parent, exist_ok=True)
    copy_python(src, dst)
    error = compile_python(dst) if args.compile else None

    if error is None:
        sys.exit(0)
    else:
        print(error, file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main(sys.argv)
