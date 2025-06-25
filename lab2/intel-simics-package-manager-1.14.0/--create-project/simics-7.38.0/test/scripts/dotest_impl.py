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


# Start a single test by running <test-dir>/run-test.py

import sys, os, os.path, re
from os.path import dirname
from os.path import join as pjoin
import json
import pathlib

try:
    import simicsutils
    import simicsutils.internal
except:
    print("Cannot import simicsutils. Is PYTHONPATH set correctly?")
    sys.exit(1)

def mingw_bin(root, list):
    for item in list:
        fp = pjoin(root, 'bin', item)
        if os.path.exists(fp):
            return fp
    raise Exception('%s not found in %s' % (repr(list), root))

def patch_testparams(testparams, target, host_path_value, test_dir,
                     using_package_tests = False):
    """Add functions to testparams which only makes sense in the tree or
 package test system."""
    def target_variant():
        if target in ["generic", ""]:
            return None
        else:
            return ""

    def cpu_class():
        return target

    def test_root():
        return test_dir

    def simple_config(cpuclass):
        # sparcs have special short names (for no good reason)
        m = re.match(r'sparc-(.*)', cpuclass)
        if m:
            base = m.group(1)
        elif cpuclass.startswith('x86'):
            base = 'x86'
        else:
            base = cpuclass
        return pjoin(test_dir, "simple", base + ".py")

    def dumps_path():
        return simicsutils.internal.dumps_path()

    def testfiles_path():
        return simicsutils.internal.testfiles_path()

    def package_path():
        return simicsutils.internal.package_path(testparams.simics_repo_root())

    def external_package_path():
        return simicsutils.internal.external_package_path()

    def simics_dist_config():
        return pjoin(testparams.simics_repo_root(),
                     "config", "dist", "config.json")

    def host_path():
        return host_path_value

    def simics_package_specs_file():
        return (pathlib.Path(testparams.simics_repo_root()) / host_path()
                / "package-specs-base.json")

    def simics_package_specs():
        return json.loads(simics_package_specs_file().read_text())

    # Root of MingW gcc (not rest of MingW)
    def mingw_root():
        host = simicsutils.host.host_type()
        path = pjoin(testparams.simics_repo_root(), 'core', host,
                     'sys', host, 'mingw-root-native')
        return open(path).read().strip()

    def mingw_cc():
        return mingw_bin(mingw_root(), ['gcc.exe', 'mingw32-gcc.exe'])

    def mingw_cxx():
        return mingw_bin(mingw_root(), ['g++.exe', 'mingw32-g++.exe'])

    def is_package_test():
        return using_package_tests

    def make_path():
        if simicsutils.host.is_windows():
            mingw_prefix = [
                # STH-internal install path
                r"d:\usr\mingw\itm-mingw\11.1.0-1",
                # External preferred path with 11.1
                r"c:\mingw64\mingw",
                # External path with 4.9.2
                r"c:\mingw64",
            ]
            for p in mingw_prefix:
                if os.path.isdir(p):
                    return mingw_bin(p, ['make.exe', 'mingw32-make.exe'])
        else:
            return package_path() + "/make-4.1/bin/make"

    extra = [
        testfiles_path,
        simics_dist_config,
        simics_package_specs,
        simics_package_specs_file,
        package_path,
        external_package_path,
        host_path,
        dumps_path,
        simple_config,
        cpu_class,
        target_variant,
        mingw_root,
        mingw_cc,
        mingw_cxx,
        test_root,
        is_package_test,
        make_path,
    ]
    for f in extra:
        setattr(testparams, f.__name__, f)

    # Only modify testparams.__all__ once.
    if 'begin_subtest' in testparams.__all__:
        return

    testparams.__all__ += [f.__name__ for f in extra]
    # Add extra internal API
    testparams.__all__ += [
        'begin_subtest', 'end_subtest', 'report_success', 'report_failure',
        'report_timeout', 'report_run', 'pr', 'results_from_file',

        'run_simics', 'run_testfun',
        'run_multi_subtest', 'run_subtest', 'run_subtests',

        'test_log_file',
        ]

def main():
    try:
        (host_arch, target,
         host_dir, test_root, log_root,
         test_name) = sys.argv[1:7]
    except Exception:
        print(("Usage: %s host_arch target host_dir test_dir log_dir test_name"
               % os.path.basename(sys.argv[0])))
        print("where:")
        print("  host_arch   host architecture")
        print("  target      target cpu")
        print("  host_dir    host directory (absolute)")
        print("  test_dir    test script directory (absolute)")
        print("  log_dir     test log directory (absolute)")
        print("  test_name   test name")
        sys.exit()

    if len(sys.argv) >= 8:
        logdir =  sys.argv[7]
    else:
        logdir = ""

    if len(sys.argv) >= 9:
        project = sys.argv[8]
    else:
        project = ""

    if len(sys.argv) >= 10:
        simics_base = sys.argv[9]
    else:
        simics_base = ""

    using_package_tests = False
    if len(sys.argv) >= 11:
        using_package_tests = sys.argv[10] == "True"

    # Use alternate output if DOTEST_VERBOSE is set in the environment
    verbose = os.getenv("DOTEST_VERBOSE")
    if verbose:
        print("DOTEST", host_arch, target, host_dir, test_root, log_root, \
              test_name)

    test_resource_dir = os.path.abspath(dirname(dirname(
        sys.argv[0])))

    if not project:
        project = dirname(host_dir)

    if not simics_base:
        simics_base = dirname(test_resource_dir)

    simics_repo_root = dirname(simics_base)
    sys.path.append(pjoin(simics_base, "scripts"))
    sys.path.append(pjoin(simics_base, "scripts", "dist"))
    sys.path.append(pjoin(test_resource_dir, "scripts"))

    testdir = pjoin(test_root, test_name)
    if not logdir:
        logdir = pjoin(log_root, host_arch, test_name, target)
    import testparams
    testparams.setup(simics_base, host_arch, project, test_root,
                     None, s_in_lab = True, simics_repo_root=simics_repo_root)
    patch_testparams(testparams, target, host_dir, test_root,
                     using_package_tests)

    (ntests, failures) = testparams.run_tests(testdir, logdir, verbose)
    print("======= End of", ntests, "tests", test_name, "===========")
    sys.stdout.flush()
    sys.exit(failures != 0)

if __name__ == '__main__':
    main()
