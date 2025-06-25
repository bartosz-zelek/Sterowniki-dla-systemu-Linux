# GDB extension: main module
# Copyright (C) 2014-2015 Free Software Foundation, Inc.
 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


import gdb
import pickle
import os
import re
import sys
import traceback

from gdb.FrameDecorator import FrameDecorator

from . import objects
from . import parser
from .ctree import *
from .errors import *

_dml_frame_filter = None

_indices_matcher = re.compile(r'.+\[([0-9]+)\]$')

def GdbDecorator(f):
    def wrapper(*args):
        try:
            return f(*args)
        except DMLError as e:
            print(e)
        except Exception:
            traceback.print_exc()
    return wrapper

def _lookup_method(obj, names):
    assert len(names) > 0
    indices = []
    for n in names:
        m = _indices_matcher.match(n)
        if m:
            indices.append(int(m.groups()[0]))
    exp = '.'.join(names)
    for m in obj.iter_components_rec('method'):
        if m.logname(indices).endswith(exp):
            return m
    return None

class DMLInfo:
    def __init__(self, ver, cls, dev, cfile, gblock):
        self.ver = ver
        self.cls = cls
        self.dev = dev
        self.cfile = cfile
        self.start = gblock.start
        self.end   = gblock.end

    def match_symtab(self, symtab):
        gblock = symtab.global_block()
        return gblock.start == self.start and gblock.end == self.end

    def lookup_method(self, funcname):
        for m in self.dev.iter_components_rec('method'):
            for f in m.funcs:
                if f[0] == funcname:
                    return (m, f)
        return (None, None)

    def lookup_function(self, f, names):
        if names[0] == self.cls:
            cls_matched = True
            names = names[1:]
        else:
            cls_matched = False
        m = _lookup_method(self.dev, names)
        if not m:
            return None
        blk = gdb.block_for_pc(self.start)
        if not blk:
            return None
        if len(m.funcs):
            func_name = m.funcs[0][0]
        else:
            return None
        (sym, _) = gdb.lookup_symbol(func_name, blk.static_block)
        if sym:
            if not sym.symtab.fullname().endswith(f):
                return None
            return (cls_matched, self.cfile, func_name)
        return None

    def search_dml_objects(self, names):
        return self.dev.search_objects(names)

_dbg_info = {}

def _search_dml_info_by_symtab(symtab):
    global _dbg_info
    for info in list(_dbg_info.values()):
        if info.match_symtab(symtab):
            return info
    return None

def _search_method_in_all_infos(func):
    global _dbg_info
    for info in list(_dbg_info.values()):
        (m, f) = info.lookup_method(func)
        if m:
            return m
    return None

def _load_dml_device(cfile, gfile, symtab):
    (_, ver, cls, dev) = [pickle.load(gfile) for _ in range(4)]  # nosec
    global _dbg_info
    global _dml_frame_filter
    _dbg_info[cls] = DMLInfo(ver, cls, objects.mkobj(dev, None),
                             cfile, symtab.global_block())
    if not _dml_frame_filter:
        _dml_frame_filter = FrameFilter()

_split_cfile_matcher = re.compile(r'(.+)-[0-9]+$')

def gdb_load_dml_device(cls):
    # TODO: we use a symbol name which is only existing in particular
    # dml device locate to the symtable. We need to figure out a better
    # solution for it.
    mangled_classname = re.sub(r'[^A-Za-z0-9_]', '_', cls)
    symname = '_dml_gdb_marker_' + mangled_classname
    sym = gdb.lookup_global_symbol(symname)
    if not sym:
        return
    cfile = sym.symtab.fullname()
    outprefix = os.path.splitext(cfile)[0]
    gfile = outprefix + '.g'
    gfile_found = os.path.exists(gfile)
    if not gfile_found:
        split_cfile = _split_cfile_matcher.match(outprefix)
        if split_cfile:
            gfile = split_cfile.group(1) + '.g'

    try:
        f = open(gfile, 'rb')
    except IOError:
        gdb.write('Cannot open file %s\n' % (gfile,), gdb.STDLOG)
    else:
        with f:
            _load_dml_device(cfile, f, sym.symtab)

class DMLFrameDecorator(FrameDecorator):
    def __init__(self, fobj):
        super(DMLFrameDecorator, self).__init__(fobj)
        self.fobj = fobj

    def function(self):
        frame = self.fobj.inferior_frame()
        sal = frame.find_sal()
        symtab = sal.symtab
        if not self.fobj.filename().endswith('.dml'):
            return super(DMLFrameDecorator, self).function()
        name = str(frame.name())
        info = _search_dml_info_by_symtab(symtab)
        if info:
            m, f = info.lookup_method(name)
            if m:
                indices = read_indices(frame, m.dimensions)
                return m.logname(indices)
        return name

class FrameFilter:
    def __init__(self):
        self.name = 'dml_filter'
        self.priority = 100
        self.enabled = True
        gdb.frame_filters[self.name] = self

    def filter(self, frame_iter):
        return map(DMLFrameDecorator, frame_iter)

def read_indices(frame, dimensions):
    block = frame.block()
    if not block.is_valid():
        return []
    indices = []
    for i in range(dimensions):
        s, _ = gdb.lookup_symbol('_idx%d' % i, block, gdb.SYMBOL_VAR_DOMAIN)
        if s is None or not s.is_valid():
            return None
        assert s.is_argument and s.needs_frame
        v = s.value(frame)
        indices.append(int(v))
    return tuple(indices)

