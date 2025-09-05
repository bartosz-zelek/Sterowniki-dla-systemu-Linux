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


# Script to run tests in a project
# For each failing test or test which times out, print a reference to the
# log file and line where the test starts its logging
# Return 0 if all tests pass, otherwise return 1


import itertools
import argparse
import os
import re
import subprocess
import sys
import time
import signal
import pathlib

# If we run from a Simics installation we need some modules in the Simics base
# package, so we need to update the python search path.
if "SIMICS_BASE_PACKAGE" in os.environ:
    simics_base = os.environ["SIMICS_BASE_PACKAGE"]
    sys.path.append(os.path.join(simics_base, "scripts"))
else:
    simics_base = os.path.dirname(os.path.dirname(sys.argv[0]))
import project as prj
import lookup_file
import host_type
import simicsutils.host
from suiteinfo import SuiteinfoException, parse_suiteinfo, parse_xfail

_text_description = '''List or run Simics tests.

Simics tests are placed in *test suites*, which are directories containing a
`SUITEINFO` file. By default, the `test-runner` command searches for test
suites in the directories `test`, `targets` and `modules` of the Simics
project, and in the `test` directory of installed Simics packages.

If the program is run with the flag `--list`, the names of all found test
suites are listed:
```
$ ./bin/test-runner --list
modules/bar/test
modules/foo/test
test/t15_checkpoints: [simics]/test/t15_checkpoints
```

The name of a suite is the path of the directory, relative to the project
or Simics installation path, using / as directory separator. If the test
suite is located in the Simics installation, then the absolute path is
printed as well.

The flag `--test` can be used to list the individual tests of each test suite:
```
$ ./bin/test-runner --tests --suite=modules/bar/test
Suite: modules/foo/test
    s-foo
    s-info-status
```

If neither `--list` nor `--tests` is passed, all found test suites will be
run. For example, the command `./bin/test-runner --suite=modules/bar/test`
will run all tests of the test suite `modules/bar/test`. If `test-runner`
is run with no arguments, then all tests in all test suites will be run.

Other than the argument `--suite`, a *suite pattern* can also be used to
filter test suites. If one or more suite patterns are provided, only the
suites matching at least one of the patterns will be run. A suite matches
a suite pattern if the pattern is a substring of the suite's name. For
example, if there is a suite `modules/foo/test`, then the command
`./bin/test-runner foo` will run that suite, and possibly other suites as well.

When an environment variable `TEST_PATTERN` is set, the value will be used as
a pattern to filter the tests to be run within the test suite. The pattern is
interpreted according to Python's regular expression syntax. For example, if
the variable is set as `export TEST_PATTERN="s-.+DMA-"`, then `test-runner`
will run tests named `s-xyz-DMA-read` and `s-abc-DMA-write`, and skip tests
named `Xs-xyz-DMA-read` or `s-abc-DMAwrite`.

The variable can also be set to multiple patterns with the semicolon as a
separator, e.g. `export TEST_PATTERN="pattern0;pattern1"`. This will run tests
that match any one of the given patterns.

By default, all tests run sequentially. When the `-j` option is passed,
automatically added `s-*.py` scripts, may be executed in parallel. The tests
will be executed in an unspecified order, so `-j` should only be used if you
know that there are no dependencies between the tests of the suite.
'''


def _extract_options(parser):
    def _extract_action(a):
        vstr = ' ' + a.metavar if a.metavar else ''
        return ', '.join(['%s%s' % (ostr, vstr) for ostr in a.option_strings])
    strings = []
    for action in parser._optionals._group_actions:
        if action.help == argparse.SUPPRESS:
            continue
        strings.append('''<dt><tt>%s</tt></dt>
<dd>%s</dd>''' % (_extract_action(action), action.help))
    return '''
<dl>
%s
</dl>''' % ('\n'.join(strings),)


_exit_code_desc = '''
The exit status is 0 if selected test suites succeed, 1 if there is an
internal error, and 2 if any test fails.
'''


def _extract_docu(parser):
    return '''
# test-runner
<article class="not-numbered not-in-toc doc-item">

## SYNOPSIS
```
test-runner [options] [suite-pattern ...]
```

## DESCRIPTION
%s

## OPTIONS
%s

## RETURN VALUE
%s

## SEE ALSO
The test framework in the *API Reference Manual*.

</article>
''' % (_text_description, _extract_options(parser), _exit_code_desc)


