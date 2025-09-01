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


import pickle
import os
import time
import sys
import argparse
import subprocess
import traceback
import contextlib
import glob
import re
from pathlib import Path
from simicsutils.internal import ispm_path, simics_base
sys.path.append(str(Path(simics_base()) / ".." / ".." / "test" / "scripts"))
import dotest_impl
import simicsutils.host
import testparams
from simicsutils.host import is_windows


class OrderDependency:
    def __init__(self, chain):
        # Dependency chain, in the form of a list with subtest names.
        self.chain = chain


class BasicTest:
    resource_locks = None

    def get_result(self, job_id):
        return []

    def get_dependencies(self):
        return []


class SimicsTest(BasicTest):
    def __init__(self, name, script, timeout, scratch_project, threads, env,
                 est_time, cwd, batch_mode, param_project, extra_args, base_package,
                 outfile=None, errfile=None):
        self.script = os.path.abspath(script)
        self.name = name
        self.timeout = timeout
        self.est_time = est_time
        self.scratch_project = scratch_project
        self.env = env.copy() if env is not None else None
        self.threads = threads
        self.testenv = TestEnviron()
        self.extra_args = extra_args
        self.cwd = cwd
        self.batch_mode = batch_mode
        self.param_project = param_project
        self.base_package = base_package
        self.outfile = outfile
        self.errfile = errfile

    def run(self, job_id):
        testparams.validate_test_name(self.name)
        self.testenv.restore()
        start = time.time()
        print("=BEGIN %s" % self.name)
        sys.stdout.flush()
        with contextlib.ExitStack() as stack:
            if self.outfile:
                print(f"NOTE: stdout redirected to {self.outfile}")
                outfile = stack.enter_context(open(self.outfile, "a"))
            else:
                outfile = sys.stdout
            if self.errfile:
                print(f"NOTE: stderr redirected to {self.errfile}")
                errfile = stack.enter_context(open(self.errfile, "a"))
            else:
                errfile = sys.stderr

            ret = testparams.run_simics(
                self.extra_args + [self.script],
                timeout=self.timeout,
                outfile=outfile,
                errfile=errfile,
                extra_env=self.env,
                cwd=self.cwd,
                batch_mode=self.batch_mode,
                param_project=self.param_project,
                scratch_project=self.scratch_project,
                base_package=self.base_package,
            )

        if ret == 1:
            print("*** timeout ***")
        elif ret != 0:
            print("*** failed (reaper exit status %d) ***" % ret)
        print("=END %s %.1f s" % (self.name, time.time() - start))
        sys.stdout.flush()


class SimicsMultiTest(BasicTest):
    def __init__(self, name, script, timeout, threads, env, est_time,
                 extra_args):
        self.script = os.path.abspath(script)
        self.name = name
        self.timeout = timeout
        self.est_time = est_time
        self.env = env.copy() if env is not None else None
        self.threads = threads
        self.extra_args = extra_args
        self.testenv = TestEnviron()

    def run(self, job_id):
        self.testenv.restore()
        assert testparams.logfile is sys.stdout
        testparams.run_multi_subtest(
            self.script,
            timeout=self.timeout,
            name=self.name,
            extra_args=self.extra_args
        )
        sys.stdout.flush()


class PickleTest(BasicTest):
    use_reaper = True

    def run(self, job_id):
        testparams.begin_subtest(self.name)

        self.job_id = job_id
        pickle_file = os.path.join(self.testenv.logdir,
                                   "test-%d.pickl" % job_id)
        with open(pickle_file, "wb") as f:
            pickle.dump(self, f)

        sys.stdout.flush()
        ret = self._run_pickle_file(pickle_file)
        if ret == 0:
            pass
        elif ret == 1:
            testparams.report_timeout(self.name)
        else:
            testparams.report_failure(self.name, "reaper exit status %d" % ret)
        os.unlink(pickle_file)

        testparams.end_subtest(self.name)

    # Run a test from a pickle file with reaper
    def _run_pickle_file(self, pickle_file):
        host_arch = simicsutils.host.host_type()
        simics_base = testparams.simics_base
        reaperbin = os.path.join(
            simics_base, host_arch, "bin", "reaper")

        mini_python = (os.getenv("SIMICS_PYTHON") or
                       os.path.join(
                           self.testenv.project, "bin",
                           f"mini-python{simicsutils.host.batch_suffix()}"))
        testparams_impl = os.path.join(simics_base, "test", "scripts",
                                       "testparams_impl.py")
        timeout = self.timeout
        if host_arch == "win64":
            timeout = timeout * 2  # Windows is slow
        reaper = [reaperbin, "-t",
                  str(timeout)] if self.use_reaper else []
        args = reaper + [mini_python, testparams_impl, pickle_file]
        proc = subprocess.Popen(args, stdout=sys.stdout, stderr=sys.stdout)
        return proc.wait()

    def outer_pickle_run(self):
        subtest_file = self._subtest_file(self.job_id)
        try:
            os.unlink(subtest_file)
        except OSError:
            pass
        self.pickle_run()
        _subtests.save(subtest_file)

    def _subtest_file(self, job_id):
        return os.path.join(self.testenv.logdir, "result-%d.pickl" % job_id)

    def get_result(self, job_id):
        return _subtests.load(self._subtest_file(job_id))


