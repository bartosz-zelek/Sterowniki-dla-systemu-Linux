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


class SuiteinfoException(Exception):
    pass


def named(s):
    def fun(f):
        f.__name__ = s
        return f
    return fun

@named("space-separated list of strings")
def spaced_list(s):
    return s.split()

#:: doc suiteinfo {{
# ## SUITEINFO
#
# Apart from defining its parent directory as a test suite, the contents of a
# `SUITEINFO` file can also define some properties of the test suite. Each
# line which does not start with a `#` character defines a parameter. The
# parameter definition is provided as the name of the parameter, a
# ':'-character, and finally the value for the parameter. Whitespace at the
# start and the end of the line is ignored, as well as whitespace characters
# around the ':'.
#
# Currently the following parameters are accepted:
#
# <div class="dl">
#
# - <span class="term">timeout</span>
#     The amount of time the suite is allowed to run before it is terminated
#     by the test system, measured in seconds. The value must be an integer.
#     The default value is 60 seconds.
# - <span class="term">disabled\_on\_hosts</span>
#     Host types (`linux64` or `win64`) on which the suite should not be run.
# - <span class="term">tags</span>
#     Space-separated list of tags for the suite. Each tag is an arbitrary
#     text; when a tag is passed to `test-runner` through the `--tag` argument,
#     then only suites containing that tag will be selected.
# </div>
#
# Using any other parameter name is currently considered a syntax error.
#
# Example SUITEINFO file:
# ```
# # give up after 5 minutes
# timeout: 300
# # do not run on Windows host
# disabled_on_hosts: win64
# # suite belongs to two groups
# tags: pci nightly
# ```
# }}

# Types, default values and descriptions for the SUITEINFO keys
known_suiteinfo_keys = {
    "timeout": (int, 60, "number of seconds the test suite is allowed to run"),
    "disabled_on_hosts": (spaced_list, [],
                          "host types the suite should not be run on"),
    "tags": (spaced_list, [], "tags for the suite"),
    "py3_suite": (bool, False, ("Deprecated, no effect.")),
    "py3": (bool, False, ("Deprecated, no effect.")),
    "py2_suite": (bool, False, ("Deprecated, no effect")),
    "py2": (bool, False, ("Deprecated, no effect")),
    "threads": (int, None, "number of threads needed to run the suite"),
    "estimated_time": (int, None,
                       "estimated time for this suite, in seconds"),
    "priority": (int, None,
                 "suite priority; high priority suites are started first"),
}

def parse_suiteinfo(filename, lines):
    def parse_error(msg):
        raise SuiteinfoException("%s:%s" % (filename, msg))

    info = {}
    for line in lines:
        line = line.strip()
        if len(line) == 0:
            continue
        if line.startswith('#'):
            continue
        parts = [e.strip() for e in line.split(":", 1)]
        if len(parts) != 2 or len(parts[0]) == 0 or len(parts[1]) == 0:
            parse_error("Line '%s' not a <key>:<value> line." % line)
        (key, value) = parts
        if key in info:
            parse_error("Duplicate values for key %s" % key)
        if key not in known_suiteinfo_keys:
            parse_error("Unknown SUITEINFO key %s" % key)
        (key_type, default, desc) = known_suiteinfo_keys[key]
        try:
            info[key] = key_type(value)
        except ValueError:
            parse_error("Malformed value '%s' for key '%s', %s expected"
                        % (value, key, key_type.__name__))
    for key, (key_type, default, desc) in list(known_suiteinfo_keys.items()):
        info.setdefault(key, default)
    return info
def parse_xfail(filename, lines):
    suite_info = {}
    for line in lines:
        line = line.strip()
        if len(line) == 0:
            continue
        if line.startswith('#'):
            continue
        parts = [e.strip() for e in line.split()]
        test_info = {'host': [], 'target': [], 'dist': []}
        curr_attr_list = [] # Initialize with a dummy list
        for part in parts[1:]:
            if part in test_info:
                curr_attr_list = test_info[part]
            else:
                curr_attr_list.append(part)
        test_name = parts[0]
        if test_name in suite_info:
            for k in test_info:
                suite_info[test_name][k].extend(test_info[k])
        else:
            suite_info[test_name] = test_info
    return suite_info