# A simple formatter to print the description nicer, it may not work well
# for all cases, so that manual testing is required if any changes to
# _text_description have been made.
def _format_print_desc(desc):
    def _substr(items):
        i = 0
        sum = 0
        while i < len(items):
            sum += len(items[i]) + (0 if i == 0 else 1)
            if i == (len(items) - 1) or (sum + 1 + len(items[i + 1])) > 75:
                break
            i += 1
        return (' '.join(items[0:i+1]), items[i+1:])

    def _untag(block):
        return re.sub('`', '', block)

    def _format(block):
        sections = block.split('\n\n')
        for section in sections:
            words = section.split()
            while len(words):
                line, words = _substr(words)
                print('    ' + line)
            print()

    index = 0
    while True:
        start = desc.find('```')
        if start == -1:
            break
        _format(_untag(desc[0:start]))
        end = desc.find('```', start + 3)
        for l in desc[start+4:end].splitlines():
            print('        ' + l)
        print()
        desc = desc[end + 4:]
        index += 1
    _format(_untag(desc))

class TestSuite:
    def __init__(self, name, suite_dir, installed, test_dir):
        self.name = name
        self.suite_dir = os.path.abspath(os.path.realpath(suite_dir))
        self.installed = installed
        self.test_dir = test_dir
        f = None
        try:
            filename = os.path.join(self.suite_dir, "SUITEINFO")
            f = open(filename)
            for (key, value) in list(parse_suiteinfo(filename, f).items()):
                setattr(self, key, value)
            self.exception = None
        except SuiteinfoException as e:
            # Other code uses tags and disabled_on_hosts for filtering, let's
            # provide default values to avoid errors in that step
            self.tags = []
            self.disabled_on_hosts = []
            self.exception = e
        finally:
            if f:
                f.close()
        try:
            filename = os.path.join(self.suite_dir, "XFAIL")
            f = open(filename)
            self.xfail = parse_xfail(filename, f)
        except IOError:
            self.xfail = {}

    def get_abs_path(self):
        return self.suite_dir

    def matches_patterns(self, suite_patterns):
        for r in suite_patterns:
            tmp = self.name + os.sep
            if r in tmp: return True
        return False

    def has_tag(self, tag):
        return tag in self.tags

    def get_name(self):
        return self.name

    def get_timeout(self):
        return self.timeout

    def get_test_dir(self):
        return self.test_dir

    def test_is_xfailed(self, test_name, target):
        if test_name not in self.xfail:
            return False
        host_type = simicsutils.host.host_type()
        fail_host = self.xfail[test_name]['host']
        fail_targ = self.xfail[test_name]['target']
        return (host_type in fail_host if fail_host else True) and \
            (target in fail_targ if fail_targ else True)

# root: the root directory of a project or Simics package to search test suites
# path: the relative path inside the root directory
# installed: true when the root is path of a Simics package
def get_suites_in_directory(root, path, installed):
    assert os.path.isabs(root)
    absdir = os.path.join(root, path)
    if not os.path.isdir(absdir):
        return []
    return [TestSuite(os.path.relpath(dirpath, root), dirpath,
                      installed, absdir)
            for (dirpath, dirnames, filenames) in walk(absdir)
            if 'SUITEINFO' in filenames]