class SuiteParserTest(PickleTest):
    def __init__(self, name, suitedir, logdir):
        self.suitedir = os.path.abspath(suitedir)
        self.name = name
        self.threads = 1
        self.timeout = 60
        self.est_time = 99999
        self.testenv = TestEnviron("generic", logdir)

    def pickle_run(self):
        d = {"encoding": "utf-8"}
        sys.path.append(".")
        with open("suites.py", **d) as f:
            exported_env = dict((s, getattr(testparams, s))
                                for s in testparams.__all__)
            exec(f.read(), exported_env)  # nosec


class SuiteDir(BasicTest):
    def __init__(self, name, suitedir, env, ext_pkgs_file=None):
        self.name = name
        self.suitedir = os.path.abspath(suitedir)
        self.env = env.copy() if env is not None else None
        self.ext_pkgs_file = ext_pkgs_file


class TestSuite(PickleTest):
    def __init__(self, name, suitedir, target, logdir, timeout=600, env={},
                 use_reaper=True, ext_pkgs_file=None):
        self.timeout = timeout
        self.suitedir = os.path.abspath(suitedir)
        self.name = "suite"
        self.env = env.copy() if env is not None else None
        self.external_packages = None
        self.use_reaper = use_reaper
        self.testenv = TestEnviron(target, logdir)
        if ext_pkgs_file:
            self.ext_pkgs_file = ext_pkgs_file
        else:
            self.ext_pkgs_file = os.path.join(self.suitedir,
                                              "external-packages")

    def pickle_run(self):
        logdir = self.testenv.logdir
        logs_path = os.path.join(logdir, "logs")
        scratch_path = os.path.join(logdir, "scratch")

        testparams.rm_tree(logs_path)
        testparams.rm_tree(scratch_path)
        testparams.make_dirs(logs_path)
        testparams.make_dirs(scratch_path)

        module_load_log = os.path.join(logdir, 'simics-modules.log')
        os.environ['SIMICS_MODULE_LOAD_LOG'] = module_load_log
        if self.env:
            os.environ.update(self.env)
        testparams.cleanup_module_load_log(module_load_log)

        exported_env = dict((s, getattr(testparams, s))
                            for s in testparams.__all__)

        if self.external_packages is not None:
            self._setup_external_packages()

        testparams.run_suite(
            self.suitedir, testparams.global_reporter,
            testparams.test_pattern_test_filter,
            exported_env)

        testparams.merge_module_load_log(module_load_log)

    def _setup_external_packages(self):
        def fail(testname, err):
            testparams.begin_subtest(testname)
            testparams.report_failure(testname, err)
            testparams.end_subtest(testname)

        def find_package_dir(pkg_num, version):
            if not is_windows():
                ispm = ispm_path(testparams.simics_repo_root())
                install_dir = self.ext_packages_dir
                p = subprocess.run([ispm, "packages", "--config-file",
                                    os.path.join(testparams.simics_repo_root(),
                                                 "logs", "ispm.conf"),
                                    "--install-dir", install_dir,
                                    "--list-installed"],
                                   text=True, check=True, capture_output=True)
                pkg_path = None
                for l in p.stdout.splitlines():
                    m = re.match(r"\s(?P<num>\d+)\s+(?P<name>\S+)\s+(?P<version>\S+)\s+(?P<path>\S+)", l)
                    if (m and int(m.group('num')) == pkg_num
                          and m.group('version') == version):
                        pkg_path = m.group('path')
                        break
                return pkg_path
            else:
                for p in glob.glob(os.path.join(self.ext_packages_dir,
                                                "**", "packageinfo", "*"),
                                   recursive=True):
                    try:
                        pkg_info = Path(p).read_text()
                        if (f"package-number: {pkg_num}" in pkg_info
                                and f"version: {version}" in pkg_info):
                            return os.path.dirname(os.path.dirname(p))
                    except UnicodeDecodeError:
                        # Ignore binary files in packageinfo directory
                        pass
                return None

        # Find packages
        pkg_paths = []
        for (pkg_num, version, _pkg) in self.external_packages:
            if not version or not _pkg:
                fail("missing-package-%d" % pkg_num,
                     "package %d not installed" % pkg_num)
            else:
                pkg = find_package_dir(pkg_num, version)
                if not pkg:
                    # Verify either system has valid Kerberos token or not
                    krb_status = "Unknown error"
                    pr = subprocess.run(['klist', '-s'])
                    if pr.returncode == 1:
                        krb_status = ("No valid Kerberos token found."
                                      " Run kinit to issue")
                    fail(f"missing-package-dir-{pkg_num}-{version}",
                         f"{krb_status}")
                else:
                    pkg_paths.append(pkg)

        # Create scratch project
        import external_package_setup
        external_package_setup.setup_project(pkg_paths)
        testparams.package_list = external_package_setup.package_list_path()
        testparams.has_external_packages = True

    # Return external-package dependencies in the form
    # [('name', test_case_constructor), ...]
    def get_dependencies(self):
        if not os.path.isfile(self.ext_pkgs_file):
            return []

        install_dir = os.path.join(
            testparams.project, "logs", "external-packages")
        testparams.make_dirs(install_dir)

        target = self.testenv.target.replace("-turbo", "")

        import external_package_setup
        pkgs = external_package_setup.parse_external_packages(
            self.ext_pkgs_file, target)
        self.external_packages = pkgs
        self.ext_packages_dir = install_dir

        return [_make_pkg_install(pkg_num, version, pkg, install_dir)
                for (pkg_num, version, pkg) in pkgs]


