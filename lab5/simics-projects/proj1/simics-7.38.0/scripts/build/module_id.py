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

import os
import sys
import time
import optparse
import re

from subprocess import Popen, PIPE, STDOUT

DEFAULT_SIMICS_BID_NAMESPACE    = "simics"
DEFAULT_PROJECT_BID_NAMESPACE = "__simics_project__"

def run_simics(simics, args):
    p = Popen([simics] + args, stdin=PIPE, stdout=PIPE, text=True)
    ret = p.wait()
    output = " ".join([x.strip() for x in p.stdout.readlines()])
    if not ret:
        return output
    else:
        raise Exception("Failed to run Simics")

def register_arguments(include_user_build_id = True):
    optpar = optparse.OptionParser(
        """usage: %prog [--c-module-id] <options>
        Create a valid module-id.c for a Simics module
        """)
    optpar.add_option(
        '-c', '--c-module-id', dest = 'c_mod_id', action = 'store_true',
        default = False, help ='Create a module-id file for a C module.')

    # TODO: remove this option when no longer used by GNU Make; CMake is using
    #       the new --build-id option instead
    optpar.add_option(
        '--core-build', dest = 'core_build', action = 'store_true',
        default = False, help = 'Module built in simics project namespace.')

    optpar.add_option(
        '-o', '--output', dest = 'output',
        default = None, help ='(Required) Output file.')

    if include_user_build_id:
        # TODO: remove this option when no longer used by GNU Make; CMake is using
        #       the new --build-id option instead
        optpar.add_option(
            '--user-build-id', dest = 'user_build_id',
            default = "", help ='(Optional) User-provided build-id.')

    optpar.add_option(
        '--build-id', dest = 'build_id',
        default = "", help ='(Optional) Build-id.')

    optpar.add_option(
        '--simics-script', dest = 'simics',
        default = None,
        help ='(Python only, Required). Script that starts Simics.')
    optpar.add_option(
        '--source-file', dest = 'source_file',
        default = None,
        help ='(Python only, Required). Source file of the module.')
    optpar.add_option(
        '--module-name', dest = 'modname',
        default = None, help ='(C only, Required). Name of the module.')
    optpar.add_option(
        '--classes', dest = 'classes',
        default = "",
        help ='(C only, Optional). Classes (but not components) implemented '
        'by the module, as ;-separated string')
    optpar.add_option(
        '--components', dest = 'components',
        default = "",
        help ='(C only, Optional). Components implemented by the module. '
        'as ;-separated string')
    optpar.add_option(
        '--user-version', dest = 'user_version',
        default = "", help ='(C only). User defined version string')
    optpar.add_option(
        '--host-type', dest = 'host_type',
        default = "", help ='(C only). Host type module is compiled on.')
    optpar.add_option(
        '--thread-safe', dest = 'thread_safe',
        default = "no",
        help = 'If the module is thread safe')
    optpar.add_option(
        '--iface-py-module', dest = 'iface_py_modules',
        default = [], action = 'append',
        help ='(C only). Name of a Python module provided by this Simics'
        + ' module, which provides interface wrappings.')
    optpar.add_option(
        '--py-iface-list', dest = 'py_iface_lists',
        default = [], action = 'append',
        help ='(C only). File containing whitespace-separated list'
        + ' of interfaces for which the module provides Python wrappers.')
    optpar.add_option(
        '--dml-dev', dest='dml_devs', action='append', default=[],
        help='''(C only). DML filename stem for one included device. Generate a
        call to the DMLC-generated class initialization function
        (_initialize_XYZ).''')
    optpar.add_option(
        '--user-init-local', dest='user_init_local', action='store_true',
        help='''(C only). Call a user-supplied function init_local when
        loading the module''')
    optpar.add_option(
        '--py-ver', dest = 'py_version', default = None,
        help = 'Python major version if module only works for a particular one')
    optpar.add_option(
        '--py-minor-ver', dest = 'py_minor_version', default = None,
        help = 'Python minor version if module only works for a particular one')
    optpar.add_option(
        '--init-c-wrappers', action = 'store_true',
        dest = 'init_c_wrappers', default = False,
        help = 'Add call to initialization of c interface wrappers')

    return optpar