class Simics:
    def __init__(self, simics_base_dir, package_list, project):
        self.base = simics_base_dir
        self.package_list_file = package_list
        self.package_list = lookup_file.PackageList(package_list)
        self.host_type = self.get_host_type()
        self.python = project.get_filename("bin", "mini-python"
                                           + simicsutils.host.batch_suffix())
        exe_suffix = ["", ".exe"][simicsutils.host.is_windows()]
        self.reaper = os.path.join(simics_base_dir, self.host_type, "bin",
                                   "reaper" + exe_suffix)
        self.test_single_suite_py = self.get_filename("test", "scripts",
                                                      "test_single_suite.py")

    def get_base_path(self):
        return self.base

    def get_filename(self, *path):
        """Lookup a filename composed of the parts in path in simics."""
        return self.package_list.lookup_path_in_packages(os.path.join(*path))

    def get_host_type(self):
        return host_type.host_type(self.base)

    def get_test_single_suite_arguments(self, emacs_mode, projectdir, logdir,
                                        suitedir, names, timeout,
                                        target, testdir, jobs, keep_scratch):
        reaper = [] if emacs_mode else [self.reaper, "-t", str(timeout), "-r"]
        return (reaper + [self.python]
                + [self.test_single_suite_py, self.base,
                   logdir, suitedir,
                   "--project=%s" % projectdir,
                   "--jobs=%d" % jobs]
                + (["--package-list", self.package_list_file]
                   if self.package_list_file else [])
                + (["--target=%s" % target] if target else [])
                + ["--testdir=%s" % testdir]
                + ["--test=%s" % (name,) for name in names]
                + (["--keep-scratch"] if keep_scratch else []))

    def get_suites(self):
        """Find test suites in the simics installation.
        """
        test_dirs = self.package_list.find_dir_in_all_packages("test")
        return set(itertools.chain(
            *(get_suites_in_directory(
                os.path.dirname(td), os.path.basename(td), True)
              for td in test_dirs)))

    def get_suite_name_for_path(self, path):
        test_dirs = self.package_list.find_dir_in_all_packages("test")
        for td in test_dirs:
            d = os.path.relpath(path, td)
            if not d.startswith('..'):
                return os.path.join('test', d)
        return None

def walk_follow_symlinks(root):
    def is_cyclic(path):
        '''Return true if path contains a cycle
        (via symlinks, typically)'''
        def parent_dirs(path):
            next = os.path.dirname(path)
            while next != path:
                yield next
                path = next
                next = os.path.dirname(path)
        return any(os.path.samefile(path, parent)
                   for parent in parent_dirs(path))
    for root, dirs, files in os.walk(root):
        yield (root, dirs, files)
        for path in (os.path.join(root, dir) for dir in dirs):
            if os.path.islink(path) and not is_cyclic(path):
                for x in walk_follow_symlinks(path):
                    yield x

# walk(path): Similar to os.walk(path, followlinks=True), but takes
# care of cycles in a better way. See bug 13463.
if hasattr(os.path, 'islink') and hasattr(os.path, 'samefile'):
    walk = walk_follow_symlinks
else:
    walk = lambda path: os.walk(path)

class AbstractProject:
    suite_roots = []

    def __init__(self, path):
        self.path = os.path.realpath(path)

    def get_path(self):
        return self.path

    def get_suites(self):
        """Find all test suites in the project.
        """
        return itertools.chain(*(get_suites_in_directory(self.path, td, False)
                                 for td in self.suite_roots))

    def get_filename(self, *path):
        """Lookup a filename in the project."""
        return os.path.join(self.path, *path)

    def get_suite_name_for_path(self, path):
        d = os.path.relpath(path, self.path)
        return None if d.startswith('..') else d


class Project(AbstractProject):
    def __init__(self, path, base_dir="modules", module_dirs=None):
        AbstractProject.__init__(self, path)

        roots = {pathlib.Path(d) for d in [base_dir, "test", "targets"]}
        roots.update(pathlib.Path(d) for d in module_dirs or [])
        self.suite_roots = roots


class SimicsTree(AbstractProject):
    suite_roots = ["test", os.path.join("src", "devices"),
                   os.path.join("src", "components"),
                   os.path.join("src", "extensions"), "targets"]


class Logdir:
    def __init__(self, logdir, emacs_mode):
        self.logdir = logdir
        self.emacs_mode = emacs_mode

    def get_logdir_for_suite(self, suite_name, target):
        if self.emacs_mode:
            if target:
                suite_dir = os.path.join(os.path.basename(suite_name), target)
            else:
                suite_dir = os.path.basename(suite_name)
        else:
            if target:
                suite_dir = os.path.join(suite_name, target)
            else:
                suite_dir = suite_name
        return os.path.join(self.logdir, suite_dir)

    def get_logfile_for_suite(self, suite_name, target):
        return os.path.join(self.get_logdir_for_suite(suite_name, target),
                            "test.log")

