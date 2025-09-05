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


import re, sys

def print_syntax():
    print('Syntax:', file=sys.stderr)
    print(' ', sys.argv[0], '<host type>', file=sys.stderr)
    print('Analyzes output from objdump (stdin) and generates', file=sys.stderr)
    print('trampoline data (stdout)', file=sys.stderr)

class Function:
    def __init__(self, name):
        self.name = name
        self.start = self.end = None
        self.placeholders = {}
        self.offsets = {}

    def set_end(self, end):
        self.end = end

    def set_start(self, start):
        assert self.start is None
        self.start = self.end = start

    def analyze_opcode(self, opcode):
        opcode = opcode.lower()
        for o in self.placeholders:
            self.test_opcode(o, opcode)

    def print_data(self):
        print('static const struct trampoline_data %s_data = {' % self.name)
        print('\t.templ   = (void (*)(void))&%s,' % (self.name))
        print('\t.size    = %d,' % (self.end - self.start))
        print('\t.offsets = {')
        width = -max(len(s) for s in self.offsets)
        for o in sorted(self.offsets):
            print('\t\t.%*s = %d,' % (width, o, self.offsets[o]))
        print('\t}')
        print('};')
        print()

    def verify(self):
        raise NotImplementedError

    def test_opcode(self, placeholder, opcode):
        raise NotImplementedError

class x86_Function(Function):
    class BadOpcode:
        pass
    bad_relative_call = BadOpcode()

    def __init__(self, name, is_64bit = False):
        Function.__init__(self, name)

        # these constants must be kept in sync with
        # src/core/common/trampolines.h
        if is_64bit:
            self.placeholders = {
                'function_hi'          : 'ef be ad de',
                'function_lo'          : 'be ba 01 c0',
                'data_hi'              : 'c5 c5 c5 c5',
                'data_lo'              : '5c 5c 5c 5c',
                self.bad_relative_call : 'e8 00 00 00 00',
                }
        else:
            self.placeholders = {
                'function' : 'ef be ad de',
                'data'     : 'c5 c5 c5 c5',
                }
        self.offsets = {}

    def test_opcode(self, placeholder, opcode):
        o = opcode.find(self.placeholders[placeholder])
        if o < 0:
            return

        if placeholder == self.bad_relative_call:
            raise RuntimeError('%s trampoline contains RIP-relative call,'
                               ' perhaps due to a too-large struct'
                               ' argument' % (self.name,))
        assert not isinstance(placeholder, self.BadOpcode)

        if placeholder in self.offsets:
            raise RuntimeError(
                'found multiple %s (%s) in function %s' % (
                    placeholder, self.placeholders[placeholder],
                    self.name))
        # 3 characters for hex byte and space
        o //= 3
        self.offsets[placeholder] = o + self.end - self.start

    def verify(self):
        for p in self.placeholders:
            if isinstance(p, self.BadOpcode):
                continue
            if p not in self.offsets:
                raise RuntimeError('could not find %s (%s) in function %s' % (
                        p, self.placeholders[p], self.name))

class amd64_Function(x86_Function):
    def __init__(self, name):
        x86_Function.__init__(self, name, is_64bit = True)

host_functions = {
    'linux64'     : amd64_Function,
    'win64'       : amd64_Function,
}

def analyze_objdump(objdump, host_function):
    '''Reads output from 'objdump -dw' from the
    'objdump' file and uses the 'host_function' class (a Function
    subclass) to analyze the output.
    Prints 'struct trampoline_data' constants on stdout.'''

    # for objdump -dw
    label_re = re.compile(r'^0*([0-9A-Fa-f]+)'
                          r' <_?(py_[A-Za-z_][0-9A-Za-z_]*_trampoline.*)>:')

    code_re = re.compile(r'^[ \t]*(?:0x)?0*([0-9A-Fa-f]+):[ \t]*(([0-9A-Fa-f]{2} )+)')

    section_re = re.compile(r'^section \.text%(py_[A-Za-z_][0-9A-Za-z_]*_trampoline.*)?')
    skip_section = False
    section_name = None

    all_functions = []
    cfunc = None
    for l in objdump.readlines():
        m = section_re.match(l)
        if m:
            if not m.group(1):
                skip_section = True
                section_name = None
                continue
            skip_section = False
            section_name = m.group(1)
            continue

        if skip_section:
            continue

        m = label_re.match(l)
        if m:
            fname = section_name or m.group(2)
            if cfunc:
                cfunc.verify()
                all_functions.append(cfunc)
            cfunc = host_function(fname)
            start = m.group(1)
            if start:
                cfunc.set_start(int(start, 16))
            continue
        if not cfunc:
            continue
        m = code_re.match(l)
        if m:
            bytes  = m.group(2)
            nbytes = len(bytes) // 3
            if m.group(1):
                ofs = int(m.group(1), 16)
            else:
                ofs = 0
            if cfunc.start == None:
                cfunc.set_start(ofs)
            cfunc.analyze_opcode(bytes)
            cfunc.set_end(ofs + nbytes)

    if cfunc:
        cfunc.verify()
        all_functions.append(cfunc)

    print('/* created by %s */' % sys.argv[0])
    print()

    for func in all_functions:
        func.print_data()

def main():
    try:
        host_type = sys.argv[1]
    except IndexError:
        print_syntax()
        sys.exit(1)

    try:
        host_function = host_functions[host_type]
    except KeyError:
        print("Unknown host type '%s'" % host_type, file=sys.stderr)
        sys.exit(1)

    analyze_objdump(sys.stdin, host_function)

if __name__ == '__main__':
    main()
