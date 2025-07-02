# © 2017 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


"""Builds all of the required SystemC libraries in the correct order.

Before running this script, the user must make sure to:
1. Export the following variables:
     CC and CXX
   either from config-user.mk or via make parameters or as shell environment
   variables. The config-user.mk is found in the root of the Simics project.
   For better support of Valgrind additionally export the variable
   SYSTEMC_VALGRIND_CFLAGS. E.g SYSTEMC_VALGRIND_CFLAGS=-I/usr/include/valgrind
2. Stand in the root directory of a Simics project
3. If running on Windows, run from an MSVC shell. Currently only version 2015
   and 2022 are supported. The script will auto-detect the version from shell
   environment variables, but if that fails the MSVC_VERSION variable is set to
   'msvc140' (2015).

See SystemC Library Programming Guide for details.

NOTE: currently only Visual Studio compiler is supported on Windows.

NOTE: This script may take several minutes to complete. Errors during
the process can be found in logs/build-systemc-libraries.log.

"""


import os
from os.path import join as pjoin
from io import open
import sys
import subprocess
import argparse
import shutil
from simicsutils.host import batch_suffix, host_type, is_windows

# Avoid os.environ transforming variables to all upper-case on Windows
if is_windows():
    from nt import environ
else:
    from os import environ

# Need to locate progress Python module within SystemC Library package
if "SIMICS_BASE_PACKAGE" in os.environ:
    simics_base = os.environ['SIMICS_BASE_PACKAGE']
    if simics_base.endswith('core'):
        simics_base = simics_base[:-5]
else:
    simics_base = os.path.dirname(os.path.dirname(sys.argv[0]))
sys.path.append(pjoin(simics_base, "scripts"))
from lookup_file import lookup_file, select_package_list
if os.path.exists('.package-list'):
    select_package_list(os.path.realpath('.package-list'))
else:
    select_package_list(None)
progress_path = lookup_file('src/extensions/systemc-library/progress-1.5')
if os.path.exists(progress_path):
    sys.path.append(str(progress_path))
else:
    sys.exit("Failure to locate SystemC Library source code, aborting...")

# Progress bar and threading for hooking the make output
from progress.bar import Bar
import contextlib
import threading

# Need lookup_file to find modules in all installed Simics packages
sys.path.append(pjoin(simics_base, "core", "scripts"))
from lookup_file import lookup_file, select_package_list
if os.path.exists('.package-list'):
    select_package_list(os.path.realpath('.package-list'))
else:
    select_package_list(None)

def log_file():
    '''Return the path to the log file.'''
    logdir = pjoin(os.getcwd(), 'logs')
    if not os.path.exists(logdir):
        os.mkdir(logdir)
    assert os.path.exists(logdir)
    return pjoin(logdir, 'build-systemc-libraries.log')

def write_logs(msgs):
    '''Write msgs into the log file'''
    with open(log_file(), 'at', encoding='utf-8') as f:
        for msg in msgs:
            f.write(msg)
            f.write('\n')

class ProgressLogger(threading.Thread):
    # the build_times can be collected from the build log by filtering
    # out the "script debug: number of lines: N (M)" lines
    build_times = {
        # module: (gcc V=0, V=1, msvc V=0, V=1)
        'systemc-kernel':           (299, 788, 220, 234),
        'systemc-serialization':    ( 33,  75, 150, 199),
        'systemc-checkpoint':       ( 17,  43,  80, 104),
        'systemc-cci':              ( 13,  42, 109, 160),
        'systemc-library':          (182, 196, 180, 197),
        'systemc-isctlm':           (120, 478, 493, 537),
        'systemc-isctlm-awareness': ( 52,  60,  31,  63),
    }
    def __init__(self, module, args, fd):
        threading.Thread.__init__(self)
        self.verbose = 'V=1' in args
        # module exists in build_times, asserted in systemc_modules()
        i = 0 if 'linux64' in host_type() else 2
        i += 1 if self.verbose else 0
        max = self.build_times[module][i]
        self.module = module
        self.bar = Bar(module.ljust(25), max=max, suffix='%(percent)d%%')
        self.log = fd
        (self.read_fd, self.write_fd) = os.pipe()
        self.pipe = open(self.read_fd, 'rb', buffering=0)
        self.start()

    def close(self):
        os.close(self.write_fd)

    def fileno(self):
        return self.write_fd

    def run(self):
        for line in self.pipe:
            self.bar.next()
            self.log.write(line.decode('utf-8'))
        self.log.write("script debug: number of lines: %d (%s)\n" %(
                self.bar.index, self.module))
        self.bar.next(self.bar.remaining)
        self.bar.finish()
        self.pipe.close()

@contextlib.contextmanager
def progress_logger(module, args):
    with open(log_file(), 'at', encoding='utf-8') as f:
        logger = ProgressLogger(module, args, f)
        try:
            yield logger
        finally:
            logger.close()
            logger.join()

