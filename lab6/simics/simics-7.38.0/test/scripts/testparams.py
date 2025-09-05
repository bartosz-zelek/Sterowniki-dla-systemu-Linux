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


#:: doc testparams {{
# Information about the test environment and some test utilities.
# }}


import sys, os, os.path, subprocess, re, glob, errno, time, codecs
import traceback, shutil
import threading
import signal
import stat
import tempfile
from simicsutils.host import is_windows, batch_suffix
import importlib
import importlib.util
import queue
from pathlib import Path

# Only the public API
__all__ = [
    'TestFailure', 'TestTimeout', 'fail', 'timeout',

    'sandbox_path', 'simics_base_path', 'project_path',

    'test_root', 'simics_repo_root'
]

# Stubs for internal functions
def cpu_class(): raise NotImplementedError
def simple_config(cpuclass): raise NotImplementedError
def testfiles_path(): raise NotImplementedError
def dumps_path(): raise NotImplementedError
def package_path(): raise NotImplementedError
def simics_dist_config(): raise NotImplementedError
def simics_package_specs(): raise NotImplementedError
def simics_package_specs_file(): raise NotImplementedError
def host_path(): raise NotImplementedError
def external_package_path(): raise NotImplementedError
def make_path(): raise NotImplementedError

def simics_repo_root():
    if simics_repo is None:
        # Tests that rely on simics_repo_root must currently be run
        # by testrun.py, checked out from the repo (not a copy).
        raise Exception('Not running from a Simics repo')
    return simics_repo

simics_repo_root.vt_internal = True

# Duplicate from simicsutils.internal, for backwards compatibility
def ensure_text(s, encoding='utf-8', errors='strict'):
    assert isinstance(s, (str, bytes))
    if isinstance(s, bytes):
        return s.decode(encoding, errors)
    else:
        return s

#:: doc testparams.test_root {{
# Return the absolute path to the simics-base test directory.
# }}
def test_root(): raise NotImplementedError

def safe_long_filename(path):
    if is_windows() and not path.startswith("\\\\?\\"):
        path = "\\\\?\\" + path.replace("/", "\\")
    return path

def rm_tree(path):
    p = safe_long_filename(path)
    if os.path.exists(p):
        # avoid problems with removing write-protected paths
        for (base, dirs, files) in os.walk(p):
            for d in dirs:
                path = Path(base) / d
                current_mode = path.stat().st_mode
                desired_mode = current_mode | stat.S_IWUSR | stat.S_IXUSR
                if current_mode != desired_mode:
                    path.chmod(desired_mode)
        shutil.rmtree(p)

# faster, shorter, race-free implementation of os.makedirs
def make_dirs(name, mode=0o777):
    try:
        os.mkdir(name, mode)
        return
    except OSError as err:
        if err.errno == errno.EEXIST:
            return
        if err.errno != errno.ENOENT:
            raise
    make_dirs(os.path.dirname(name), mode)
    make_dirs(name, mode)

def target_variant():
    return ''

def prepend_to_path(env, var, val):
    if var in env:
        env[var] = val + ":" + env[var]
    else:
        env[var] = val

def simics_environ():
    e = os.environ.copy()
    e['SANDBOX'] = sandbox_path()
    e['SIMICS_BASE'] = simics_base
    e['SIMICS_ROOT'] = simics_base
    e['CONFFILE'] = simple_config(cpu_class())
    e['CPUCLASS'] = cpu_class()
    e['TEST_ROOT'] = test_root()
    e['SIMICS_PROJECT'] = project if project else ""
    e['SIMICS_HOST'] = host_platform()
    if simics_repo is not None:
        e['SIMICS_REPO_ROOT'] = str(simics_repo)
    e['MINI_PYTHON_PATH'] = str(mini_python_path())
    return e

def enable_core_dump():
    try:
        import resource
        resource.setrlimit(resource.RLIMIT_CORE, (-1, -1))
    except ImportError:
        pass
    except ValueError:
        pass # Raised if not allowed to raise limit

def mini_python_path():
    return Path(project_path()) / 'bin' / f'mini-python{batch_suffix()}'

logdir = None