def parse_arguments(optpar):
    (options, args) = optpar.parse_args()

    if not options.output:
        raise Exception("No output file")

    if options.c_mod_id:
        if (not options.modname):
            raise Exception("Missing argument: module name")
    elif options.py_mod_id:
        if (not options.simics or
            not options.source_file):
            raise Exception("Missing arguments: simics script or source file")

    # check user_build_id
    if optpar.has_option('--user-build-id') and options.user_build_id:
        if not re.match(r'^[^:;]+:\d+$', options.user_build_id):
            raise Exception("Incorrect build-id: expected namespace:build-id")
        nspace, bid = options.user_build_id.split(':')
        if nspace == DEFAULT_SIMICS_BID_NAMESPACE:
            raise Exception(DEFAULT_SIMICS_BID_NAMESPACE + " build-id namespace "
                            + "is reserved for official Simics packages")
        if nspace == DEFAULT_PROJECT_BID_NAMESPACE:
            raise Exception(DEFAULT_PROJECT_BID_NAMESPACE + " build-id "
                            + "namespace is reserved as default build-id "
                            + "namespace in project")
        options.user_build_id = (nspace, bid)
    elif options.build_id:
        nspace, bid = options.build_id.split(':')
        options.user_build_id = (nspace, bid)

    return (options, args)

class ModuleId:
    def __init__(self, options, simics_build_id):
        self.options = options
        self.simics_build_id = simics_build_id
    def builddate(self):
        return os.getenv("_USER_BUILD_DATE", str(int(time.time())))
    def create_module_id(self):
        pass

