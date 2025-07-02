# DML expression
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


from . import crep, objects
from functools import total_ordering
import re
from .errors import *

__all__ = (
    'a_assignment',
    'a_conditional',
    'a_and', 'a_or',
    'a_lt', 'a_le', 'a_bt', 'a_be',
    'a_eq', 'a_ne',
    'a_band', 'a_bor', 'a_bxor', 'a_shl', 'a_shr',
    'a_add', 'a_sub', 'a_mul', 'a_div', 'a_mod',
    'a_lnot', 'a_bnot', 'a_minus', 'a_plus',
    'a_addressof', 'a_deref',
    'a_dot', 'a_arrow', 'a_indexed',
    'a_stringify', 'a_defined',
    'a_bool', 'a_int', 'a_float', 'a_funcall', 'a_string', 'a_var',
    'a_nodename', 'a_undefined', 'a_cast', 'a_bitslice')

class Expression:
    constant = False
    priority = 0
    def __str__(self):
        return '<unknown DML expression %r>' % (repr(self),)
    def read(self):
        assert False, 'Expression read'
    def eval(self, indices):
        assert False, 'Expression eval'
    def as_bool(self):
        assert False, 'Expression as_bool'
    def write(self):
        raise EASSIGN(self)

class LValue(Expression):
    def write(self, tgt):
        return f'{self.read()} = ({tgt})'

class a_assignment(Expression):
    priority = 20

    def __init__(self, src, tgt):
        self.src = src
        self.tgt = tgt

    def __str__(self):
        return f'{self.src} = {self.tgt}'

    def read(self):
        return self.src.write(self.tgt.read())

    def eval(self, indices):
        return a_assignment(self.src.eval(indices), self.tgt.eval(indices))

class a_conditional(Expression):
    priority = 30
    def __init__(self, cond, texpr, fexpr):
        self.cond = cond
        self.texpr = texpr
        self.fexpr = fexpr
    def __str__(self):
        return '%s ? %s : %s' % (self.cond, self.texpr, self.fexpr)
    def read(self):
        cond = self.cond.read()
        texpr = self.texpr.read()
        fexpr = self.fexpr.read()
        if self.texpr.priority <= self.priority:
            texpr = '(' + texpr + ')'
        if self.fexpr.priority <= self.priority:
            fexpr = '(' + fexpr + ')'
        return cond + ' ? ' + texpr + ' : ' + fexpr
    def eval(self, indices):
        cond = self.cond.eval(indices)
        if cond.constant:
            if cond.value:
                return self.texpr.eval(indices)
            return self.fexpr.eval(indices)
        return a_conditional(cond,
                             self.texpr.eval(indices),
                             self.fexpr.eval(indices))

class BinOp(Expression):
    def __init__(self, lh, rh):
        self.lh = lh
        self.rh = rh
    def __str__(self):
        lh = str(self.lh)
        rh = str(self.rh)
        if self.lh.priority <= self.priority:
            lh = '(' + lh + ')'
        if self.rh.priority <= self.priority:
            rh = '(' + rh + ')'
        return lh + ' ' + self.op + ' ' + rh
    def read(self):
        lh = self.lh.read()
        rh = self.rh.read()
        if self.lh.priority <= self.priority:
            lh = '(' + lh + ')'
        if self.rh.priority <= self.priority:
            rh = '(' + rh + ')'
        return lh + ' ' + self.op + ' ' + rh

class Test(Expression):
    type = 'TBool'
    def as_bool(self):
        return self

class Logical(BinOp, Test):
    pass

class a_or(Logical):
    priority = 40
    op = '||'
    def eval(self, indices):
        lh = self.lh.eval(indices)
        if lh.constant and lh.value:
            return a_bool(True)
        rh = self.rh.eval(indices)
        if rh.constant and rh.value:
            return a_bool(True)
        if lh.constant and rh.constant:
            return a_bool(False)
        return a_or(lh, rh)

class a_and(Logical):
    priority = 50
    op = '&&'
    def eval(self, indices):
        lh = self.lh.eval(indices)
        if lh.constant and not lh.value:
            return a_bool(False)
        rh = self.rh.eval(indices)
        if rh.constant and not rh.value:
            return a_bool(False)
        if lh.constant and rh.constant:
            return a_bool(True)
        return a_and(lh, rh)