# Run Simics via reaper. Normally returns reaper exit code, which is
# 0=success, 1=timeout, 2=failure.
# If asynchronous is true, just returns the subprocess object without
# waiting for it to finish. If reaper=False, no reaper is used.
# If coredumps is true, core dumps will be turned on during the
# subtest. Use it sparingly as core files can quickly fill up a
# filesystem.
def run_simics(args, timeout, echo=False, variant=None, outfile=None,
               errfile=subprocess.STDOUT, coredumps=False,
               settings=False, reaper=True, wdeprecated=True,
               batch_mode=True, param_project=None, simics_binary=None,
               default_package_list=True, scratch_project=False,
               py3_warnings=True, asynchronous=False, cwd=None,
               base_package=None, deprecations_as_errors=True, extra_env = []):
    assert reaper or timeout == None
    base = base_package or simics_base
    exe = ".exe" if is_windows() else ""
    reaperbin = os.path.join(base, host_arch, "bin", f"reaper{exe}")

    if param_project == None and not has_external_packages:
        param_project = project
    arg_project = (param_project if param_project and not scratch_project
                   else sandbox_project())

    if simics_binary:
        simics = [os.path.join(base, host_arch, "bin", simics_binary)]
    else:
        if arg_project:
            simics = [os.path.join(arg_project, "bin",
                                   f"simics{batch_suffix()}")]
        else:
            simics = [os.path.join(base, "bin",
                                   f"simics{batch_suffix()}")]

    if is_windows():
        flags = ["--stop-on-error",
                 "-e", '@conf.sim.deprecation_stack_trace=True',
                 "--project", arg_project]
    else:
        flags = ["--stop-on-error", "-e", "sim->deprecation_stack_trace=TRUE",
                 "--project", arg_project]

    if deprecations_as_errors:
        flags.append("--deprecations-as-errors")
    if wdeprecated and "--no-wdeprecated" not in args:
        # --wdeprecated takes precedence, so don't add it if user has
        # added --no-wdeprecated
        flags.append("--wdeprecated")
    if not settings:
        flags.append("--no-settings")
    if coredumps:
        flags.append("--dump-core")
        enable_core_dump()
    if echo:
        flags.append("--echo")
    if variant == None:
        variant = target_variant()
    if variant:
        flags.append(variant)
    if default_package_list and package_list and not "--package-list" in args:
        flags.append("--package-list")
        flags.append(package_list)
    if batch_mode:
        flags.append("--batch-mode")

    args = simics + flags + args
    if reaper:
        if "win64" in host_arch:
            timeout = timeout * 2  # Windows is slow
        if sys.version_info > (3, 10):
            timeout = timeout * 2  # Host Python is slow
        args = [reaperbin, "-t", str(timeout), "-c", "-r"] + args

    e = simics_environ()
    if extra_env:
        e.update(extra_env)
    sys.stdout.flush()

    if outfile == None:
        outfile = logfile
    proc = subprocess.Popen(args, env=e, cwd=cwd,
                            stdout=outfile, stderr=errfile)
    if asynchronous:
        return proc
    else:
        try:
            ret = proc.wait()
        except KeyboardInterrupt:
            ret = 2
        return ret

def trim_suffix(s, suffix):
    if s.endswith(suffix):
        return s[:-len(suffix)]
    return s

def run_multi_subtest(subtest, timeout=120, name=None, extra_args=[]):
    """Run one simics performing multiple subtests.

    For each subtest, the Simics process must write to its standard output a
    line containing only 'BEGIN testname', and later a corresponding
    'SUCCESS testname' or 'FAILURE testname message'.  Remember to ensure that
    the messages are at the beginning of a line.
    """
    start = time.time()
    proc = run_simics((extra_args or []) + [subtest], timeout,
                      asynchronous = True, outfile = subprocess.PIPE)
    action = None
    for l in proc.stdout:
        line = ensure_text(l)
        m = re.match(r"(BEGIN|SUCCESS|FAILURE) (\S+) *(.*)", line)
        if m:
            (action, subsub, msg) = m.groups()
            msg = msg.strip()   # Strip away \r at the end on windows
            if action == "BEGIN":
                begin_subtest(subsub)
            elif action == "SUCCESS":
                report_success(subsub)
                end_subtest(subsub)
            elif action == "FAILURE":
                report_failure(subsub, msg)
                end_subtest(subsub)
            else:
                assert False
        else:
            logfile.write(line)

    if action == 'BEGIN':
        report_failure(subsub, 'subtest never ended')
        end_subtest(subsub)

    if name == None:
        name = trim_suffix(subtest, ".py")
    begin_subtest(name)
    print("run_multi_subtest() total time %.2fs" % (
        time.time() - start), file=logfile)
    report_run(name, proc.wait())
    end_subtest(name)

def not_in_lab():
    global in_lab
    return not in_lab

#:: doc testparams.host_arch {{
# Return the host OS.
# }}
def host_platform():
    return host_arch

#:: doc testparams.simics_base_path {{
# Return the absolute path to the Simics base package.
# }}
def simics_base_path():
    return simics_base

def sandbox_path():
    return logdir
sandbox_path.vt_internal = True

def sandbox_project():
    return os.path.join(sandbox_path(), "scratch")

#:: doc testparams.project_path {{
# Returns the absolute path to the project under test. Tests added with
# `suite.add_simics_test()` and `suite.add_simics_tests()` run Simics in
# this project.
# }}
def project_path():
    if package_list:
        # project path when testing with external packages.
        # FIXME: Set global project variable instead?
        return os.path.dirname(package_list)
    elif project:
        return project
    else:
        # FIXME: run_test should handle the package test rig,
        # but it doesn't, package list is in the installation, I guess.
        # Would be better to let setup() set project and package_list.
        q = os.path.join(simics_base_path(), '..', 'proj')
        if os.path.exists(q):
            return q
    return None

#:: doc testparams.TestFailure {{
# Exception which test functions should raise to indicate test failure.
# }}
class TestFailure(Exception):
    pass

#:: doc testparams.TestTimeout {{
# Exception which test functions should raise to indicate timeout.
# }}
class TestTimeout(Exception):
    pass

class _SuiteTerm(Exception):
    pass