def run_command(cmd_args_list, log, env={}):
    try:
        proc = subprocess.run(cmd_args_list, stdout=log, stderr=log, env=env)
    except OSError as e:
        with open(log_file(), 'at', encoding='utf-8') as f:
            f.write("*** failure to execute \"%s\": %s\n"
                    % (' '.join(cmd_args_list), e))
        return (-1, '')
    return proc.returncode

def check_run_command(command, args, extra_env = {}, progress=True):
    '''Check the result of a command. Return False if error.'''
    cmd_args_list = [command] + args
    write_logs(["Executing: %s" % cmd_args_list])

    env = environ.copy()
    env.update(extra_env)
    if progress:
        with progress_logger(args[0], args) as log:
            ret = run_command(cmd_args_list, log, env)
    else:
        with open(log_file(), 'at', encoding='utf-8') as log:
            ret = run_command(cmd_args_list, log, env)

    # '***' in stderr also means ret != 0, so this is sufficient
    return ret == 0

def module_exists_in_pkgs(fname):
    '''Find file in all installed Simics packages. Used as a way
    to check if SystemC packages have been installed.'''
    return lookup_file(pjoin("src", "extensions", fname)) != None

def systemc_modules(pwd, intc_ext, checkpointing, cci, isctlm):
    '''Return a list of all SystemC modules with current Simics pkgs'''
    module_list = ["systemc-kernel",
                   "systemc-library"]
    if intc_ext:
        if checkpointing:
            module_list.insert(1, "systemc-serialization")
            module_list.insert(2, "systemc-checkpoint")
        if cci:
            module_list.insert(1, "systemc-cci")

        # For now, we assume ISCTLM requires INTC_EXT=1
        if isctlm:
            if module_exists_in_pkgs("systemc-isctlm"):
                module_list += ["systemc-isctlm",
                                "systemc-isctlm-awareness"]
    # Make sure the build_times in ProgressLogger is up-to-date if new entries
    # are added here. Alternatively the data could be stored in a single place,
    # but that would require a less user friendly format
    assert set(ProgressLogger.build_times.keys()) >= set(module_list)
    return module_list

def copy_module(module_name, pwd, force=False):
    '''Copy module to current project. force=True will replace existing module.'''
    # Check if module already exists
    module_path = pjoin(pwd, "modules", module_name)
    if os.path.exists(module_path):
        if force:
            shutil.rmtree(module_path)
        else:
            write_logs(["Note: module %s not copied " % module_name,
                        "since it is already there. If you want a hard copy,",
                        "use --force option."])
            # also make it immediately visible to the user
            print(("%s already exists, use --force option to"
                   " build from package" % module_name))
            return True  # skip --copy-module, return Success

    project_setup = pjoin(pwd, "bin",
                                 "project-setup" + batch_suffix())
    return check_run_command(project_setup,
                             ["--copy-module=%s" % module_name,
                              "%s" % pwd], progress=False)

def compile_module(module_name, make, options, force=False):
    '''Compile module with options provided by user'''
    if host_type() == 'win64' and make == 'make':
        # default make on win64 is mingw make.bat
        make_cmd = pjoin("bin", "make.bat")
    else:
        make_cmd = make
    if force:
        check_run_command(make_cmd, ["clean-%s" % module_name], progress = False)
    return check_run_command(make_cmd, [module_name] + options)


#:: doc build-systemc-libraries-switches-list-usage {{
# <section class="not-numbered">
#
# ### Usage
#
# <div class="dl">
#
# - <span class="term">Windows</span>
#
#     ```simics
#     prompt: project> bin\build-systemc-libraries.bat [options]
#     ```
#
# - <span class="term">Linux</span>
#
#     ```simics
#     prompt: project$ bin/build-systemc-libraries [options]
#     ```
#
# </div>
# </section>
# }}

#:: doc build-systemc-libraries-switches-list-options {{
# <section class="not-numbered">
#
# ### Options
#
# <div class="dl">
#
#
# - <span class="term">`-h, --help`</span>
#     Show help message and exit.
# - <span class="term">`-f, --force`</span>
#     Force rebuild of all the libraries.
# - <span class="term">`-u, --unittests`</span>
#     Build and run unit tests.
# - <span class="term">`-j, --jobs`</span>
#     Run up to N build jobs in parallel.
# - <span class="term">`-m, --make`</span>
#     Specify which GNU Make binary to run. Defaults to 'make'
# - <span class="term">`-v, --version`</span>
#     Specify which version of SystemC kernel to use. Defaults to '2.3.3'
# - <span class="term">`-std, --cxx-standard`</span>
#     Specify which C++ standard to use. Defaults to 17 for SystemC kernel
#     3.0.0, otherwise defaults to 14.
# - <span class="term">`...`</span>
#     The rest are parsed as make options for the modules.
# </div>
# </section>
# }}

