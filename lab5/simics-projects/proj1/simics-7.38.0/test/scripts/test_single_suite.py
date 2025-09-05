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


# Start a single test suite by running <test-dir>/tests.py or simics tests
# with the pattern s-*.py

import sys, os, os.path, optparse, simicsutils.host
import testparams
import dotest_impl

if 'SIMICS_HOST' in os.environ:
    host_arch = os.environ['SIMICS_HOST']
else:
    host_arch = simicsutils.host.host_type()

def normalize_test_names(suitedir, names):
    subtests = []
    for n in names:
        if ((n.endswith('.py') or n.endswith('.simics'))
             and os.path.isfile(os.path.join(suitedir, n))):
            subtests.append(testparams.script_to_name(n))
        else:
            subtests.append(n)
    return subtests

def main():
    usage = """usage: %prog [options] simics_base log_dir suitedir

Arguments:
  simics_base the root of the simics tree or simics-base package (absolute)
  log_dir     test log directory (absolute)
  suitedir    the directory of the suite (absolute)"""
    parser = optparse.OptionParser(usage = usage)
    parser.add_option("-w", "--project", dest="project",
                      help="specify a project to use", metavar="PROJECT",
                      default=None)
    parser.add_option("--workspace", dest="project",
                      help=optparse.SUPPRESS_HELP, default=None)
    # TODO: remove argument --list in Simics 6
    parser.add_option("-l", "--list", dest="list", action="store_true",
                      default=False, help=optparse.SUPPRESS_HELP)
    parser.add_option("-t", "--test", dest="tests", metavar="TEST",
                      action="append", default=[],
                      help="add test to run")
    parser.add_option("-p", "--package-list", dest="package_list",
                      help = optparse.SUPPRESS_HELP, default=None)
    parser.add_option("-j", "--jobs", dest="jobs",
                      type="int", default=1,
                      help="run up to N Simics script tests in parallel",
                      metavar="N")
    parser.add_option("--target", dest="target",
                      help=optparse.SUPPRESS_HELP, default="")
    parser.add_option("--testdir", dest="test_dir",
                      help=optparse.SUPPRESS_HELP, default="")
    parser.add_option("--keep-scratch", dest="keep_scratch",
                      action="store_true", help=optparse.SUPPRESS_HELP,
                      default="")

    options, args = parser.parse_args()
    project = options.project
    try:
        (simics_base, logdir, suitedir) = args
    except Exception:
        parser.print_help()
        sys.exit()

    if options.tests:
        subtests = normalize_test_names(suitedir, options.tests)
        def test_filter(test):
            return test.get_name() in subtests
    else:
        test_filter = testparams.test_pattern_test_filter

    sys.path.append(os.path.join(simics_base, "scripts"))
    test_resource_dir = os.path.abspath(os.path.dirname(os.path.dirname(
        sys.argv[0])))
    sys.path.append(os.path.join(test_resource_dir, "scripts"))
    host_dir = os.path.join(project, host_arch)
    test_root = options.test_dir
    test_name = os.path.basename(suitedir)

    # Use alternate output if DOTEST_VERBOSE is set in the environment
    verbose = os.getenv("DOTEST_VERBOSE")
    if verbose:
        print("DOTEST", host_arch, options.target, host_dir, test_root, logdir, test_name)

    testparams.setup(simics_base, host_arch, project, test_root,
                     options.package_list)
    dotest_impl.patch_testparams(testparams, options.target,
                                 host_dir, options.test_dir)

    if options.list:
        suite = testparams.find_tests(suitedir)
        if suite and suite.get_tests():
            for t in suite.get_tests():
                if test_filter(t):
                    print('    ' + t.get_name())
        else:
            print("No tests found")
    else:
        ntests, failures = testparams.run_tests(suitedir, logdir, True,
                                                test_filter, options.jobs,
                                                options.keep_scratch)
        if verbose:
            print("======= End of", ntests, "tests", test_name, "===========")
        sys.stdout.flush()
        sys.exit(failures != 0)

if __name__ == '__main__':
    main()