#:: doc testparams.fail {{
# Raise a `TestFailure` exception.
#
# Argument:
# <div class="dl">
#
# - <span class="term">reason</span>
#     a string containing the reason for the failure, passed to the
#    `TestFailure` constructor
# </div>
# }}
def fail(reason=""):
    raise TestFailure(reason)

#:: doc testparams.timeout {{
# Raise a `TestTimeout` exception.
# }}
def timeout():
    raise TestTimeout

def run_subtest(script, timeout=120, name=None, outfile=None, echo=False,
                variant=None, extra_args=[], namesuffix='',
                scratch_project=False, batch_mode=True, extra_env={}):
    """Run a Simics test.
    Arguments:
    script:
    path to the script to run as a subtest
    timeout:
    amount of time the test is allowed to run before timing out
    name:
    name of the test, optional, defaults to the name of the script
    outfile:
    file to write Simics output to, optional, defaults to the log file
    echo:
    pass the -echo flag to Simics if True, optional, defaults to False
    extra_args:
    extra arguments to pass to simics

    Run script, which can be either a Python script, or a simics script,
    in Simics. Simics is run in batch mode and with the --stop-on-error,
    --wdeprecated and --deprecations-as-errors flags. If the script is a
    Python script it will have access to the test modules.

    The return value is zero if the test succeeded, nonzero
    otherwise. A return value of None indicates the test was not run
    since it did not match the test pattern.
    """

    if variant == "-stall":
        raise TestFailure("Error: No more -stall in simics6!")

    if variant == "-fast":
        raise TestFailure("Error: No more -fast in simics6!")

    sname, fun = simics_test_fun(script, timeout, name, outfile,
                                 echo, variant, extra_args or [],
                                 namesuffix=namesuffix,
                                 scratch_project=scratch_project,
                                 batch_mode=batch_mode,
                                 extra_env=extra_env)

    if not matches_test_pattern(sname):
        return None

    begin_subtest(sname)
    res = fun()
    report_run(sname, res)
    end_subtest(sname)
    return res

test_patterns = None
def add_test_pattern(p):
    global test_patterns
    if test_patterns == None:
        test_patterns = set()
    test_patterns.add(p)

def add_filter_file(fnam = 'filter'):
    try:
        with open(fnam) as ff:
            for l in ff.readlines():
                ls = l.strip()
                if not ls.startswith("#"):
                    add_test_pattern('^' + ls + '$')
    except IOError:
        return False
    return True

def matches_test_pattern(p):
    global test_patterns
    if test_patterns == None:
        test_patterns = set(os.environ.get('TEST_PATTERN', '').split(';'))
    try:
        tps_compiled = [re.compile(tp) for tp in test_patterns]
    except re.error as exc:  # report gracefully invalid patterns, e.g. "*"
        raise Exception(
            f"Incorrect test pattern '{exc.pattern}': {exc}"
        ) from None

    return any(tp.match(p) for tp in tps_compiled)

def test_pattern_test_filter(t):
    assert isinstance(t, Test)
    return matches_test_pattern(t.get_name())

def run_subtests(pattern="*.simics", timeout=120, echo=False, variant=None,
                 extra_args=[], chdir=None, namesuffix='', batch_mode=True):
    """Run a Simics test for each script which matches the given pattern.

    pattern:
    pattern used to match scripts, see Python's glob for
    the syntax
    timeout:
    amount of time the test is allowed to run before timing out
    echo:
    pass the -echo flag to Simics if True, optional, defaults to False
    variant, chdir:
    internal, do not use
    extra_args:
    extra arguments to pass to simics
    """
    with WithChdir(chdir):
        for st in sorted(glob.glob(pattern)):
            run_subtest(st, timeout, echo=echo, variant=variant,
                        extra_args=extra_args, namesuffix=namesuffix,
                        batch_mode=batch_mode)

def run_testfun(testfun, args = [], name = None, **kwargs):
    """Run a function as a test.
    testfun:
    function to run as a test
    args:
    arguments to pass to testfun
    name:
    name of the test, optional, defaults to the name of the function
    ignore_test_pattern:
    set to true if testfun should always be run

    Run the function as a test. The function should
    signal failure and timeout with the TestFailure and
    TestTimeout classes. If the function simply returns
    the test succeeds.

    Returns True if test was successful; False on TestFailure, TestTimeout or
    Exception; None if no tests were run due to mismatched TEST_PATTERN.
    """
    if not name:
        name = testfun.__name__

    ignore_test_pattern = kwargs.get('ignore_test_pattern', False)
    if not ignore_test_pattern and not matches_test_pattern(name):
        return None

    begin_subtest(name)

    ok = False

    try:
        testfun(*args)
    except TestFailure as e:
        pr(traceback.format_exc())
        report_failure(name, "failure: %s" % (e,))
    except TestTimeout:
        pr(traceback.format_exc())
        report_timeout(name)
    except Exception:
        pr(traceback.format_exc())
        report_failure(name, "unhandled exception")
    else:
        report_success(name)
        ok = True

    end_subtest(name)

    return ok

