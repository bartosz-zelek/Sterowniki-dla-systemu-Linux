# © 2018 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import os
import sys
import re
import collections
import unittest
import argparse

from simicsutils.host import host_type

class DepsSyntaxError(Exception):
    def __init__(self, line, msg):
        Exception.__init__(self, msg)
        self.line = line

host_types = ['linux64', 'win64']
valid_keys = ['targets'] + ['targets-' + host for host in host_types]
class Deps:
    def __init__(self, props):
        self.props = props

    @staticmethod
    def from_file(s):
        '''
        Syntax: Line-based. Empty lines, and lines starting with #, are ignored.
        Remaining lines have a key name, followed by :, followed by a value.
        Currently the only valid value is 'targets'.
        '''
        d = {}
        # the permitted set of chars is possibly overly restrictive;
        # it can maybe be relaxed later if needed
        illegal_chars = re.compile('[^-: /+_.A-Za-z0-9]')
        for (i, line) in enumerate(s.split('\n'), 1):
            line = line.strip()
            if not line or line.startswith('#'):
                # bad chars after # is ok
                continue
            bad = illegal_chars.search(line)
            if bad:
                raise DepsSyntaxError(i, 'illegal character %r'
                                      % (bad.group(0),))
            if line.count(':') != 1:
                raise DepsSyntaxError(i, 'bad syntax, need "key: value"')
            [key, value] = line.split(':', 1)
            if key in d:
                raise DepsSyntaxError(i, '"%s" declared twice in one file'
                                      % (key,))
            if key not in valid_keys:
                raise DepsSyntaxError(i, 'Unknown key %r, must be one of %s'
                                  % (key, ', '.join(valid_keys)))
            # all known values are space-separated lists
            d[key] = value.split()
        for key in valid_keys:
            if key not in d:
                d[key] = []
        return Deps(d)

class TestParse(unittest.TestCase):
    def expect_deps(self, deps, exp):
        '''Expect that the Deps object has the properties defined by the dict
        exp, and default value for keys not in exp'''
        for k in exp:
            self.assertIn(k, deps.props)
            self.assertEqual(deps.props[k], exp[k])
        for k in deps.props:
            if k not in exp:
                self.assertEqual(deps.props[k], [])

    def test_parse(self):
        for bad_file in [
                'targets: =',
                'targets',
                'foo: bar',
                'targets : bar',
                'targets: bar\ntargets: bar']:
            with self.assertRaises(DepsSyntaxError):
                Deps.from_file(bad_file)
        self.expect_deps(Deps.from_file(''), {})
        self.expect_deps(Deps.from_file('\n  #  xyz abc  \n\n'), {})
        self.expect_deps(
            Deps.from_file('targets:  foo bar+  -a_A4./b  '),
            {'targets': ['foo', 'bar+', '-a_A4./b']})
        self.expect_deps(Deps.from_file('''
targets: x
targets-linux64: y
targets-win64: x y
'''),
                          {'targets': ['x'],
                           'targets-linux64': ['y'],
                           'targets-win64': ['x', 'y']})

def deps_for_mod(deps_filename, mod):
    with open(deps_filename) as deps_file:
        try:
            deps = Deps.from_file(deps_file.read())
        except DepsSyntaxError as e:
            print("%s:%d: error: %s" % (deps_filename, e.line, e), file=sys.stderr)
            exit(1)
    return deps.props['targets'] + deps.props['targets-%s' % (host_type(),)]

def main():
    parser = argparse.ArgumentParser('Create makefile for module dependencies')
    parser.add_argument('dest', help='makefile to create')
    parser.add_argument('basedirs', nargs='*',
                        help='base directories whose subdirs contain files'
                        + ' named "MODULEDEPS"')
    args = parser.parse_args()

    out_lines = []
    # all MODULEDEPS files
    moduledeps_files = []
    # all modules that appear as dependencies to some other module
    all_deps = set()

    # arg 1: output .d file; remaining args: base directories for
    # modules, such that [base]/*/MODULEDEPS are the dependency files
    # to scan We send the base directories instead of the full list of
    # directories, to avoid too many args on Windows
    for parent in args.basedirs:
        for mod in os.listdir(parent):
            candidate = os.path.join(parent, mod, 'MODULEDEPS')
            if not os.path.isfile(candidate):
                continue
            moduledeps_files.append(candidate)
            targets = deps_for_mod(candidate, mod)
            if targets:
                out_lines.append("%s: %s\n" % (mod, " ".join(targets)))
                all_deps.update(targets)

    # Trigger a rebuild of this depfile if any MODULEDEPS file is
    # modified
    out_lines.append('%s: %s\n'
                     % (args.dest, ' '.join(sorted(moduledeps_files))))
    # Survive and trigger rebuild also if a MODULEDEPS file is removed
    out_lines.append('%s:\n' % (' '.join(sorted(moduledeps_files)),))
    # Tolerate that a target required by some module is not present.
    # This is needed
    out_lines.append('%s:\n' % (' '.join(sorted(all_deps)),))
    destdir = os.path.dirname(args.dest)
    if not os.path.isdir(destdir):
        os.makedirs(destdir)
    with open(args.dest, "w", encoding="utf-8") as f:
        f.writelines(out_lines)

if __name__ == '__main__':
    main()