def argument_parser():
    parser = argparse.ArgumentParser(
        prog = "bin/build-systemc-libraries",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        usage = "%(prog)s [script options] [make options]",
        description = __doc__)

    parser.add_argument(
        '-f', '--force', action='store_true', default=False,
        help = 'force rebuild of all the libraries')
    parser.add_argument(
        '-u', '--unittests', action='store_true', default=False,
        help = 'build and run unit tests')
    parser.add_argument(
        '-j', '--jobs', type=int, default=1,
        help="run up to N build jobs in parallel")
    parser.add_argument(
        '-m', '--make', type=str, default='make',
        help="specify which GNU Make binary to run")
    parser.add_argument(
        '-v', '--version', type=str, default='2.3.3',
        choices=['2.3.3', '2.3.4', '3.0.0'],
        help="specify which version of SystemC kernel to use")
    parser.add_argument(
        '-std', '--cxx-standard', type=str,
        choices=['14', '17'],
        help="specify which C++ standard to use")
    parser.add_argument(
        'make_options', nargs=argparse.REMAINDER)

    return parser.parse_args()

class BuildError(Exception):
    def __init__(self, reason):
        self.reason = reason

    def __str__(self):
        return self.reason

# The script supports both INTC_EXT as make option and as environment variable.
# make options have precedence over environment variable, and we assume that
# INTC_EXT=1 is the default value. Function returns True/False depending on the
# state of the INTC_EXT flag, i.e. are the extensions enabled or not.
def intc_ext(make_options):
    if 'INTC_EXT=0' in make_options or '0' == os.environ.get('INTC_EXT'):
        return False

    return True

# SystemC checkpointing is opt-in
def build_checkpointing(make_options):
    if 'USE_SIMICS_CHECKPOINTING=yes' in make_options:
        return True

    if 'yes' == os.environ.get('USE_SIMICS_CHECKPOINTING'):
        return True

    return False

def build_cci(make_options):
    if 'USE_SIMICS_CCI=no' in make_options:
        return False

    if 'no' == os.environ.get('USE_SIMICS_CCI'):
        return False

    return True

def build_isctlm(make_options):
    if not module_exists_in_pkgs("systemc-isctlm"):
        return False

    if 'SYSTEMC_CXX_STANDARD=17' in make_options:
        return False

    if '17' == os.environ.get('SYSTEMC_CXX_STANDARD'):
        return False

    return True

def build_systemc_libraries():
    # Start by truncating the log file to avoid confusion
    open(log_file(), 'w').close()

    pwd = os.getcwd()
    if not os.path.exists(pjoin(pwd, ".project-properties")):
        raise BuildError("This script must be run in the root of a project")

    args = argument_parser()
    # Skip the options when this function is not invoked by
    # build-systemc-libraries application. For example, when it is invoked
    # by test framework.
    if 'build_systemc_libraries' not in os.path.basename(sys.argv[0]):
        make_options = []
    else:
        make_options = args.make_options

    make_options.append("-j%d" % args.jobs)
    if not args.unittests:
        make_options.append("SYSTEMC_NO_UNITTESTS=1")

    # Forces systemc-boost.mk to look at BOOST_LIB_PATH, required by ISCTLM
    if build_isctlm(make_options):
        make_options.append("SYSTEMC_ISCTLM=yes")

    if args.version == '3.0.0' and args.cxx_standard == '14':
        raise BuildError("The SystemC kernel 3.0.0 can't be built with C++14")
    make_options.append(f"SYSTEMC_KERNEL_VERSION={args.version}")
    if not args.cxx_standard is None:
        make_options.append(f"SYSTEMC_CXX_STANDARD={args.cxx_standard}")

    print("Building:")
    print("  with SystemC kernel %s" % args.version)
    for module in systemc_modules(pwd, intc_ext(make_options),
                                  build_checkpointing(make_options),
                                  build_cci(make_options),
                                  build_isctlm(make_options)):
        if not copy_module(module, pwd, args.force):
            raise BuildError("failed when copying module %s" % module)
        # TODO(ah): remove this hack when we have resolved the MODULEDEPS files
        # === begin hack ===
        deps = pjoin(pwd, "modules", module, "MODULEDEPS")
        if os.path.exists(deps):
            os.remove(deps)
        # === end hack ===
        if not compile_module(module, args.make, make_options, args.force):
            raise BuildError("failed when compiling module %s" % module)

if __name__ == "__main__":
    try:
        build_systemc_libraries()
    except BuildError as e:
        write_logs(["Error: %s" % e])
        sys.exit("\n*** Failed building SystemC libraries, %s (details: %s)" % (
            e, log_file()))

    print("")
    print("All SystemC libraries successfully built")