# Global variables used by the functions in this module and set in _main().
# We provide some defaults here when we can.
simics_base = None
host_arch = None
project = None
simics_repo = None
test_resource_dir = None
package_list = None
has_external_packages = False
in_lab = False
def setup(s_simics_base, s_host_arch, s_project, s_test_resource_dir,
          s_package_list, s_in_lab=False, simics_repo_root=None):
    global simics_base, host_arch, project, test_resource_dir, package_list,\
           in_lab, simics_repo
    simics_base = s_simics_base
    host_arch = s_host_arch
    project = s_project
    simics_repo = simics_repo_root
    test_resource_dir = s_test_resource_dir
    package_list = s_package_list
    in_lab = s_in_lab
    import lookup_file
    lookup_file.select_package_list(package_list)

_valid_test_name_pattern = re.compile(r'[-a-zA-Z_/\\0-9.~()+=:;]+$')
def validate_test_name(name):
    if not _valid_test_name_pattern.match(name):
        fail(f"Invalid test name: '{name}'")

class Reporter:
    def __init__(self, result_file, logfile, verbose):
        self.subtest_start_time = {}
        self.ntests = 0
        self.failures = 0
        self.result_file = result_file
        self.logfile = logfile
        self.verbose = verbose
        self.lock = threading.Lock()

    def print_result(self, *strs):
        self.result_file.write(' '.join(strs) + '\n')
        self.result_file.flush()

    def pr(self, str):
        self.logfile.write(ensure_text(str) + "\n")
        self.logfile.flush()

    def report_result(self, subtest, result):
        if result.startswith('*'):
            self.pr(result)  # include failure message in log for reference
        if self.verbose:
            self.print_result('RESULT', subtest, result)
        else:
            self.print_result(subtest, result)
        self.ntests += 1

    def report_success(self, subtest):
        self.report_result(subtest, "--- passed ---")

    def report_failure(self, subtest, reason=""):
        reason = str(reason)
        if not reason.isprintable():
            # newlines in reason would confuse the log parser
            reason = repr(reason)
        self.report_result(subtest, "*** failed (%s) ***" % reason)
        self.failures += 1

    def report_timeout(self, subtest):
        self.report_result(subtest, "*** timeout ***")
        self.failures += 1

    def report_run(self, subtest, ret):
        if ret == 0:
            self.report_success(subtest)
        elif ret == 1:
            self.report_timeout(subtest)
        else:
            self.report_failure(subtest, "exit-status %d" % ret)

    def _begin_subtest(self, subtest):
        validate_test_name(subtest)
        self.pr("=BEGIN %s" % subtest)
        if self.verbose:
            self.print_result('BEGIN', subtest)

    def _log_subtest_time(self, subtest):
        self.subtest_start_time[subtest] = time.time()

    def begin_subtest(self, subtest):
        self._begin_subtest(subtest)
        self._log_subtest_time(subtest)

    def end_subtest(self, subtest):
        now = time.time()
        if subtest in self.subtest_start_time:
            start = self.subtest_start_time[subtest]
            elapsed = " %.1f s" % (now - start)
        else:
            self.pr("ERROR: subtest %s had no begin marker" % subtest)
            elapsed = ""
        self.pr("=END %s%s" % (subtest, elapsed))
        if self.verbose:
            self.print_result('END', subtest)

    def enter_subtest(self, name):
        self.begin_subtest(name)
        self.saved_stdout = sys.stdout
        sys.stdout = self.logfile

    def leave_subtest(self, name):
        sys.stdout = self.saved_stdout
        self.end_subtest(name)

    def results_from_file(self, filename):
        try:
            with open(filename) as f:
                for line in f:
                    (name, _, status) = line.partition(" ")
                    status = status.replace("*", "").strip()
                    self.begin_subtest(name)
                    if not "--- passed ---" in status:
                        self.report_failure(name, status)
                    self.end_subtest(name)
        except OSError as e:
            self.report_failure(filename, e.strerror)

    def get_ntests(self):
        return self.ntests

    def get_failure_count(self):
        return self.failures

# Simple reporter which uses a single logfile (which can be stdout).
class SilentReporter(Reporter):
    def __init__(self, logfile):
        super(SilentReporter, self).__init__(None, logfile, False)
    def print_result(self, *strs):
        pass
    def get_logger(self, test, prefix):
        return SimpleMonitor(test.get_report_name(prefix), self)

class SuiteReporter(Reporter):
    """The reporter used by the suite, it requires a real file as log output"""
    def __init__(self, result_file, logfile, verbose):
        Reporter.__init__(self, result_file, logfile, verbose)
        assert hasattr(logfile, 'name')
        self.logdir = os.path.dirname(self.logfile.name)

class _Monitor:
    def __init__(self, name, reporter):
        self.name = name
        self.reporter = reporter

    def _handle_exit(self, exc_type, exc_value, tb):
        if exc_type == None:
            self.reporter.report_success(self.name)
        elif exc_type == TestFailure:
            self.reporter.report_failure(self.name, exc_value)
        elif exc_type == _SuiteTerm:
            self.reporter.report_failure(self.name, "Suite timeout")
        elif exc_type == TestTimeout:
            self.reporter.report_timeout(self.name)
        elif issubclass(exc_type, Exception):
            print(traceback.format_exc(), file=self.logfile)
            self.reporter.report_failure(
                self.name,
                "%s, %r" % (exc_type.__name__, str(exc_value)))
        else:
            return False # Don't ignore unhandled exception
        return True