@GdbDecorator
def eval_expression(expr_str):
    try:
        frame = gdb.selected_frame()
        sal = frame.find_sal()
    except gdb.error:
        # silently return if there isn't selected frame currently
        return None
    if not sal.symtab:
        return None
    info = _search_dml_info_by_symtab(sal.symtab)
    if not info:
        # Currently not in DML context
        return None
    meth, func = info.lookup_method(frame.name())
    if not meth:
        return None
    indices = [a_int(ind) for ind in read_indices(frame, meth.dimensions)]
    expr = parser.parse(expr_str).make(meth, indices)
    return expr.eval(indices).read()

def dml_symbol_name(sym):
    info = _search_dml_info_by_symtab(sym.symtab)
    if not info:
        return None
    meth, func = info.lookup_method(sym.name)
    if meth:
        return meth.logname('static')
    return None

@GdbDecorator
def dml_function_name(name):
    try:
        frame = gdb.selected_frame()
        sal = frame.find_sal()
    except gdb.error:
        # silently return if there isn't selected frame currently
        return None
    if not sal.symtab:
        return None
    info = _search_dml_info_by_symtab(sal.symtab)
    if not info:
        return None
    meth, func = info.lookup_method(name)
    if not meth:
        return None
    return meth.logname('static')

_bkpt_if_matcher               = re.compile(r'(.+) if (.+)')
_bkpt_ignore_matcher           = re.compile(r'[\+\*\-](.*)')
_bkpt_line_matcher             = re.compile(r'((.*): *)?([0-9]+)')
_bkpt_file_matcher             = re.compile(r'.*\.dml$')
_bkpt_func_matcher             = re.compile(
    r'(?:\$|dev\.)?[_a-zA-Z][_a-zA-Z0-9]*')
_bkpt_qualified_func_matcher   = re.compile(
    r'(?:\$|dev\.)[_a-zA-Z][_a-zA-Z0-9]*')
_bkpt_label_matcher            = re.compile(r'[_a-zA-Z][_a-zA-Z0-9]*')

assert all(_bkpt_func_matcher.match(valid)
           for valid in ('$foo', 'dev.foo', '$foo.bar', 'dev.foo.bar',
                         '$foo0', 'dev.foo0', 'foo', 'foo.bar', 'foo0',
                         '_foo', '$_foo', 'dev._foo'))
assert all(not _bkpt_func_matcher.match(invalid)
           for invalid in ('0foo', '$0foo'))
assert all(_bkpt_qualified_func_matcher.match(valid)
           for valid in ('$foo', 'dev.foo', '$foo.bar', 'dev.foo.bar',
                         '$foo0', 'dev.foo0', '$_foo', 'dev._foo'))
assert all(not _bkpt_qualified_func_matcher.match(invalid)
           for invalid in ('foo', 'foo.bar', '$0foo', 'dev.0foo'))

def _search_function_name(f, name):
    qualified_name = qualified(name)
    names = (qualified_name or name).split('.')
    if len(names) == 1 and not qualified_name:
        for info in (_dbg_info.values()):
            (sym, _) = gdb.lookup_symbol(
                names[0],
                gdb.block_for_pc(info.start).static_block)
            if sym and sym.is_function:
                return None

    rets = []
    for info in list(_dbg_info.values()):
        results = info.lookup_function(f, names)
        if results:
            rets.append(results)
    if not len(rets):
        return None
    if len(rets) == 1:
        return (f, rets[0][2])
    cm = [r for r in rets if r[0]]
    if len(cm):
        return (cm[0][1], cm[0][2])
    return (rets[0][1], rets[0][2])

def qualified(identifier):
    if identifier.startswith('$'):
        return identifier[1:]
    elif identifier.startswith('dev.'):
        return identifier[4:]
    else:
        return None

@GdbDecorator
def dml_eval_breakpoint_address(address):
    # b [file:]line
    # b [file:]func[:label]
    # b +offset
    # b -offset
    # b *addr
    # b ... if expr
    if _bkpt_ignore_matcher.match(address):
        return None
    if _bkpt_line_matcher.match(address):
        return None
    m = _bkpt_if_matcher.match(address)
    if m:
        addr, expr = m.groups()
    else:
        addr = address
        expr = ''
    parts = [x.strip() for x in addr.split(':', 3)]
    if len(parts) > 3:
        return None
    elif len(parts) == 3:
        f = parts[0]
        m = parts[1]
        l = parts[2]
    elif len(parts) == 2:
        if _bkpt_qualified_func_matcher.match(parts[0]):
            f = ''
            m = parts[0]
            l = parts[1]
        else:
            f = parts[0]
            m = parts[1]
            l = ''
    else:
        f = ''
        m = parts[0].strip()
        l = ''
    if f and not _bkpt_file_matcher.match(f):
        return None
    if not (m and _bkpt_func_matcher.match(m)):
        return None
    if l and not _bkpt_label_matcher.match(l):
        return None
    ret = _search_function_name(f, m)
    if not ret:
        return None
    ret = ':'.join([x for x in list(ret) + [l] if x])
    if expr:
        ret += ' if ' + expr
    return ret

@GdbDecorator
def dml_symbol_name_with_label(fun_with_label):
    if not ':' in fun_with_label:
        return None
    funname, label = fun_with_label.split(':', 1)
    m = _search_method_in_all_infos(funname)
    if not m:
        return None
    return m.logname('static') + ':' + label

from . import commands