class PythonTest(PickleTest):
    def __init__(self, name, script, threads, timeout, est_time):
        self.name = name
        self.timeout = timeout
        self.script = script
        self.threads = threads
        self.est_time = est_time
        self.testenv = TestEnviron()

    def pickle_run(self):
        d = {"encoding": "utf-8"}
        with open(self.script, **d) as f:
            exported_env = dict((s, getattr(testparams, s))
                                for s in testparams.__all__)
            exec(f.read(), exported_env)  # nosec


class PythonFunctionTest(PickleTest):
    def __init__(self, name, func, args, threads, timeout, est_time, cwd):
        self.name = name
        self.timeout = timeout
        self.func = (func.__name__, func.__module__)
        self.args = args
        self.threads = threads
        self.est_time = est_time
        self.testenv = TestEnviron()
        self.cwd = cwd

    def pickle_run(self):
        (fname, module) = self.func
        import importlib
        module = importlib.import_module(module)
        with testparams.WithChdir(self.cwd):
            getattr(module, fname)(*self.args)


class PkgInstall(PickleTest):
    def __init__(self, name, pkg_num, version, pkg, install_dir):
        self.name = name
        self.pkg = pkg
        self.pkg_num = pkg_num
        self.version = version
        self.install_dir = install_dir
        self.threads = 1
        self.est_time = 0
        self.timeout = 600
        self.testenv = TestEnviron()
        self.resource_locks = ["pkg-install"]

    def pickle_run(self):
        if not self.pkg:
            testparams.report_failure(
                self.name, "external package %d not found" % self.pkg_num)
            return
        import external_package_setup
        external_package_setup.install_external_package(
            self.pkg_num, self.version, self.pkg,
            self.install_dir, testparams.simics_base)


_target = None


def setup_target(target, logdir):
    host_arch = simicsutils.host.host_type()
    host_dir = os.path.join(testparams.project, host_arch)
    testparams.logdir = logdir
    global _target
    _target = target
    dotest_impl.patch_testparams(testparams, target, host_dir,
                                 testparams.test_resource_dir, False)
    reporter = testparams.SilentReporter(sys.stdout)
    testparams.global_reporter = reporter


def setup(simics_base, project, test_root, simics_repo_root=None):
    project = os.path.abspath(project)
    simics_base = os.path.abspath(simics_base)
    test_root = os.path.abspath(test_root)
    host_arch = simicsutils.host.host_type()
    testparams.setup(simics_base, host_arch, project, test_root,
                     None, s_in_lab=True, simics_repo_root=simics_repo_root)


class TestEnviron:
    def __init__(self, target=None, logdir=None):
        self.simics_base = testparams.simics_base
        self.host_arch = testparams.host_arch
        self.project = testparams.project
        self.simics_repo = testparams.simics_repo
        self.test_resource_dir = testparams.test_resource_dir
        self.package_list = testparams.package_list
        self.in_lab = testparams.in_lab
        self.logdir = logdir if logdir else testparams.logdir
        self.target = target if target else _target
        self.has_external_packages = testparams.has_external_packages

    def restore(self):
        testparams.simics_base = self.simics_base
        testparams.host_arch = self.host_arch
        testparams.project = self.project
        testparams.simics_repo = self.simics_repo
        testparams.test_resource_dir = self.test_resource_dir
        testparams.package_list = self.package_list
        testparams.in_lab = self.in_lab
        testparams.logdir = self.logdir
        testparams.has_external_packages = self.has_external_packages
        setup_target(self.target, self.logdir)