class SimpleMonitor(_Monitor):
    def __enter__(self):
        self.logfile = sys.stdout
        self.reporter.begin_subtest(self.name)
    def __exit__(self, exc_type, exc_value, tb):
        ignore = self._handle_exit(exc_type, exc_value, traceback)
        self.reporter.end_subtest(self.name)
        return ignore

class _SerMonitor(_Monitor):
    def __enter__(self):
        self.reporter.begin_subtest(self.name)
        self.saved_stdout = sys.stdout
        self.saved_stderr = sys.stdout
        sys.stdout = self.logfile
        sys.stderr = self.logfile

    def __exit__(self, exc_type, exc_value, traceback):
        ignore = self._handle_exit(exc_type, exc_value, traceback)
        self.reporter.end_subtest(self.name)
        sys.stdout = self.saved_stdout
        sys.stderr = self.saved_stderr
        return ignore

def _split_log_file_name(logdir, name):
    d = os.path.dirname(name)
    if d:
        make_dirs(os.path.join(logdir, d))
    return os.path.join(logdir, name + ".log")

class SerSplitMonitor(_SerMonitor):
    """Serial test monitor, it redirects the output of test to a
    generated log file"""
    def __init__(self, name, reporter):
        _SerMonitor.__init__(self, name, reporter)
        self.logfile = codecs.open(
            _split_log_file_name(reporter.logdir, name), "w", "utf-8")

    def __exit__(self, exc_type, exc_value, traceback):
        ignore = _SerMonitor.__exit__(self, exc_type, exc_value, traceback)
        self.logfile.close()
        return ignore

class SerMergedMonitor(_SerMonitor):
    """Serial test monitor, it redirects the output of test to a
    specified log file"""
    def __init__(self, name, reporter):
        _SerMonitor.__init__(self, name, reporter)
        self.logfile = self.reporter.logfile

class _ParMonitor(_Monitor):
    def __enter__(self):
        self.reporter._log_subtest_time(self.name)

class ParSplitMonitor(_ParMonitor):
    """Parallel test monitor, it redirects the output of test to a
    generated log file"""
    def __init__(self, name, reporter):
        _ParMonitor.__init__(self, name, reporter)
        self.logfile = codecs.open(
            _split_log_file_name(reporter.logdir, name), "w", "utf-8")

    def __exit__(self, exc_type, exc_value, tb):
        with self.reporter.lock:
            self.reporter._begin_subtest(self.name)
            ignore = self._handle_exit(exc_type, exc_value, tb)
            self.logfile.close()
            self.reporter.end_subtest(self.name)
        return ignore

class ParMergedMonitor(_ParMonitor):
    """Parallel test monitor, it redirects the output of test to a
    specified log file"""
    def __init__(self, name, reporter):
        _ParMonitor.__init__(self, name, reporter)
        self.logfile = tempfile.SpooledTemporaryFile(max_size = 4096)

    def __exit__(self, exc_type, exc_value, tb):
        with self.reporter.lock:
            self.reporter._begin_subtest(self.name)
            self.logfile.seek(0)
            for l in self.logfile:
                self.reporter.logfile.write(ensure_text(l))
            self.logfile.close()
            ignore = self._handle_exit(exc_type, exc_value, tb)
            self.reporter.end_subtest(self.name)
        return ignore

class SplitLogReporter(SuiteReporter):
    """The reporter will redirect output of each test to a separated file"""
    def get_logger(self, test, prefix):
        cls = (ParSplitMonitor if isinstance(test, ParSimicsTest)
               else SerSplitMonitor)
        return cls(test.get_report_name(prefix), self)

class MergedLogReporter(SuiteReporter):
    """The reporter will redirect output of each test to a merged file"""
    def get_logger(self, test, prefix):
        cls = (ParMergedMonitor if isinstance(test, ParSimicsTest)
               else SerMergedMonitor)
        return cls(test.get_report_name(prefix), self)

# provide a default logfile to stdout
logfile = sys.stdout

def test_log_file():
    return logfile

# provide a default reporter that only reports to stdout
global_reporter = Reporter(sys.stdout, sys.stdout, False)

def begin_subtest(subtest):
    global_reporter.begin_subtest(subtest)

def end_subtest(subtest):
    global_reporter.end_subtest(subtest)

def report_success(subtest):
    global_reporter.report_success(subtest)

def report_failure(subtest, reason=""):
    global_reporter.report_failure(subtest, reason)

def report_timeout(subtest):
    global_reporter.report_timeout(subtest)

def report_run(subtest, res):
    global_reporter.report_run(subtest, res)

def pr(str):
    global_reporter.pr(str)

def results_from_file(filename):
    global_reporter.results_from_file(filename)

class WithChdir:
    def __init__(self, dir, logfile = None):
        self.dir = dir
        self.logfile = logfile or test_log_file()
    def __enter__(self):
        if self.dir is not None:
            self.old_wd = os.getcwd()
            os.chdir(self.dir)
            cwd = os.getcwd().replace('\\', '/')
            print(f"Entering directory '{cwd}'", file=self.logfile)
    def __exit__(self, exc_type, exc_value, traceback):
        if self.dir is not None:
            cwd = os.getcwd().replace('\\', '/')
            print(f"Leaving directory '{cwd}'", file=self.logfile)
            os.chdir(self.old_wd)