class Compare(BinOp, Test):
    def eval(self, indices):
        lh = self.lh.eval(indices)
        rh = self.rh.eval(indices)
        return self.compare(lh, rh)
    def compare(self, lh, rh):
        assert False, 'Compare: compare'

class Relational(Compare):
    priority = 100

class Equality(Compare):
    priority = 90

class a_lt(Relational):
    op = '<'
    def compare(self, lh, rh):
        if lh.constant and rh.constant:
            return a_bool(lh.value < rh.value)
        return a_lt(lh, rh)

class a_le(Relational):
    op = '<='
    def compare(self, lh, rh):
        if lh.constant and rh.constant:
            return a_bool(lh.value <= rh.value)
        return a_le(lh, rh)

class a_bt(Relational):
    op = '>'
    def compare(self, lh, rh):
        if lh.constant and rh.constant:
            return a_bool(lh.value > rh.value)
        return a_bt(lh, rh)

class a_be(Relational):
    op = '>='
    def compare(self, lh, rh):
        if lh.constant and rh.constant:
            return a_bool(lh.value >= rh.value)
        return a_be(lh, rh)

class a_eq(Equality):
    op = '=='
    def compare(self, lh, rh):
        if lh.constant and rh.constant:
            return a_bool(lh.value == rh.value)
        return a_eq(lh, rh)

class a_ne(Equality):
    op = '!='
    def compare(self, lh, rh):
        if lh.constant and rh.constant:
            return a_bool(lh.value != rh.value)
        return a_ne(lh, rh)

class BitBinOp(BinOp):
    type = 'TInt'
    def eval(self, indices):
        lh = self.lh.eval(indices)
        rh = self.rh.eval(indices)
        return self.bitbin_op(lh, rh)
    def bitbin_op(self, lh, rh):
        assert False, 'BitBinOp: bitbin_op'

class a_band(BitBinOp):
    priority = 80
    op = '&'
    def bitbin_op(self, lh, rh):
        if lh.constant and rh.constant:
            return a_int(lh.value & rh.value)
        return a_band(lh, rh)

class a_bor(BitBinOp):
    priority = 60
    op = '|'
    def bitbin_op(self, lh, rh):
        if lh.constant and rh.constant:
            return a_int(lh.value | rh.value)
        return a_bor(lh, rh)

class a_bxor(BitBinOp):
    priority = 70
    op = '^'
    def bitbin_op(self, lh, rh):
        if lh.constant and rh.constant:
            return a_int(lh.value ^ rh.value)
        return a_bxor(lh, rh)

class BitShift(BitBinOp):
    priority = 110

class a_shl(BitShift):
    op = '<<'
    def bitbin_op(self, lh, rh):
        if lh.constant and rh.constant:
            return a_int(lh.value << rh.value)
        return a_shl(lh, rh)

class a_shr(BitShift):
    op = '>>'
    def bitbin_op(self, lh, rh):
        if lh.constant and rh.constant:
            return a_int(lh.value >> rh.value)
        return a_shr(lh, rh)

class Additive(BinOp):
    priority = 120

class a_add(Additive):
    op = '+'
    def eval(self, indices):
        lh = self.lh.eval(indices)
        rh = self.rh.eval(indices)
        # Constant folding
        if isinstance(lh, a_string) and isinstance(rh, a_string):
            return a_string(lh.value + rh.value)
        if isinstance(lh, (a_int, a_float)):
            if isinstance(lh, a_int) and isinstance(rh, a_int):
                return a_int(lh.value + rh.value)
            if isinstance(rh, (a_int, a_float)):
                return a_float(lh.value + rh.value)
        # Simplifications
        if lh.constant and lh.value == 0:
            return rh
        if rh.constant and rh.value == 0:
            return lh
        return a_add(lh, rh)

class a_sub(Additive):
    op = '-'
    def eval(self, indices):
        lh = self.lh.eval(indices)
        rh = self.rh.eval(indices)
        # Constant folding
        if isinstance(lh, (a_int, a_float)):
            if isinstance(lh, a_int) and isinstance(rh, a_int):
                return a_int(lh.value - rh.value)
            if isinstance(rh, (a_int, a_float)):
                return a_float(lh.value - rh.value)
        # Simplifications
        if rh.constant and rh.value == 0:
            return lh
        return a_sub(lh, rh)