class TestRunner:
    """Runs tests and reports the result to an output stream."""

    _res_line = re.compile(r"RESULT ([^ ]+) (.*)")
    _start_line = re.compile(r"BEGIN\s+([!-~]+)")

    def __init__(self, simics, projectdir, logdir, verbose=False, jobs=1,
                 emacs=False, keep_scratch=False):
        self.simics = simics
        self.projectdir = projectdir
        self.logdir = logdir
        self.verbose = verbose
        self.jobs = jobs
        self.emacs = emacs
        self.keep_scratch = keep_scratch

    def _test_single_suite(self, reporter, suite, names, target):
        def signal_handler(number, frame):
            # We need to trap the SIGINT signal in order to not kill the
            # test_runner itself. The signal will be automatically propagated
            # to all children so no need to resend it, in fact, sending it will
            # not work as we will then send two signals to the child.
            pass
        if self.verbose and not self.emacs:
            print('==', suite.get_name(), '==')
        reporter.start_suite(suite)
        args = self.simics.get_test_single_suite_arguments(
            self.emacs,
            self.projectdir,
            self.logdir.get_logdir_for_suite(suite.get_name(), target),
            suite.get_abs_path(),
            names,
            suite.get_timeout(),
            target,
            suite.get_test_dir(),
            self.jobs,
            self.keep_scratch)
        env = os.environ.copy()
        env['DOTEST_VERBOSE'] = 't'
        if self.emacs:
            old_handler = signal.signal(signal.SIGINT, signal_handler)
        popen = subprocess.Popen(args, env=env, stdout=subprocess.PIPE,
                                 text=True, encoding='utf-8')
        hasFailure = False
        while True:
            line = popen.stdout.readline()
            if line == "": break
            failed = self._handle_output_line(reporter, suite, line)
            hasFailure = hasFailure or failed
        res = popen.wait()
        if self.emacs:
            signal.signal(signal.SIGINT, old_handler)
        if res == 1:
            reporter.suite_timeout(suite)
        elif res != 0 and not hasFailure:
            reporter.suite_failure(suite)
        reporter.end_suite()

    def _handle_output_line(self, reporter, suite, line):
        if self.emacs:
            reporter.line_output(line)
            return False
        if self.verbose:
            reporter.wr(line)
        test_start_match = self._start_line.match(line)
        if test_start_match:
            reporter.start_test(test_start_match.group(1))
        match = self._res_line.match(line)
        if match is None:
            return False
        test_name = match.group(1)
        reason = match.group(2)
        if match.group(2) == "*** timeout ***":
            reporter.test_timeout(suite, test_name, reason, self.verbose)
            return True
        elif match.group(2).startswith("*"):
            reporter.test_failed(suite, test_name, reason, self.verbose)
            return True
        else:
            reporter.test_success(suite, test_name, reason, self.verbose)
            return False

    def run_tests(self, reporter, suites, names, target):
        """Run the provided suites and report to the reporter.

        Returns 0 if all tests passed and 1 otherwise."""
        reporter.start_test_run()
        for suite in suites:
            if suite.exception:
                reporter.start_suite(suite)
                reporter.add_error("BROKEN SUITE %s: %s"
                                   % (suite.get_name(), str(suite.exception)))
                reporter.end_suite()
            else:
                self._test_single_suite(reporter, suite, names, target)
        reporter.end_test_run()

    def list_tests(self, suites, target):
        """List tests for provided suites and patterns."""
        mb = os.path.dirname(os.path.dirname(sys.argv[0]))
        test_resource_dir = os.path.join(mb, 'test')
        sys.path.append(os.path.join(test_resource_dir, "scripts"))
        import testparams
        import dotest_impl
        testparams.setup(self.simics.get_base_path(),
                         simicsutils.host.host_type(),
                         self.projectdir, test_resource_dir, None)
        for suite in suites:
            print('Suite:', suite.get_name())
            if suite.exception:
                print('*** ERROR:', suite.exception)
            dotest_impl.patch_testparams(
                testparams, target,
                os.path.join(self.projectdir, simicsutils.host.host_type()),
                suite.get_test_dir())
            try:
                _suite = testparams.find_tests(suite.get_abs_path())
                if _suite is None:
                    print("    can't list tests for suites with run-test.py")
                elif _suite.get_tests():
                    for t in _suite.get_tests():
                        print('    ' + (t.get_name()))
                else:
                    print("    no tests found")
            except testparams.TestFailure as e:
                print('*** ERROR:', e)