class Test:
    def __init__(self, name, fn, line, simics, extra_args=[]):
        validate_test_name(name)
        self._name = name
        self._file = fn
        self._line = line
        self._simics = simics
        self._extra_args = extra_args

    def get_name(self):
        return self._name

    def get_detail(self):
        return "%s|%s|%s|%s|%s" % (self._name, 'S' if self._simics else 'P',
                                   ' '.join(self._extra_args),
                                   self._file, self._line)

    def get_report_name(self, prefix):
        return (prefix + '~' + self._name) if prefix else self._name

class FunTest(Test):
    def __init__(self, name, fun, args=()):
        fc = fun.__code__
        self._fun = fun
        self._args = args
        Test.__init__(self, name, fc.co_filename, fc.co_firstlineno, False)

    def run(self, logger):
        with logger:
            self._fun(*self._args)

class _SimicsTest(Test):
    def __init__(self, script, name, args, timeout, extra_args):
        Test.__init__(self, name, script, 1, True, extra_args)
        self._args = args
        self._timeout = timeout
        self.proc = None

    def stop(self):
        if self.proc:
            self.proc.send_signal(2)

    def run_simics_impl(self, *args, **kwargs):
        return run_simics(*args, **kwargs)

    def run(self, logger):
        with logger:
            self.proc = self.run_simics_impl(self._extra_args + self._args,
                                             self._timeout,
                                             asynchronous = True,
                                             outfile = logger.logfile,
                                             errfile = logger.logfile,
                                             settings = not_in_lab())
            try:
                ret = self.proc.wait()
            except KeyboardInterrupt:
                ret = 2
            check_reaper_res(ret)

class ParSimicsTest(_SimicsTest):
    def __init__(self, script, name, args, timeout, extra_args):
        _SimicsTest.__init__(self, script, name, args, timeout, extra_args)
        self.finished = threading.Event()

    def run(self, logger):
        try:
            _SimicsTest.run(self, logger)
        finally:
            self.finished.set()

class SerSimicsTest(_SimicsTest): pass

def script_to_name(script):
    for suffix in ['.py', '.simics']:
        if script.endswith(suffix):
            return trim_suffix(script, suffix)
    return script

def script_to_args(script):
    return [script]

def _script_to_args_name(script, name, namesuffix):
    args = script_to_args(script)
    if name is None:
        name = script_to_name(script)
    return args, name + namesuffix

def simics_test_fun(script, timeout, name=None, outfile=None, echo=False,
                    variant=None, extra_args=[], namesuffix='',
                    scratch_project=False, batch_mode=True, extra_env={}):
    (args, name) = _script_to_args_name(script, name, namesuffix)
    return name, lambda: run_simics(extra_args + args, timeout,
                                    outfile=outfile, echo=echo,
                                    variant=variant, settings=not_in_lab(),
                                    scratch_project=scratch_project,
                                    batch_mode=batch_mode,
                                    extra_env=extra_env)

def check_reaper_res(res):
    if res == 0:
        return

    if res == 1:
        timeout()
    else:
        fail("exit-status %d" % res)

class TestThreadPool:
    def __init__(self, max_workers):
        self._max_workers = max_workers
        self._queue = queue.Queue()
        self._threads = []
        self.lock = threading.Lock()
        self.done = threading.Event()
        self.finished = 0

    def _worker(self):
        while True:
            with self.lock:
                try:
                    w = self._queue.get(block = False)
                except queue.Empty:
                    self.finished += 1
                    break
            (run, logger) = w
            run(logger)
        if self.finished >= len(self._threads):
            self.done.set()

    def submit(self, *args):
        with self.lock:
            self._queue.put(args)
        if len(self._threads) < self._max_workers:
            t = threading.Thread(target = self._worker, args = ())
            t.setDaemon(True)
            t.start()
            self._threads.append(t)

    def join(self):
        for t in self._threads:
            t.join()