class Subtests:
    def __init__(self):
        self.tests = []

    # Add automatic dependencies for tests ending with "-<digit>"
    def _add_auto_deps(self):
        n = {}
        for t in self.tests:
            if not hasattr(t, "name"):
                continue
            m = re.match(r'^([\w-]*-)([0-9]*)$', t.name)
            if m:
                n.setdefault(m.group(1), []).append(m.group(0))
        for x in n.values():
            if len(x) > 1:
                add_dependency(*sorted(x))

    # Save information about all subtests in a pickled file
    def save(self, filename):
        self._add_auto_deps()
        if self.tests:
            blob = pickle.dumps(self.tests)
            Path(filename).write_bytes(blob)

    @staticmethod
    def load(filename):
        try:
            with open(filename, "rb") as f:
                tests = pickle.load(f)  # nosec
            os.unlink(filename)
            return tests
        except IOError:
            return []


_subtests = Subtests()


def add_subtest(script, timeout=120, name=None, extra_args=[],
                namesuffix='', scratch_project=False, batch_mode=True,
                threads=1, env=None, est_time=0, cwd=None,
                param_project=None, base_package=None,
                outfile=None, errfile=None, deps=[]):
    tname = (name if name else testparams.script_to_name(script)) + namesuffix
    if testparams.matches_test_pattern(tname):
        t = SimicsTest(tname, script, timeout, scratch_project, threads, env,
                       est_time, cwd, batch_mode, param_project, extra_args, base_package,
                       outfile, errfile)
        _subtests.tests.append(t)
        for dep in deps:
            add_dependency(dep, tname)


def add_subtests(pattern, **kwds):
    scripts = sorted(glob.glob(pattern))
    if not scripts:
        testparams.fail(f"no tests were matched by {repr(pattern)} pattern")
    for script in scripts:
        add_subtest(script, **kwds)


def add_multi_subtest(script, timeout=120, name=None, extra_args=[],
                      threads=1, env=None, est_time=0):
    tname = name if name else testparams.script_to_name(script)
    t = SimicsMultiTest(tname, script, timeout, threads, env, est_time,
                        # without -q, the welcome banner appears between tests
                        ['-q'] + extra_args)
    _subtests.tests.append(t)


def add_suite(name, testdir, env=None, ext_pkgs_file=None):
    s = SuiteDir(name, testdir, env, ext_pkgs_file)
    _subtests.tests.append(s)


def add_pytest(script, name=None, namesuffix='',
               threads=1, timeout=120, est_time=60, deps=[]):
    tname = (name if name else testparams.script_to_name(script)) + namesuffix
    t = PythonTest(tname, script=script,
                   threads=threads, timeout=timeout, est_time=est_time)
    _subtests.tests.append(t)
    for dep in deps:
        add_dependency(dep, tname)


def add_testfun(name, func, args=[],
                threads=1, timeout=120, est_time=60, cwd=None, deps=[]):
    if testparams.matches_test_pattern(name):
        t = PythonFunctionTest(name, func=func, args=args, threads=threads,
                               timeout=timeout, est_time=est_time, cwd=cwd)
        _subtests.tests.append(t)
        for dep in deps:
            add_dependency(dep, name)

def add_dependency(*args):
    t = OrderDependency(args)
    _subtests.tests.append(t)


def _make_pkg_install(pkg_num, version, pkg, install_dir):
    if version:
        name = "install-%d-%s" % (pkg_num, version)
    else:
        name = "install-%d" % pkg_num

    def create_pkg_install(suite_dir, logdir):
        setup_target("generic", logdir)
        return PkgInstall(name, pkg_num, version, pkg, install_dir)
    return (name, create_pkg_install)


# Run tests from a pickle file
def main():
    p = argparse.ArgumentParser()
    p.add_argument('pickle_file', help="testcase pickle file")
    args = p.parse_args()
    with open(args.pickle_file, "rb") as f:
        t = pickle.load(f)  # nosec

    tenv = t.testenv
    sys.path.append(".")
    sys.path.append(os.path.join(tenv.simics_base, "scripts"))
    sys.path.append(os.path.join(tenv.simics_base, "scripts", "dist"))
    sys.path.append(os.path.join(tenv.simics_repo, "test", "scripts"))

    setup(tenv.simics_base, tenv.project, tenv.test_resource_dir)
    t.testenv.restore()
    try:
        t.outer_pickle_run()
    except:
        traceback.print_exc(file=sys.stdout)
        testparams.report_failure(t.name, "exception")


def _export_api():
    api = ["add_subtest", "add_subtests", "add_multi_subtest",
           "add_testfun", "add_pytest", "add_suite",
           "add_dependency"]
    for x in api:
        setattr(testparams, x, globals().get(x))
        testparams.__all__.append(x)


if __name__ == '__main__':
    import testparams_impl
    testparams_impl.main()
else:
    _export_api()