class TestReporter:
    def __init__(self, stream, logdir, target):
        self.stream = stream
        self.logdir = logdir
        self.target = target
        self.suite = None

    def _test_is_xfailed(self, test_name, suite=None):
        if not suite and not self.suite:
            return False
        suite = suite if suite else self.suite
        return suite.test_is_xfailed(test_name, self.target)

    def start_test_run(self):
        self.failures = []
        self.timeouts = []
        self.start_time = time.time()
        self.test_count = 0
        self.suite_count = 0

    def start_suite(self, suite):
        self.suite_count += 1
        self.suite = suite

    def suite_timeout(self, suite):
        self.wr("T")
        self.add_timeout(suite, None, "suite timed out")

    def suite_failure(self, suite, reason="suite failed"):
        self.wr("F")
        self.add_failure(suite, None, reason)

    def end_suite(self):
        self.suite = None

    def start_test(self, test_name):
        pass

    def test_success(self, suite, test_name, reason, verbose=False):
        xfailed = self._test_is_xfailed(test_name, suite)
        if not verbose:
            self.wr("f" if xfailed else ".")
        if xfailed:
            self.add_failure(suite, test_name, 'Unexpected success')
        self.test_count += 1

    def test_failed(self, suite, test_name, reason, verbose=False):
        xfailed = self._test_is_xfailed(test_name, suite)
        if not verbose:
            self.wr("." if xfailed else "f")
        if not xfailed:
            self.add_failure(suite, test_name, reason)
        self.test_count += 1

    def test_timeout(self, suite, test_name, reason, verbose=False):
        xfailed = self._test_is_xfailed(test_name, suite)
        if not verbose:
            self.wr("." if xfailed else "t")
        if not xfailed:
            self.add_timeout(suite, test_name, reason)
        self.test_count += 1

    def end_test_run(self):
        end_time = time.time()
        self.pr()
        for f in self.failures:
            self.pr(f)
        for t in self.timeouts:
            self.pr(t)
        self.pr("Ran %d tests in %d suites in %f seconds."
                % (self.test_count, self.suite_count,
                   end_time - self.start_time))
        if len(self.failures) == 0 and len(self.timeouts) == 0:
            self.pr("All tests completed successfully.")
        else:
            self.pr("Failures: %d  Timeouts: %d" % (len(self.failures),
                                                    len(self.timeouts)))


    ## Helper methods for the test reporting

    def get_failure_count(self):
        return len(self.failures) + len(self.timeouts)

    def pr(self, *strings):
        print(" ".join(strings), file=self.stream)
        self.stream.flush()

    def wr(self, string):
        self.stream.write(string)
        self.stream.flush()

    def get_failure_string(self, suite, test_name, reason):
        suite_name = suite.get_name() if suite else "unknown"
        location = self.get_test_result_location(suite_name, test_name)
        if test_name is None:
            return "%s: test suite %s failed (%s)" % (location,
                                                      suite_name, reason)
        else:
            return "%s: test %s in %s failed (%s)" % (location, test_name,
                                                      suite_name, reason)

    def add_error(self, reason):
        self.wr("!")
        self.failures.append(reason)

    def add_failure(self, suite, test_name, reason):
        self.failures.append(self.get_failure_string(suite, test_name, reason))

    def add_timeout(self, suite, test_name, reason):
        self.timeouts.append(self.get_failure_string(suite, test_name, reason))

    def get_test_result_location(self, suite_name, test_name=None):
        logfile = self.logdir.get_logfile_for_suite(suite_name, self.target)
        if not test_name:
            return logfile
        line_count = 1
        for line in open(logfile, 'rb'):
            if line.startswith(b"=BEGIN %s" % test_name.encode('utf-8')):
                return "%s:%d" % (logfile, line_count)
            line_count += 1
        return logfile