#:: doc defining-tests {{
# # Defining Test Suites
# The test framework supports two ways to discover tests in a suite:
# - automatic detection of tests
# - explicit registration of tests with respective API functions
#
# ## Automatic Detection of Tests
#
# In general, the test framework will search the files inside the suite
# directory and add a Simics test for each file matching the pattern "s-\*.py",
# the test name will be named after the script file, with the extension
# stripped.
#
# ## Explicit Registration of Tests
#
# If the suite directory contains a `tests.py` file, the framework imports
# this file as a Python module and runs the `tests` function defined within
# in order to add tests to the suite. The signature of this function is:
# `def tests(suite)`.
#
# The suite parameter supports the following methods:
# <div class="dl">
#
# - <span class="term">`add_test(name, function, args)`</span>
#     Add a Python test named `name`, which runs the `function` with the
#     optional tuple of arguments in `args`.
# - <span class="term">`add_simics_test(script, timeout=120, name=None,
#     extra_args=[], namesuffix="")`</span>
#     Add a Simics test which runs `script`. You can specify a timeout measured
#     in seconds with `timeout`. The name of the test defaults to the name of
#     the script, with the extension stripped, but can be overridden with
#     `name`. `extra_args` is a list of extra command line arguments to pass to
#     Simics before the script is run. `namesuffix` is added to the end of the
#     test name, this can be used to distinguish runs of the same test script
#     with different arguments. Simics is run in batch mode and with the
#     `--stop-on-error`, `--wdeprecated` and `--deprecations-as-errors`  flags.
#
#     The test should signal test failures by raising an exception, or exiting
#     Simics with a non zero exit code. There are support APIs for testing
#     documented in the *API Reference Manual*.
# - <span class="term">`add_simics_tests(pattern, timeout=120, extra_args=[],
#     namesuffix="")`</span>
#     Add a Simics test for each file in the suite directory which matches
#     `pattern`. This pattern is a glob. `timeout`, `extra_args` and
#     `namesuffix` are passed to `add_simics_test` for each test.
# - <span class="term">`set_suite_setup(setup)`</span>
#     set a function to be run before the tests in the suite to perform setup
#     actions. The function should be callable without any arguments, and its
#     result is ignored.
# - <span class="term">`set_suite_teardown(teardown)`</span>
#     set a function to be run after the tests in the suite to perform cleanup
#     actions. The function should be callable without any arguments, and its
#     result is ignored.
# </div>
# }}

class Suite:
    def __init__(self, max_jobs=1):
        self.tests = []
        self.setup = lambda: None
        self.teardown = lambda: None
        self.max_jobs = max_jobs
        self.timeout = False
        self._cur_test = None

    def add_test(self, name, function, args=()):
        self.tests.append(FunTest(name, function, args))

    def add_simics_test(self, script, timeout=120,
                        name=None, extra_args=[], parallel=False,
                        namesuffix=""):
        (args, name) = _script_to_args_name(script, name, namesuffix)
        f = os.path.abspath(script)
        if parallel and self.max_jobs > 1:
            self.tests.append(ParSimicsTest(f, name, args, timeout, extra_args))
        else:
            self.tests.append(SerSimicsTest(f, name, args, timeout, extra_args))

    def add_simics_tests(self, pattern, timeout=120, extra_args=[],
                         namesuffix=""):
        for st in sorted(glob.glob(pattern)):
            self.add_simics_test(st, timeout, extra_args=extra_args,
                                 parallel=True, namesuffix=namesuffix)

    def set_suite_setup(self, setup):
        self.setup = setup

    def set_suite_teardown(self, teardown):
        self.teardown = teardown

    def get_tests(self):
        return self.tests

    def _signal_term_handler(self, sig, frame):
        self.timeout = True
        if self._cur_test and isinstance(self._cur_test, _SimicsTest):
            self._cur_test.stop()
        raise _SuiteTerm()

    def run_tests(self, reporter, test_filter=lambda t: True, prefix = None):
        self.setup()
        tests = [t for t in self.tests if test_filter(t)]
        par_tests = [t for t in tests if isinstance(t, ParSimicsTest)]
        ser_tests = [t for t in tests if not isinstance(t, ParSimicsTest)]

        try:
            signal.signal(signal.SIGTERM, self._signal_term_handler)
            # Run serial tests first for users may expect the serial tests
            # will be run in order
            for t in ser_tests:
                if not self.timeout:
                    logger = reporter.get_logger(t, prefix)
                    self._cur_test = t
                    t.run(logger)
            self._cur_test = None
            if len(par_tests) and not self.timeout:
                pool = TestThreadPool(self.max_jobs)
                for t in par_tests:
                    logger = reporter.get_logger(t, prefix)
                    pool.submit(t.run, logger)

                while not pool.done.is_set():
                    try:
                        # wake up every 0.1 seconds so that it won't lose
                        # the external signal
                        pool.done.wait(0.1)
                    except _SuiteTerm:
                        for t in par_tests:
                            if not t.finished.is_set():
                                t.stop()
            signal.signal(signal.SIGTERM, signal.SIG_DFL)
        finally:
            self.teardown()

def find_tests(testdir, max_jobs=1, SuiteType=Suite):
    global logdir
    # It's required to list the checkpoint tests
    if not logdir:
        logdir = ""
    cwd = os.getcwd()
    os.chdir(testdir)
    try:
        if os.path.isfile(os.path.join(testdir, "tests.py")):
            fp = os.path.join(testdir, "tests.py")
            sys.path.append(testdir)
            spec = importlib.util.spec_from_file_location('', fp)
            mod = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(mod)
            suite = SuiteType(max_jobs)
            mod.tests(suite)
            sys.path.remove(testdir)
        elif os.path.isfile(os.path.join(testdir, "run-test.py")):
            suite = None
        else:
            suite = SuiteType(max_jobs)
            suite.add_simics_tests("s-*.py")
    finally:
        os.chdir(cwd)
    return suite

def merge_module_load_log(module_load_log):
    '''Merges module load log files named 'module_load_log.*' into one
    file called 'module_load_log'.'''
    try:
        with open(module_load_log, 'wb') as modfile:
            for f in glob.glob(module_load_log + '.*'):
                with open(f, 'rb') as ff:
                    modfile.write(ff.read())
                    try:
                        os.unlink(f)
                    except OSError:
                        pass
    except OSError:
        pass