class CModuleId(ModuleId):
    def abi(self):
        return ['\t"ABI:" %s ";"\n' % 'SYMBOL_TO_STRING(SIM_VERSION)']
    def api(self):
        return ['\t"API:" BUILD_API ";"\n']
    def build_id(self):
        if self.simics_build_id:
            build_id = 'SYMBOL_TO_STRING(SIM_VERSION)'
        elif self.options.user_build_id:
            build_id = '"%s"' % self.options.user_build_id[1]
        else:
            build_id = '"0"'
        return ['\t"BLD:" %s ";"\n' % build_id]
    def build_id_namespace(self):
        if self.simics_build_id:
            bid_ns = 'simics'
        elif self.options.user_build_id:
            bid_ns = self.options.user_build_id[0]
        else:
            bid_ns = DEFAULT_PROJECT_BID_NAMESPACE
        return ['\t"BLD_NS:%s;"\n' % bid_ns]
    def builddate(self):
        return ["\t\"BUILDDATE:\" \"%s\" \";\"\n" % ModuleId.builddate(self)]
    def version(self):
        return ["\t\"VER:\" SYMBOL_TO_STRING(SIM_VERSION_COMPAT) \";\"\n"]
    def modname(self):
        return ["\t\"MOD:\" \"%s\" \";\"\n" % (self.options.modname)]
    def cls_or_cmps(self, l, idname):
        if l:
            # Stripping \' is because of CMake invocations on Windows
            cls = l.strip('\'').split(";")
            classes = ";".join(["%s:" % idname + x.strip('\' ') for x in [x for x in cls if x.strip('\' ')]])
            return ["\t\"%s\" \";\"\n" % classes]
        else:
            return []
    def classes(self):
        return self.cls_or_cmps(
            self.options.classes + self.options.components, 'CLS')
    def components(self):
        return self.cls_or_cmps(self.options.components, 'COMP')

    def py_interfaces(self):
        if self.options.py_iface_lists:
            py_interfaces = []
            for filename in self.options.py_iface_lists:
                with open(filename) as f:
                    py_interfaces.extend(f.read().split())
            py_interfaces = "".join("PYIFACE:%s;" % (x,) for x in py_interfaces)
            return ['\t"%s"\n' % py_interfaces]
        else:
            return []
    def user_version(self):
        if self.options.user_version:
            return ["\t\"USRVER:\" \"%s\" \";\"\n" % self.options.user_version]
        else:
            return []
    def host_type(self):
        return ["\t\"HOSTTYPE:\" \"%s\" \";\"\n" % (self.options.host_type)]
    def thread_safe(self):
        if self.options.thread_safe == "yes":
            return ["\t\"THREADSAFE;\"\n"]
        else:
            return []
    def py_version(self):
        if self.options.py_version:
            return ["\t\"PY_VERSION:%s;\"\n" % self.options.py_version]
        else:
            return []
    def py_minor_version(self):
        # compatibility with dmlc tests
        if hasattr(self.options, 'py_minor_version'):
            ver = self.options.py_minor_version
        else:
            ver = '9'
        if ver:
            return ["\t\"PY_MINOR_VERSION:%s;\"\n" % ver]
        else:
            return []

    def module_date(self):
        return time.asctime()
    def module_id_structure(self):
        return (self.version() +
                self.abi() +
                self.api() +
                self.build_id() +
                self.build_id_namespace() +
                self.builddate() +
                self.modname() +
                self.classes() +
                self.components() +
                self.py_interfaces() +
                self.user_version() +
                self.host_type() +
                self.py_version() +
                self.py_minor_version() +
                self.thread_safe())

    ident_chars = set(
        chr(i)
        for (start, end) in [('A', 'Z'), ('a', 'z'), ('0', '9'), ('_', '_')]
        for i in range(ord(start), ord(end) + 1))

    def dml_init_functions(self):
        return ['_initialize_'
                + ''.join((ch if ch in self.ident_chars else '_')
                          for ch in unmangled)
                for unmangled in self.options.dml_devs]

    def create_module_id(self):
        o = open(self.options.output, "w")

        o.write("""\
/* module_id.c - automatically generated, do not edit */

#include <simics/build-id.h>
#include <simics/base/types.h>
#include <simics/util/help-macros.h>

#if defined(SIMICS_7_API)
#define BUILD_API "7"
#elif defined(SIMICS_6_API)
#define BUILD_API "6"
#else
#define BUILD_API "?"
#endif

#define EXTRA \"                                           \"

EXPORTED const char _module_capabilities_[] =
""")

        for l in self.module_id_structure():
            o.write(l)

        o.write("""\tEXTRA \";\";
EXPORTED const char _module_date[] = "%s";
""" % self.module_date())
        for fun in self.dml_init_functions():
            o.write("extern void %s(void);\n" % (fun,))
        if not self.options.user_init_local:
            # If the user does not declare that an init_local is
            # supplied, then generate one automatically and call
            # it. This yields an early error if the user tries to
            # supply an init_local, but forgets to declare that in the
            # makefile.
            o.write("extern void init_local(void) {}\n")
        for stem in self.options.iface_py_modules:
            o.write("extern void init_python_%s_module(void);\n" % (stem,))
        o.write("""EXPORTED void _simics_module_init(void);
extern void sim_iface_wrap_init(void);

extern void init_local(void);

void
_simics_module_init(void)
{

""")
        if self.options.init_c_wrappers:
            o.write("\tsim_iface_wrap_init();\n")
        for stem in self.options.iface_py_modules:
            o.write("\tinit_python_%s_module();\n" % (stem,))
        for fun in self.dml_init_functions():
            o.write("\t%s();\n" % (fun,))
        o.write("\tinit_local();\n")
        o.write("}\n")
        o.close()

if __name__ == "__main__":
    optpar = register_arguments()
    (options, args) = parse_arguments(optpar)
    mod = CModuleId(options, simics_build_id = options.core_build)
    mod.create_module_id()