class Multiplicative(BinOp):
    priority = 130
    def eval(self, indices):
        lh = self.lh.eval(indices)
        rh = self.rh.eval(indices)
        return self.eval_multiplicative(lh, rh)
    def eval_multiplicative(self, lh, rh):
        assert False, 'Multiplicative: eval_multiplicative'

class a_mul(Multiplicative):
    op = '*'
    def eval_multiplicative(self, lh, rh):
        if isinstance(lh, a_int) and isinstance(rh, a_int):
            return a_int(lh.value * rh.value)
        # don't try to be smart about floating-point
        # Simplify the code
        if isinstance(lh, a_int):
            if lh.value == 1:
                return rh
            if lh.value == 0:
                return lh
        if isinstance(rh, a_int):
            if rh.value == 1:
                return lh
            if rh.value == 0:
                return lh
        return a_mul(lh, rh)

class a_div(Multiplicative):
    op = '/'
    def eval_multiplicative(self, lh, rh):
        if isinstance(rh, a_int):
            if rh.value == 0:
                raise EDIVZ('/')
            elif rh.value == 1:
                return lh
            elif isinstance(lh, a_int):
                return a_int(lh.value // rh.value)
        return a_div(lh, rh)

class a_mod(Multiplicative):
    op = '%'
    def eval_multiplicative(self, lh, rh):
        if isinstance(rh, a_int):
            if rh.value == 0:
                raise EDIVZ('%')
            elif rh.value == 1:
                return a_int(0)
            elif isinstance(lh, a_int):
                return a_int(lh.value % rh.value)
        return a_mod(lh, rh)

class UnaryOp(Expression):
    priority = 150
    def __init__(self, rh):
        self.rh = rh
    def __str__(self):
        rh = str(self.rh)
        if self.rh.priority <= self.priority:
            rh = '(' + rh + ')'
        return str(self.op) + rh
    def read(self):
        rh = self.rh.read()
        if self.rh.priority <= self.priority:
            rh = '(' + rh + ')'
        return str(self.op) + rh
    def eval(self, indices):
        rh = self.rh.eval(indices)
        return self.eval_unary(rh)

class a_stringify(UnaryOp):
    op = '#'
    type = 'TString'
    def eval_unary(self, rh):
        return a_string(str(rh).encode('utf-8'))

class a_defined(UnaryOp):
    op = 'defined'
    type = 'TBool'
    def eval_unary(self, rh):
        return a_bool(defined(rh))

class a_minus(UnaryOp):
    op = '-'
    def eval_unary(self, rh):
        if isinstance(rh, a_int):
            return a_int(-rh.value)
        if isinstance(rh, a_float):
            return a_float(-rh.value)
        return a_minus(rh)

class a_plus(UnaryOp):
    op = '+'
    def eval_unary(self, rh):
        if isinstance(rh, (a_int, a_float)):
            return rh
        return a_plus(rh)

class a_lnot(UnaryOp):
    op = '!'
    type = 'TBool'
    def eval_unary(self, rh):
        if rh.constant:
            return a_bool(not rh.value)
        if isinstance(rh, a_eq):
            return a_ne(rh.lh, rh.rh)
        return a_lnot(rh)

class a_bnot(UnaryOp):
    op = '~'
    type = 'TInt'
    def eval_unary(self, rh):
        return a_bnot(rh)

class a_deref(UnaryOp, LValue):
    op = '*'
    def eval_unary(self, rh):
        return a_deref(rh)

class a_addressof(UnaryOp):
    op = '&'
    def eval_unary(self, rh):
        return a_addressof(rh)

class a_sizeof(UnaryOp):
    def eval_unary(self, rh):
        assert False, 'TODO'

class a_sizeoftype(UnaryOp):
    def eval_unary(self, rh):
        assert False, 'TODO'

@total_ordering
class Constant(Expression):
    constant = 1
    priority = 1000
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return str(self.value)
    def __eq__(self, other):
        return (self.value == other.value)
    def __ne__(self, other):
        return not (self == other)
    def __lt__(self, other):
        return (self.value < other.value)
    def eval(self, indices):
        return self

class a_int(Constant):
    type = 'TInt'
    def __init__(self, value):
        assert isinstance(value, int)
        Constant.__init__(self, int(value))
    def read(self):
        value = self.value
        if value < 0:
            sign = '-'
            sign_suffix = ''
            value = -value
        else:
            sign = ''
            sign_suffix = 'U'

        if value < 256:
            s = str(self.value)
        else:
            s = sign + '%#x' % value

        if value.bit_length() > 32:
            s += sign_suffix + 'LL'
        elif value > 0x7fffffff:
            s += sign_suffix

        return s
    def as_bool(self):
        return a_bool(self.value)

class a_float(Constant):
    type = 'TFloat'
    def __init__(self, value):
        assert isinstance(value, float)
        Constant.__init__(self, value)
    def read(self):
        return repr(self.value)

def char_escape(m):
    c = m.group(0)
    char_to_escape = {b'\a': b'\\a',
                      b'\b': b'\\b',
                      b'\t': b'\\t',
                      b'\n': b'\\n',
                      b'\v': b'\\v',
                      b'\f': b'\\f',
                      b'\r': b'\\r',
                      b'"':  b'\\"',
                      b'\\': b'\\\\',
                     }
    return char_to_escape.get(c, b'\\%03o' % ord(c))

char_escape_re = re.compile(b'[\x00-\x1f"\\\\\x7f-\xff]')

def string_escape(s):
    return char_escape_re.sub(char_escape, s).decode('utf-8')

class a_string(Constant):
    type = 'TPtr'
    def __init__(self, value):
        assert isinstance(value, bytes)
        Constant.__init__(self, value)
    @property
    def quoted(self):
        return '"' + string_escape(self.value) + '"'
    def __str__(self):
        return self.quoted
    def read(self):
        return self.quoted
    def as_bool(self):
        return a_bool(1)
    def unicode_value(self):
        return str(self.value, "UTF-8")

class a_bool(Constant):
    type = 'TBool'
    def __init__(self, value):
        Constant.__init__(self, not not value)
    def __str__(self):
        if self.value:
            return 'true'
        else:
            return 'false'
    def read(self):
        if self.value:
            return '1'
        else:
            return '0'

undefval = None
class a_undefined(Constant):
    type = 'TUndefined'
    def __init__(self):
        Constant.__init__(self, undefval)
    def __str__(self):
        return '<undefined>'
    def read(self):
        raise EUNDEF()
    def as_bool(self):
        raise EUNDEF()

undefval = a_undefined()

def undefined(expr):
    assert isinstance(expr, Expression)
    return isinstance(expr, a_undefined)

def defined(expr):
    return not undefined(expr)

class a_var(LValue):
    priority = 1000
    def __init__(self, name, base=None):
        self.name = name
        self.base = base
    def __str__(self):
        return self.name
    def read(self):
        return self.name
    def eval(self, indices):
        if self.base:
            import gdb
            (sym, _) = gdb.lookup_symbol(self.name,
                                         gdb.selected_frame().block(),
                                         gdb.SYMBOL_VAR_DOMAIN)
            if sym and sym.is_valid():
                return self
            e = lookup_component(self.base, indices, self.name, 0)
            if e is None:
                return self
            return e
        else:
            return self

class a_list(Expression):
    priority = 1000
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return "[%s]" % (','.join(str(x) for x in self.value))
    def read(self):
        return "[%s]" % (','.join(x.read() for x in self.value))
    def eval(self, indices):
        return self
    def as_bool(self):
        return a_bool(self.value)

def eval_parameter(base, pval, indices):
    from .parser import parse
    if isinstance(pval, type(())):
        # an object reference
        return parse(pval[0]).make(base, indices).eval(indices)
    elif isinstance(pval, type([])):
        return a_list([eval_parameter(base, v, indices) for v in pval])
    elif isinstance(pval, int):
        return a_int(pval)
    elif isinstance(pval, str):
        return a_string(pval.encode('utf-8'))
    elif isinstance(pval, float):
        return a_float(pval)
    elif isinstance(pval, bool):
        return a_bool(pval)
    elif pval is None:
        return a_undefined()
    else:
        raise ICE('bad parameter')

def lookup_component(base, indices, name, only_local):
    "Lookup a name in object scope and return an evaled Expression, or None."
    if base.objtype == 'method':
        base = base.parent

    # The "auto" parameters
    if name == 'dev':
        return a_noderef(base.get_device(), indices)
    elif name == 'this':
        return a_noderef(base, indices)
    elif name == 'parent':
        return a_noderef(base.parent if base.parent else base,
                         indices[:-base.local_dimensions()]
                         if base.isindexed() and indices != 'static'
                         else indices)
    elif name == 'name':
        return a_string(base.name.encode('utf-8'))
    elif name in base._idxvars:
        assert base.isindexed()
        dim = base._idxvars.index(name) + base.nonlocal_dimensions()
        if dim > len(indices) - 1:
            raise EVINDEX(base)
        return indices[dim]

    # Look at the level immediate below
    node = base.get_component(name)
    if node:
        if node.objtype == 'parameter':
            return eval_parameter(base, node.pval, indices)
        return a_noderef(node, indices)

    # Search upwards in outer scopes if it wasn't found
    if not only_local and base.parent:
        if base.isindexed() and indices != 'static':
            indices = indices[:-base.local_dimensions()]
        return lookup_component(base.parent, indices, name, only_local)

    # Last resort is to look for components in anonymous banks
    if base.objtype == 'device' or not only_local:
        for bank in base.get_components('bank'):
            if not bank.name:
                return lookup_component(bank, indices, name, only_local)

    return None

class a_nodename(Expression):
    priority = 1000
    def __init__(self, name, base):
        self.name = name
        self.base = base
    def __str__(self):
        return "$%s" % self.name
    def eval(self, indices):
        e = lookup_component(self.base, indices, self.name, 0)
        if e is None:
            raise EREF(self.name)
        return e

class Postfix(Expression):
    priority = 160

class SubRef(Postfix, LValue):
    def __init__(self, expr, sub):
        self.expr = expr
        self.sub = sub
    def __str__(self):
        s = str(self.expr)
        if self.expr.priority < self.priority:
            s = '(' + s + ')'
        return s + self.op + self.sub
    def read(self):
        s = self.expr.read()
        if self.expr.priority < self.priority:
            s = '(' + s + ')'
        return s + self.op + self.sub
    def eval(self, indices):
        expr = self.expr.eval(indices)
        # Simplification
        if isinstance(expr, a_addressof) and self.op == '->':
            return a_dot(expr.rh, self.sub).eval(indices)
        if isinstance(expr, a_deref) and self.op == '.':
            return a_arrow(expr.rh, self.sub).eval(indices)
        return self.eval_subref(expr, self.sub)

class a_dot(SubRef):
    op = '.'
    def eval_subref(self, expr, sub):
        if isinstance(expr, a_noderef):
            node, indices = expr.get_ref()
            if not expr.complete:
                raise EARRAY(node)
            e = lookup_component(node, indices, sub, 1)
            if e:
                return e
            if node.objtype == 'data':
                pass
            elif node.objtype == 'interface':
                return a_arrow(expr, sub)
            else:
                raise EREF(sub, expr)
        # TODO: struct field checks
        return a_dot(expr, sub)

class a_arrow(SubRef):
    op = '->'
    def eval_subref(self, expr, sub):
        if isinstance(expr, a_noderef):
            node, indices = expr.get_ref()
            if not expr.complete:
                raise EARRAY(node)
            if node.objtype == 'data':
                pass
            elif node.objtype == 'connect' and self.op == '->':
                # Connections can have subcomponents, but can
                # also be used as unknown references
                return a_arrow(expr, sub)
            else:
                raise ENOPTR(expr)
        # TODO: struct field checks
        return a_arrow(expr, sub)

class a_indexed(Postfix):
    def __init__(self, expr, idx):
        self.expr = expr
        self.idx = idx
    def __str__(self):
        return '%s[%s]' % (self.expr, self.idx)
    def eval(self, objindices):
        expr = self.expr.eval(objindices)
        index = self.idx.eval(objindices)
        if (isinstance(expr, a_noderef)
            and (not expr.complete or expr.indices == 'static')):
            node, indices = expr.get_ref()
            if (isinstance(index, a_int)
                and not (0 <= index.value < node.arraylens()[
                    len(indices) - node.local_dimensions()])):
                raise EOOB(index)
            if indices == 'static':
                indices = (index,)
            else:
                indices += (index,)
            return a_noderef(node, indices)
        return a_indexed(expr, index)

    def read(self):
        s = self.expr.read()
        return s + '[' + str(self.idx.value) + ']'

class a_funcall(Postfix):
    def __init__(self, fun, args):
        self.fun = fun
        self.args = args

    def __str__(self):
        return '%s(%s)' % (self.fun, ', '.join([a for a in self.args]))

    def eval(self, objindices):
        fun = self.fun.eval(objindices)
        args = [a.eval(objindices) for a in self.args]
        return a_funcall(fun, args)

    def read(self):
        s = self.fun.read()
        args = [a.read() for a in self.args]
        if isinstance(self.fun, a_noderef):
            indices = ([ind.read() for ind in self.fun.indices]
                       if self.fun.indices != 'static' else [])
            args = indices + args
        if isinstance(self.fun, a_noderef):
            node = self.fun.node
            if (isinstance(node, objects.Method)
                and (len(node.funcs) > 1
                     # second element of func tuple indicates whether the
                     # method is independent
                     or not node.funcs[0][1])):
                args = ['_dev'] + args
        return s + '(' + ', '.join(args) + ')'

class a_noderef(LValue):
    "A reference to a node in the device specification"
    priority = 1000
    def __init__(self, node, indices):
        if indices != 'static':
            if node.dimensions == len(indices):
                self.complete = True
            elif node.dimensions > len(indices) and node.isindexed():
                self.complete = False
            else:
                raise ICE("Reference to node with wrong number of indices: %r"
                           % node)
        else:
            self.complete = True
        self.node = node
        self.indices = indices
        self.type = None
    def __str__(self):
        name = self.node.logname(self.indices)
        if name:
            return name
        else:
            return '<anonymous %s>' % self.node.objtype
    def get_ref(self):
        return (self.node, self.indices)
    def read(self):
        if not self.complete:
            raise EARRAY(self.node)
        node = self.node
        if node.objtype == "register" and not node.wholefield:
            raise EREGVAL(node)
        if node.objtype in ('register', 'field'):
            allocate = node.get_component('allocate')
            if not (allocate and allocate.pval):
                raise ENALLOC(node)
        if self.indices == 'static':
            raise ICE("Can't read static reference: %r" % self)
        cname = crep.cref(node, self.indices)
        if node.objtype == 'method':
            cname = '_DML_M_' + cname
        elif cname:
            cname = '_dev->' + cname
        else:
            cname = '_dev'
        return cname

class a_cast(Expression):
    def __init__(self, expr, typ):
        self.expr = expr
        self.typ = typ

    def __str__(self):
        return f'cast({self.expr}, {self.typ})'

    def eval(self, objindices):
        return a_cast(self.expr.eval(objindices), self.typ)

    def read(self):
        return f'({self.typ})({self.expr.read()})'

class a_bitslice(LValue):
    priority = 0
    def __init__(self, expr, msb, lsb):
        self.expr = expr
        self.msb = msb
        self.lsb = lsb

    def __str__(self):
        return f'{self.expr}[{self.msb}:{self.lsb}]'

    def read(self):
        return ('((%s) >> (%s)) & ((1ULL << ((%s) - (%s) + 1ULL)) - 1ULL)'
                % (self.expr.read(), self.lsb.read(), self.msb.read(),
                   self.lsb.read()))

    def write(self, src):
        source = f'(uint64)({src}) << {self.lsb.read()}'
        mask = (f'((1ULL << (({self.msb.read()}) - ({self.lsb.read()}) '
                + f'+ 1ULL)) - 1ULL) << ({self.lsb.read()})')
        return self.expr.assign_to('DML_combine_bits(%s, %s, %s)'
                                   % (self.expr.read(), source, mask))

    def eval(self, indices):
        return a_bitslice(self.expr.eval(indices), self.msb.eval(indices),
                          self.lsb.eval(indices))
