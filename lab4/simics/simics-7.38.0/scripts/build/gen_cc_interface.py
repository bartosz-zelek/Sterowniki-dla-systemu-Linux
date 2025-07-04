#!/usr/bin/env python3

# © 2021 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


"""
This script takes in a Simics interface header file, transforms it to
a C++ header file which could be used in modules using c++-api
"""

import copy
import os
import os.path
import re
import argparse
import datetime
from pathlib import Path

def snake_to_camel_case(snake_str):
    '''Converts a string from snake_case to CamelCase'''
    return ''.join([word.capitalize() for word in snake_str.split('_')])

def default_output_file(path, dirname="c++"):
    '''Return default output file by appending dirname to the input directory path'''
    p = Path(path)
    return p.parent / dirname / p.name

def default_include_path(path):
    '''Return default include path can be used in the #include statement'''
    normalized_path = path.replace('\\', '/')
    file_name = os.path.basename(normalized_path)
    return '../' + file_name

def simics_copyright():
    '''Return Simics copyright info'''
    return \
f"""// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © {datetime.datetime.now().year} Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/
"""

def is_generated():
    '''Stating that the C++ header is generated'''
    return "\n// This file is generated by the script bin/gen-cc-interface\n"

def header_guard(path, num_directories=4):
    '''Return head and foot for the header guard which includes at
    most num_directories'''
    normalized_path = path.replace('\\', '/')
    path_obj = Path(normalized_path).resolve()
    parts = path_obj.parts[1:]

    # Use at most num_directories to form the guard
    header = '_'.join(parts[-num_directories:])
    header = (header.upper()
              .replace("+","P")
              .replace("-","_")
              .replace(".","_")
              .replace("/","_"))
    return f"\n#ifndef {header}\n#define {header}\n", "\n#endif\n"

def header_inclusion(path):
    '''The lines to include necessary headers for the output C++ interface file'''
    path_obj = Path(path)
    file_path_str = str(path_obj).replace('\\', '/')

    if "src/include/simics" in file_path_str:
        # For simics core C headers, the include path becomes simpler
        parts = path_obj.parts
        simics_index = parts.index("simics")
        file_path_str = '/'.join(parts[simics_index:])
        c_include = f'\n#include <{file_path_str}>\n'
    else:
        c_include = f'\n#include "{file_path_str}"\n'

    other_includes = \
"""
#include <simics/detail/conf-object-util.h>  // get_interface
#include <simics/iface/interface-info.h>
"""
    return c_include + other_includes

def namespace_wrapper():
    '''Return namespace wrapper head and foot'''
    return \
        "\nnamespace simics {\nnamespace iface {\n", \
        "\n}  // namespace iface\n}  // namespace simics\n"

def first_param(param_string):
    if not "," in param_string:
        return param_string
    else:
        return param_string.split(",")[0]

def rest_params(param_string):
    if not "," in param_string:
        return ""
    else:
        rest = param_string.split(",")[1:]
        if len(rest) == 1:
            return rest[0].strip()
        return ", ".join([p.strip() for p in rest])

class NoArgumentNameFoundError(Exception):
    def __init__(self, str):
        self.str = str
    def __str__(self):
        return self.str

def params_name_only(param_string):
    if param_string == "":
        return ""

    # function pointer as param
    func_ptr_param_regex = r"\((\*\w+)\)\(.*?\)"
    for r in re.findall(func_ptr_param_regex, param_string):
        param_string = re.sub(func_ptr_param_regex, r, param_string, 1)

    names = []
    param_regex = r"(\w+)(?:\*|\s)"
    for p in param_string.split(", "):
        rp = p[::-1]
        result = re.findall(param_regex, rp)
        if not result:
            raise NoArgumentNameFoundError(param_string)
        else:
            names.append(result[0][::-1])
    return ", ".join(names)

typedef_replace = {
    "branch_recorder_handler" :
    ("iter_func_t *iter;",
     "addr_prof_iter_t (*iter)(branch_recorder_t *br, generic_address_t start, generic_address_t stop);"),
    "interrupt_cpu" :
    ("interrupt_ack_fn_t ack;",
     "int (*ack)(conf_object_t *obj);")
}

class InvalidMethodFormatError(Exception):
    pass

class Method:
    '''A method inside a Simics interface'''
    def __init__(self, string=""):
        if string == "":
            self.ret = None
            self.name = None
            self.params = None
        else:
            self.ret, self.name, self.params = self.parse_string(string)

    # From a method string like:
    # "void (*some_method)(conf_object_t *obj, const frags_t *frame)"
    # return a tuple consists of all parts including:
    # return_type, name, parameters
    @staticmethod
    def parse_string(string):
        if "DEPRECATED_FUNC" in string:
            raise InvalidMethodFormatError("Deprecated function")

        method_str = re.sub(re.compile(r"\s\s+"), " ", string.strip())

        # PYTHON_METHOD is empty when PYWRAP is not defined
        method_str = method_str.replace("PYTHON_METHOD ", "")

        # It is assumed all C++ wrapped interfaces have the first parameter
        # of type conf_object(_t)
        if not "conf_object" in method_str:
            raise InvalidMethodFormatError("no conf_object parameter")

        method_regex = r"([^(]+)\(\*(\w+)\)\((.+)\)"
        result = re.findall(method_regex, method_str)
        if not result:
            raise InvalidMethodFormatError("Wrong format")

        return tuple(item.strip() for item in result[0])