class EmacsTestReporter(TestReporter):
    def __init__(self, stream, logdir, target):
        TestReporter.__init__(self, stream, logdir, target)

    def pr(self, *strings):
        self.stream.flush()

    def wr(self, string):
        self.pr()

    def line_output(self, string):
        self.stream.write(string)
        self.pr()

def filter_suites(suites, suite_patterns, host_type, tags):
    return (s for s in suites if ((suite_patterns == []
                                   or s.matches_patterns(suite_patterns))
                                  and host_type not in s.disabled_on_hosts
                                  and (tags == []
                                       or any(s.has_tag(t) for t in tags))))

def merge_suites(*suite_collections):
    """Merge the suite collections and only leave one suite for each name.
    The first suite with a given name will be used.
    """
    suites = itertools.chain(*suite_collections)
    index = {}
    for s in suites:
        if s.get_abs_path() not in index:
            index[s.get_abs_path()] = s
    return list(index.values())

def print_suites(suites):
    local = []
    installed = []
    for s in suites:
        if s.exception:
            error_msg = " ERROR: " + str(s.exception)
        else:
            error_msg = ""
        if s.installed:
            installed.append("%s: %s" % (s.get_name(), s.get_abs_path())
                             + error_msg)
        else:
            local.append(s.get_name() + error_msg)
    for l in sorted(local) +   sorted(installed):
        print(l)

def report_error_then_exit(msg, exit_code):
    print(msg, file=sys.stderr)
    exit(exit_code)

def remove_trailing_separator(s):
    if s[-1] in ['/', '\\']:
        s = s[:-1]
        return remove_trailing_separator(s)
    return s

def is_simics_script(f):
    return os.path.isfile(f) and (f.endswith('.py') or f.endswith('.simics'))

def create_argument_parser(base_default):
    usage = 'test-runner [options] [substring] ...'
    desc = 'List or run Simics functional tests.'
    parser = argparse.ArgumentParser(usage=usage, description=desc)
    parser.add_argument("suite_patterns", nargs="*", help=argparse.SUPPRESS)

    parser.add_argument("--project", dest="projectdir",
                        type=str, default=None,
                        help=argparse.SUPPRESS)
    parser.add_argument("-p", "--package-list", dest="package_list",
                        type=str, default=None,
                        help=argparse.SUPPRESS)
    parser.add_argument("--docu", dest='docu', action="store_true",
                        default=False, help=argparse.SUPPRESS)
    parser.add_argument("--in-tree", dest="in_tree", action="store_true",
                        default=False, help = argparse.SUPPRESS)

    parser.add_argument("--details", dest="details", action="store_true",
                        default=False, help = "print detail help information")

    parser.add_argument("--logdir", dest="logdir", default=None,
                        type = str,
                        help="the directory tests save logfiles in, defaults to"
                             " [project]/logs/tests")
    parser.add_argument("--basedir", dest="basedir", default=base_default,
                        type = str,
                        help="the base directory where the modules containing"
                             " the tests exist, defaults to 'modules'")
    parser.add_argument("--moduledirs", dest="moduledirs", default='',
                        type=lambda dirs: [d for d in dirs.split(";") if d],
                        help="directories with modules containing tests,"
                             "separated by ';'")
    parser.add_argument("--project-only", dest='project_only',
                        action="store_true", default=False,
                        help="disable the tests from Simics packages")

    parser.add_argument("-l", "--list", dest="list", action="store_true",
                        default=False,
                        help="list the suites matching the patterns")
    parser.add_argument("-t", "--tests", dest="tests", action="store_true",
                        default=False,
                        help="list tests matching the given patterns")
    parser.add_argument("--tag", dest="tags", action="append",
                        default=[], type=str, metavar='TAG',
                        help = "Run all suites where a tag in SUITEINFO"
                        + " matches TAG")
    parser.add_argument("--suite", dest="suites", default=[], action="append",
                        type=str, metavar='SUITE',
                        help="specify a test suite to be executed")
    parser.add_argument("-n", "--name", dest="names", default=[],
                        action="append", type=str, metavar='NAME',
                        help="specify a test name to be executed")
    parser.add_argument("-j", "--jobs", dest="jobs",
                        type=int, default=1, nargs="?", metavar="N",
                        help="run up to N Simics script tests in parallel in"
                             " each test suite; N will be automatically set"
                             " to the number of processors on the machine if"
                             " not specified")
    parser.add_argument("-v", "--verbose", dest="verbose",
                        action="store_true", default=False,
                        help="print more verbose information when running")
    parser.add_argument("--target", dest="target",
                        help=argparse.SUPPRESS, default="")
    parser.add_argument("--emacs", dest="emacs", action="store_true",
                        help=argparse.SUPPRESS, default=False)
    parser.add_argument("--keep-scratch", dest="keep_scratch",
                        action="store_true", help=argparse.SUPPRESS,
                        default=False)
    parser.add_argument("--py3", dest="py3",
                        action="store_true", help="Deprecated, no effect.",
                        default=False)
    parser.add_argument("--py2", dest="py2",
                        action="store_true", help="Deprecated, no effect.",
                        default=False)
    return parser

