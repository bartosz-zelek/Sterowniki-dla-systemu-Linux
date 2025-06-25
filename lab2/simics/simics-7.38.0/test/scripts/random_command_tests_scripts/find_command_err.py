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


# Starts Simics and runs the generated list of commands and checks for errors.
# If an error is detected, the script tries to find the shortest sequence of
# commands needed to reproduce the error.

import os
import re
import subprocess
import time
from testparams import run_simics, pr, sandbox_path
import testparams

test_script_dir = os.path.dirname(os.path.abspath(__file__))

# Timing variables for debug purposes to see where we spend our time
time_in_run = 0
time_in_recursion = 0

def find_last_command(output):
    error_lines = []
    for l in reversed(output):
        m = re.match("RUNNING line ([0-9]*) ", l.strip())
        if m:
            return int(m.group(1)), reversed(error_lines)
        else:
            error_lines.append(l)
    return (0, [])

def log(logfile, output):
    print(output, file=logfile, end='', flush=True)

def print_output(logfile, output):
    log(logfile, "=== Start of output from initial run\n")
    for x in output:
        log(logfile, x.strip())
        log(logfile, "\n")
    log(logfile, "=== End of output from initial run\n\n")

def run(test_script, machine_script, cmdfile, logfile, start_line, first):
    '''runs simics and returns the error line number or 0 on success'''
    p = run_simics(['-p', test_script],
                   timeout=machine_timeout,
                   outfile=subprocess.PIPE, asynchronous=True,
                   extra_env={
                       'T30_CMD_FILE': cmdfile,
                       'T30_MACHINE_SCRIPT': machine_script,
                       'T30_START_LINE': str(start_line)})
    (output, _) = p.communicate()
    output = output.decode('utf-8').splitlines()
    if first:
        print_output(logfile, output)
    if p.returncode != 0:
        (error_line, error_output) = find_last_command(output)
        if error_line == 0:
            raise Exception("Simics failure, but no commands found in output")
        return (error_line, error_output)
    else:
        return (0, [])

def find_crash_line(test_script, machine_script, cmdfile,
                    logfile, start_line, first):
    '''reports faulty line relative count in original file.'''
    log(logfile, ("  Running Simics to find command problem "
                  "(may take some time)\n"))
    crash_line, crash_output = run(test_script, machine_script,
                                   cmdfile, logfile, start_line, first)
    if crash_line == 0:
        log(logfile, "  No error found\n")
        return (0, [])
    else:
        # returned offset only valid when running on original file
        log(logfile, "  Found problem\n")
        return (crash_line, crash_output)

def generate_testfile(cmdfile, required, first, last):
    testfile = os.path.join(sandbox_path(), 'scratch', 'test-commands')
    f = open(testfile, "w")
    cmdlines = open(cmdfile).readlines()
    for line_no in required:
        f.write(cmdlines[line_no - 1])
    for line in cmdlines[first - 1:last]:
        f.write(line)
    f.close()
    return testfile

def find_crashing_start_line(test_script, machine_script,
                             cmdfile, logfile, required,
                             try_low, lowest, highest,
                             crash_line, crash_output):
    log(logfile, ("\n[%d commands found] Starting from %d [range %d - %d]\n"
                  % (len(required), try_low, lowest, highest)))
    testfile = generate_testfile(cmdfile, required, try_low, crash_line)
    (fail, new_crash_output) = find_crash_line(test_script, machine_script,
                                           testfile, logfile, 1, False)
    os.remove(testfile)
    if fail:
        # still crashes, remove more lines
        new_lowest = try_low
        new_low = try_low + (highest - try_low) // 2
        new_highest = highest
        # If we failed, we want to use the updated new_crash_output var
        # instead of the passed in crash_output argument
        crash_output = new_crash_output
    else:
        # runs without crash, include earlier lines
        new_low = lowest + (try_low - lowest) // 2
        new_lowest = lowest
        new_highest = try_low
        # Check if we have failed to reproduce
        if try_low == lowest:
            # We've gone as low as we should go, break from recursion
            return (0, crash_output)
    if lowest + 1 == highest:
        # found the lowest line that crashes Simics
        return (lowest, crash_output)
    return find_crashing_start_line(test_script, machine_script,
                                    cmdfile, logfile, required,
                                    new_low, new_lowest, new_highest,
                                    crash_line, crash_output)

def print_lines_from_file(cmdfile, lines):
    '''print lines in cmdfile selected by line numbers from lines'''
    cmdlines = open(cmdfile).readlines()
    for line_no in lines:
        pr(cmdlines[line_no - 1].strip())

def try_find_one_error(start_line, test_script, machine_script,
                       cmdfile, logfile, timeout):
    global machine_timeout, time_in_recursion, time_in_run
    # double timeout since we may run the commands about twice on error
    machine_timeout = 2 * timeout
    start_time = time.time()
    (crash_line,
     crash_output) = find_crash_line(test_script, machine_script,
                                     cmdfile, logfile, start_line, True)
    # Capture the time spent in running the command file
    time_snap = time.time()
    pr("DEBUG: spent {0:.2f} seconds running cmdfile".format(
        time_snap - start_time))

    # Add this time to the total time spent running cmdfiles
    time_in_run += time_snap - start_time

    if crash_line == 0:
        pr("No command errors found in %s" % cmdfile)
        return 0

    required = [] # line numbers of commands we have to include to crash
    while start_line < crash_line:
        (new_start, crash_output) = find_crashing_start_line(
                test_script, machine_script,
                cmdfile, logfile, required,
                (start_line + crash_line) // 2,
                start_line, crash_line + 1, crash_line, crash_output)
        # If new_start is 0, that means we failed to reproduce the failure
        # with find_crashing_start_line(). We should stop bisecting.
        if new_start == 0:
            pr("\nErrors found, but did not reproduce during the search "
               + "for the minimal cmd list.")
            pr("Showing all commands up to failure point.")
            required.extend(list(range(start_line, crash_line + 1)))
            break
        required.append(new_start)
        start_line = new_start + 1

    # Capture the time spent in recursively searching for the error
    end_time = time.time()
    pr("DEBUG: spent {0:.2f} seconds in cmdfile bisection logic".format(
        end_time - time_snap))

    # Add that time to the global total of time spent in recursion logic
    time_in_recursion += end_time - time_snap

    pr("\nCommands needed to trigger error:\n")
    pr('run-script "%s"' % machine_script.replace("\\", "\\\\"))
    print_lines_from_file(cmdfile, required)
    pr("\nError output:\n")
    pr("\n".join(crash_output))
    pr("")
    return crash_line + 1

def start(machine_script, cmdfile, log_file, timeout):
    global time_in_run, time_in_recursion
    test_script = os.path.join(test_script_dir, "run_commands.py")
    time_in_run = 0
    time_in_recursion = 0
    if not os.path.isfile(cmdfile):
        raise Exception('No such file: ' + cmdfile)
    errors = 0
    all_lines = len(open(cmdfile).readlines())
    start_line = 1
    logfile = open(log_file, 'a', encoding="utf-8")
    try:
        while start_line <= all_lines:
            new_start =  try_find_one_error(start_line, test_script,
                                            machine_script,
                                            cmdfile, logfile, timeout)
            if new_start:
                errors += 1
                start_line = new_start
            else:
                break
    finally:
        pr("DEBUG: Total time spent running cmdfiles: {0:.2f} seconds".format(
            time_in_run))
        pr("DEBUG: Total time spent in bisection logic: {0:.2f} seconds".format(
            time_in_recursion))
        logfile.close()
    if errors:
        testparams.fail('%d error(s) found when executing commands' % errors)