class Interface:
    '''A Simics interface'''
    def __init__(self, c_string="", conditions=[]):
        if c_string == "":
            self.name = None
            self.methods = []
        else:
            self.name, self.methods = self.name_and_methods(c_string)
        self.conditions = conditions

    def __str__(self):
        string = "\n"
        for condition in self.conditions:
            string += f"{condition}"
        string += \
f"""class {snake_to_camel_case(self.name)}Interface {{
  public:
    using ctype = {self.name}_interface_t;

    // Function override and implemented by user
"""
        for m in self.methods:
            if m is None:
                continue
            params_wo_obj = rest_params(m.params)
            string += \
f"""    virtual {m.ret} {m.name}({params_wo_obj}) = 0;
"""
        string += self.create_fromc_class(self.name, self.methods)
        string += self.create_toc_class(self.name, self.methods)
        string += self.create_info_class(self.name, self.methods)

        string += "};\n"
        for _ in self.conditions:
            string += "#endif\n"
        return string

    @staticmethod
    def create_fromc_class(iface, methods):
        string = \
"""
    // Function convert C interface call to C++ interface call
    class FromC {
      public:
"""
        for m in methods:
            # Skip deprecated func
            if m is None:
                continue
            if_return = "" if m.ret == "void" else "return "
            first_p = params_name_only(first_param(m.params))
            rest_p = params_name_only(rest_params(m.params))
            string += \
f"""        static {m.ret} {m.name}({m.params}) {{
            {if_return}detail::get_interface<{snake_to_camel_case(iface)}Interface>({first_p})->{m.name}({rest_p});
        }}
"""

        string += "    };\n"
        return string

    @staticmethod
    def create_toc_class(iface, methods):
        ciface = snake_to_camel_case(iface)
        string = \
f"""
    // Function convert C++ interface call to C interface call
    class ToC {{
      public:
        ToC() : obj_(nullptr), iface_(nullptr) {{}}
        ToC(conf_object_t *obj, const {ciface}Interface::ctype *iface)
            : obj_(obj), iface_(iface) {{}}

"""

        for m in methods:
            # Skip deprecated func
            if m is None:
                continue
            params_wo_obj = rest_params(m.params)
            if_return = "" if m.ret == "void" else "return "
            rest_ps = rest_params(m.params)
            rest_p = (", " if rest_ps else "") + params_name_only(rest_ps)
            string += \
f"""        {m.ret} {m.name}({params_wo_obj}) const {{
            {if_return}iface_->{m.name}(obj_{rest_p});
        }}
"""

        string += \
f"""
        const {ciface}Interface::ctype *get_iface() const {{
            return iface_;
        }}

      private:
        conf_object_t *obj_;
        const {ciface}Interface::ctype *iface_;
    }};
"""
        return string

    @staticmethod
    def create_info_class(iface, methods):
        ciface = snake_to_camel_case(iface)
        string = \
f"""
    class Info : public InterfaceInfo {{
      public:
        // InterfaceInfo
        std::string name() const override {{ return {iface.upper()}_INTERFACE; }}
        const interface_t *cstruct() const override {{
            static constexpr {ciface}Interface::ctype funcs {{
"""

        for m in methods:
            if m is None:
                string += \
"""                nullptr,
"""
            else:
                string += \
f"""                FromC::{m.name},
"""
        string += \
"""            };
            return &funcs;
        }
"""
        string += "    };\n"
        return string

    @staticmethod
    def name_and_methods(string):
        # Filter comments
        string = re.sub(re.compile(r"/\*.*?\*/", re.DOTALL), "", string)
        string = re.sub(re.compile(r"//.*?\n" ), "", string)

        # Get #name from SIM_INTERFACE(#name)
        iface_regex = r"SIM_INTERFACE\((\w+)\)"
        name = re.findall(iface_regex, string)[0]

        string = re.sub(re.compile(iface_regex), "", string)
        string = re.sub(re.compile(r"\{|\};"), "", string)
        # TODO(xiuliang): can we safely ignore all preprocessors?
        # For example, '#if !define(PYWRAP)' can be ignored as it
        # means the interface method is not Python wrappable
        string = re.sub(re.compile(r"#.+\n"), "", string)
        string = re.sub(re.compile(r"\s*\n\s*"), " ", string)

        # Replace typedef
        if name in typedef_replace:
            string = string.replace(typedef_replace[name][0],
                                    typedef_replace[name][1])

        # Get all methods
        methods_str = string.strip().split(";")[:-1]
        methods = []
        for method_str in methods_str:
            if not '(' in method_str:
                # Filter the non-method pure data like "int num_views;" in
                # branch_recorder_handler
                continue
            try:
                method = Method(method_str)
            except InvalidMethodFormatError:
                method = None
            methods.append(method)
        return name, methods