def main():
    base_default = "modules"
    parser = create_argument_parser(base_default)
    args = parser.parse_args()


    if args.docu:
        print(_extract_docu(parser))
        exit(0)

    if args.details:
        _format_print_desc(_text_description + "\n" + _exit_code_desc)
        exit(0)

    if args.verbose and not args.emacs:
        print("Using Simics at %s" % simics_base)

    if args.in_tree:
        project = SimicsTree(simics_base)
    else:
        if args.projectdir is None:
            report_error_then_exit("No project detected.", 1)
        projectdir = os.path.abspath(args.projectdir)
        if not prj.is_project_path(projectdir):
            report_error_then_exit("%s is not a valid project directory."
                                  % projectdir, 1)
        if (args.basedir != base_default
            and not os.path.exists(os.path.join(args.projectdir,
                                                args.basedir))):
            report_error_then_exit("%s is not a valid path." % args.basedir, 1)
        project = Project(projectdir, args.basedir, args.moduledirs)

        if args.verbose and not args.emacs:
            print("Using project at %s" % projectdir)
            print("Using base directory %s" % args.basedir)
    simics = Simics(simics_base, args.package_list, project)
    host_type = simics.get_host_type()

    if args.logdir:
        logdir = os.path.abspath(args.logdir)
    else:
        logdir = project.get_filename("logs", "test", host_type)
    logdir = Logdir(logdir, args.emacs)

    if args.emacs:
        reporter = EmacsTestReporter(sys.stdout, logdir, args.target)
    else:
        reporter = TestReporter(sys.stdout, logdir, args.target)
    test_runner = TestRunner(simics, project.get_path(), logdir,
                             args.verbose, args.jobs or 0, args.emacs,
                             args.keep_scratch)

    if len(args.suites) != 0 and len(args.suite_patterns) != 0:
        report_error_then_exit(
                "Don't specify both suite name and suite patterns!", 1)

    if (len(args.suite_patterns) == 1
        and is_simics_script(args.suite_patterns[0])):
        # For backward compatibility, we allow a single Simics/Python script to
        # be passed as script pattern
        pattern = args.suite_patterns[0]
        path = os.path.dirname(os.path.abspath(pattern))
        suite = (project.get_suite_name_for_path(path)
                 or simics.get_suite_name_for_path(path))
        if suite:
            args.suite_patterns = [suite]
            args.names = [os.path.basename(pattern)]
    try:
        suites = list(filter_suites(
            project.get_suites(), args.suite_patterns, host_type, args.tags))
        if not args.project_only:
            suites += list(filter_suites(
                simics.get_suites(), args.suite_patterns, host_type, args.tags))
    except SuiteinfoException as e:
        report_error_then_exit(e.args[0], 1)

    if len(args.suites) > 0:
        suite_args = list(map(remove_trailing_separator, args.suites))
        suites = [suite for suite in suites if suite.get_name() in suite_args]

    if len(suites) == 0:
        print("No tests")
        exit(0)
    suites = merge_suites(suites)  # Remove duplicates
    if args.list:
        print_suites(suites)
    elif args.tests:
        test_runner.list_tests(suites, args.target)
    else:
        test_runner.run_tests(reporter, suites, args.names, args.target)
        if reporter.get_failure_count() > 0:
            exit(2)

if __name__ == "__main__":
    main()
