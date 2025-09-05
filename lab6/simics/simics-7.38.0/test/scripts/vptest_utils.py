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


import testparams
import os, os.path, glob, shutil
import lookup_file
import sys

__all__ = ['add_checkpoint_tests',
           'add_random_command_tests',
          ]

timeout = 2000

def rm(path):
    rmfiles = glob.glob(path)
    for file in rmfiles:
        if os.path.isdir(file):
            shutil.rmtree(file)
        else:
            os.remove(file)

def full_script_path(target, script):
    if testparams.project_path() is None:
        return os.path.join(testparams.simics_base_path(),
                            "targets", target, script)
    else:
        return os.path.join(testparams.project_path(),
                            "targets", target, script)

#:: doc vptest_utils {{
# Python API for virtual platform test.
# }}

#:: doc vptest_utils.add_checkpoint_tests {{
# Add tests to check that reading and writing checkpoints does not change
# the behavior of a target system.
#
# The test does this by running Simics several times:
# 1. first load the target, run to time start, and then collect the
#    state-assertion file while running for duration cycles,
# 1. then save a checkpoint at time start and run for duration cycles,
#    comparing against the state-assertion file,
# 1. then load the checkpoint from the previous step, write a new checkpoint
#    and run for duration cycles, comparing against the state-assertion file,
# 1. then load the new checkpoint and run for duration cycles comparing
#    against the state-assertion file,
# 1. finally a test is run which runs up to time start and then writes
#    several checkpoints with duration cycles between them.
#
# Arguments:
# <div class="dl">
#
# - <span class="term">suite</span>
#     the test suite to add the tests to
# - <span class="term">target</span>
#     the name of the target directory in `[project]/targets`
# - <span class="term">script</span>
#     the script in that directory which creates and sets up the target
# - <span class="term">start</span>
#     the time the checks performed by the tests should be started ad, in steps
# - <span class="term">duration</span>
#     the number of steps past the start time the tests should check the state
#     of the system
# - <span class="term">variation</span>
#     a name for this variation of testing the target, defaults to the
#     empty string
# - <span class="term">assert\_objs</span>
#     a list of objects to use in the state assertion, defaults to `["cpu0"]`
# - <span class="term">extra\_args</span>
#     extra arguments passed to each Simics process started by the test,
#     defaults to `[]`
# - <span class="term">assert\_time</span>
#     the interval in steps between state-assertion saves, defaults to `10`
# - <span class="term">check\_cycles\_and\_steps\_in\_sync</span>
#     if True the tests check that the cycle and step counts remain in sync,
#     defaults to `True`
# </div>
# }}
def add_checkpoint_tests(suite, target, script, start, duration, variation="",
                         assert_objs=["cpu0"],
                         extra_args=[], assert_time = 10,
                         check_cycles_and_steps_in_sync=True):
    sb = testparams.sandbox_path()
    def cleanup():
        rm(os.path.join(sb, "scratch", "assert*"))
        rm(os.path.join(sb, "scratch", "conf*"))
    suite.add_test("%s/%s-cleanup(%s)" % (target, script, variation), cleanup)

    if testparams.project_path() is None:
        full_script = os.path.join(testparams.simics_repo_root(),
                                   "targets", target, script)
    else:
        full_script = os.path.join("%simics%", "targets", target, script)
    test_script_dir = str(lookup_file.lookup_file(
        os.path.join("test", "scripts", "checkpoint_tests_scripts")))
    simargs = (["-e", "@simenv.file=" + repr(full_script),
                "-e", "@simenv.start=" + repr(start),
                "-e", "@simenv.duration=" + repr(duration),
                "-e", "@fileprefix=" + repr(os.path.join(sb, "scratch")),
                "-e", "@simenv.objlist=" + repr(assert_objs),
                "-e", "@simenv.assert_time=" + repr(assert_time),
                "-e", ("@simenv.check_cycles_and_steps_in_sync="
                       + repr(int(check_cycles_and_steps_in_sync))),
                "-e", "@simenv.test_script_dir=" + repr(test_script_dir)]
               + extra_args)

    for name in ["initial-setup", "after-config-write", "after-config-read",
                 "after-config-from-config", "multi-checkpoint"]:
        full_name = "%s/%s-%s(%s)" % (target, script, name,
                                      variation)
        test_script = os.path.join(test_script_dir, "%s.simics" % name)
        suite.add_simics_test(test_script, timeout, full_name, simargs)

#:: doc vptest_utils.add_random_command_tests {{
# Add tests to check all the commands provided by the virtual platform can
# be run properly.
#
# Arguments:
# <div class="dl">
#
# - <span class="term">suite</span>
#     the test suite to add the tests to
# - <span class="term">target</span>
#     the name of the target directory in `[project]/targets`
# - <span class="term">script</span>
#     the script in that directory which creates and sets up the target
# - <span class="term">name</span>
#     the test name, defaults to `None`
# - <span class="term">project\_only</span>
#     if `True` the tests will test the commands of local modules,
#     defaults to `False`
# - <span class="term">disabled\_commands</span>
#     the commands which will be ignored by the test, defaults to `[]`
# - <span class="term">disabled\_classes</span>
#     the classes which will be ignored by the test, defaults to `[]`
# - <span class="term">disabled\_interfaces</span>
#     the interfaces which will be ignored by the test, defaults to `[]`
# </div>
# }}
def add_random_command_tests(suite, target, script, name=None,
                             project_only=False,
                             disabled_commands=[],
                             disabled_classes=[],
                             disabled_interfaces=[]):
    if name == None:
        name = '%s-%s-command-test' % (target, script)
    command_file = os.path.join(testparams.sandbox_path(), "commands-" + name)
    log_file = os.path.join(testparams.sandbox_path(), 'run-log-' + name)
    def cleanup():
        for filename in [command_file, log_file]:
            if os.path.isfile(filename):
                os.remove(filename)

    suite.add_test('cleanup-' + name, cleanup)
    machine_script = full_script_path(target, script)
    test_script_dir = lookup_file.lookup_file(
            os.path.join("test", "scripts", "random_command_tests_scripts"))
    suite.add_simics_test(
            os.path.join(test_script_dir, 'gen_commands.py'),
            timeout, name = 'generate-' + name,
            extra_args = ['-e', '@simenv.machine_script=' + repr(machine_script),
                          '-e', '@simenv.command_file=' + repr(command_file),
                          '-e', '@simenv.project_only=' + repr(project_only),
                          '-e', '@simenv.disabled_commands=' + repr(disabled_commands),
                          '-e', '@simenv.disabled_classes=' + repr(disabled_classes),
                          '-e', '@simenv.disabled_interfaces=' + repr(disabled_interfaces),
                          ])
    sys.path.append(test_script_dir)
    import find_command_err
    suite.add_test('run-' + name,
                   lambda: find_command_err.start(
                           machine_script,
                           command_file,
                           log_file,
                           timeout=timeout))