def interfaces_info(from_file):
    '''Return Simics interface information from a C interface header'''
    def strip_notnull(s):
        # NOTNULL is not relevant in C++ wrappers
        return s.replace('NOTNULL ', '')

    # An incomplete list of interfaces that should not be converted
    skipped_interfaces = set([
        # Interfaces without conf_object_t * are not used between models
        'SIM_INTERFACE(alg)',
        'SIM_INTERFACE(bank_after_read)',
        'SIM_INTERFACE(bank_before_read)',
        'SIM_INTERFACE(bank_after_write)',
        'SIM_INTERFACE(bank_before_write)',

        # internal
        'SIM_INTERFACE(conf_object)',
        'SIM_INTERFACE(internal_cached_instruction)',
        'SIM_INTERFACE(nios)',
        'SIM_INTERFACE(mips)',
        'SIM_INTERFACE(log_object)',

        # SIMICS-22830
        'SIM_INTERFACE(kbd_console)',
    ])

    interfaces = []
    inside_iface = False
    iface_lines = []
    conditions = []
    with open(from_file, 'r', encoding='utf-8') as f:
        for l in f.readlines():
            l = l.lstrip()
            if inside_iface:
                l = strip_notnull(l)
                iface_lines.append(l)
                if l.startswith('};'):
                    inside_iface = False
                    interfaces.append(Interface("".join(iface_lines),
                                                list(conditions)))
            elif l.startswith("SIM_INTERFACE("):
                if not l.split(" ")[0] in skipped_interfaces:
                    iface_lines = [l]
                    inside_iface = True
            else:
                # The preprocessor handling is limited. For example,
                # `#ifndef` is not included here due to the complexity of
                # filtering header guards. If there is a need to cover
                # additional cases, please report it to the Simics team.
                if l.startswith('#if defined') or l.startswith('#ifdef'):
                    conditions.append(l)
                if l.startswith('#endif'):
                    if (conditions):
                        conditions.pop(-1)
                # When the typedef keyword is used to create an alias
                if l.startswith('typedef'):
                    for iface in reversed(interfaces):
                        if iface.name + '_interface_t' in l:
                            last_part = l.split()[-1]
                            alias_name = last_part.rstrip(';').replace('_interface_t', '')
                            alias_interface = copy.deepcopy(iface)
                            alias_interface.name = alias_name
                            interfaces.append(alias_interface)

    return interfaces

def write_cc_header(include_path, interfaces, to_file):
    '''Write c++ interface header to to_file based on the dict information'''
    to_file.write(simics_copyright())
    to_file.write(is_generated())
    guard_head, guard_foot = header_guard(to_file.name)
    to_file.write(guard_head)
    to_file.write(header_inclusion(include_path))
    namespace_head, namespace_foot = namespace_wrapper()
    to_file.write(namespace_head)
    for i in interfaces:
        to_file.write(str(i))
    to_file.write(namespace_foot)
    to_file.write(guard_foot)

def parse_args():
    desc = "Converts a Simics c interface header to a C++ interface header"
    parser = argparse.ArgumentParser(description=desc)

    parser.add_argument("input_file", type=Path,
                        help="The path to the input c interface header")
    parser.add_argument("-o", "--output-file",
                        type=str,
                        default=argparse.SUPPRESS,
                        help="The path to the output C++ interface header. By"
                        " default under 'c++' sub-directory with the same file"
                        " name")
    parser.add_argument("-p", "--include-path",
                        type=str,
                        default=argparse.SUPPRESS,
                        help="The #include path used in the C++ interface"
                        " header to locate the c interface header")

    return parser.parse_args()

def main():
    args = parse_args()

    input_file = args.input_file
    if input_file.suffix != ".h":
        msg = f"The input file({input_file}) is not a .h file"
        raise SystemExit(msg)

    interfaces = interfaces_info(input_file)
    if not interfaces:
        # No Simics interfaces to convert
        return

    if 'output_file' in args:
        output_file = Path(args.output_file)
        if not 'include_path' in args:
            msg = "Option -p is required when option -o is present"
            raise SystemExit(msg)
    else:
        output_file = default_output_file(input_file.name)
    output_file = output_file.absolute()

    directory = output_file.parent
    if not directory.exists:
        os.makedirs(directory)

    if 'include_path' in args:
        include_path = args.include_path
    else:
        include_path = default_include_path(input_file.name)

    with open(output_file, "w", encoding="utf-8") as f:
        write_cc_header(include_path, interfaces, f)

if __name__ == "__main__":
    main()