def cleanup_module_load_log(module_load_log):
    '''Delete any old module load log files with base name
    'module_load_log'.'''
    for f in glob.glob(module_load_log + '*'):
        try:
            os.unlink(f)
        except OSError:
            pass

def run_suite(testdir, reporter, test_filter,
              exported_env = {}, prefix = None, max_jobs = 1):
    suite = find_tests(testdir, max_jobs)
    if suite and not any(test_filter(t) for t in suite.get_tests()):
        # This reduces chdir spam in the log in the common case when
        # most tests are filtered out
        return

    # If XFAIL exists, add it to the test.log so that:
    # - test-log-parser can easily find it without heuristics, and
    # - users understand that there is an XFAIL involved
    xfail = Path(testdir) / 'XFAIL'
    if xfail.exists():
        print(f"=XFAIL: {xfail}", file=test_log_file())

    with WithChdir(testdir):
        if suite is None:
            sys.path.append(testdir)
            with open("run-test.py", "rb") as f:
                code = compile(f.read(), os.path.realpath(f.name), 'exec')
                exec(code, exported_env)  # nosec
        else:
            suite.run_tests(reporter, test_filter, prefix)

def run_tests(testdir, s_logdir, verbose=False, test_filter=test_pattern_test_filter,
              max_jobs=1, keep_scratch=False):
    """Run tests in a given test directory and return the number of tests run
     and the number of failures."""
    global logdir
    global logfile
    global global_reporter

    logdir = s_logdir
    logs_path = os.path.join(logdir, "logs")
    scratch_path = os.path.join(logdir, "scratch")
    rm_tree(logs_path)
    if not keep_scratch:
        rm_tree(scratch_path)
    make_dirs(logs_path)
    make_dirs(scratch_path)
    logfilename = os.path.join(logdir, "test.log")
    logfile = codecs.open(logfilename, "w", "utf-8")
    reporter = MergedLogReporter(sys.stdout, logfile, verbose)
    global_reporter = reporter

    module_load_log = os.path.join(logdir, 'simics-modules.log')
    cleanup_module_load_log(module_load_log)
    os.environ['SIMICS_MODULE_LOAD_LOG'] = module_load_log

    exported_env = dict((s, globals()[s]) for s in __all__)

    if max_jobs == 0:
        import multiprocessing
        try:
            max_jobs = multiprocessing.cpu_count()
        except NotImplementedError:
            max_jobs = 1

    run_suite(testdir, reporter, test_filter, exported_env, None, max_jobs)
    merge_module_load_log(module_load_log)

    return reporter.get_ntests(), reporter.get_failure_count()

class TestCase:
    '''Use this as a convenient way to set the boundaries of a test
    case using a "with" statement. Will invoke the proper
    begin_subtest and end_subtest functions. If no failure is reported
    report_success will also be called automatically. The preferred way
    to report failure is to use the report_failure (continue
    execution) or fail (abort test case) methods. However, any
    exception thrown will also be caught and reported as a failure.

    Throwing a TestFailure or TestTimeout is equivalent to calling
    fail or timeout respectively.

    Example:

        with TestCase("failure") as tc:
            # Do some testing
            if failed:
                tc.fail("should fail")
            # continue if all was OK
            '''

    def __init__(self, name):
        self.name = name
        self.failed_reason = None

    def has_failed(self):
        return self.failed_reason != None

    def report_failure(self, reason=""):
        '''Fail the current test and continue execution on the next line.'''
        self.failed_reason = reason

    def report_timeout(self):
        '''Fail the current test (as timeout) and continue execution
        on the next line.'''
        self.report_failure("*** timeout ***")

    def report_reaper_result(self, res):
        '''Report the result based on the reaper's return code. This
        is typically the return of a Simics run via a call to the
        "run_simics" function. The reaper's exit codes are:
        0=success, 1=timeout, 2=failure.'''
        check_reaper_res(res)

    def fail(self, reason=""):
        '''Fail the current test and continue execution after the test
        case (i.e. will break out of the with statement).'''
        fail(reason)

    def timeout(self):
        '''Fail the current test (as timeout) and continue execution
        after the test case (i.e. will break out of the with
        statement).'''
        timeout()

    def __enter__(self):
        begin_subtest(self.name)
        return self

    def __exit__(self, type, value, traceback):
        self.__handle_exception(type, value, traceback)
        if self.has_failed():
            report_failure(self.name, self.failed_reason)
        else:
            report_success(self.name)

        end_subtest(self.name)
        return self.__is_test_failure(value)

    def __is_test_failure(self, value):
        return isinstance(value, TestFailure) or isinstance(value, TestTimeout)

    def __handle_exception(self, type, value, traceback):
        if value:
            self.__log_exception(type, value, traceback)
            if not self.has_failed():
                if isinstance(value, TestTimeout):
                    self.report_timeout()
                else:
                    self.report_failure(str(value))

    def __log_exception(self, type, value, tb):
        traceback.print_exception(type, value, tb, file = test_log_file())
