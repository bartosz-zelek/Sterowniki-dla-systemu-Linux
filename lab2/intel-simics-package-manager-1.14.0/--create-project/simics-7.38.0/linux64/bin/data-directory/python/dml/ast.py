# AST
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

from .ctree import *

__all__ = ('Ast',
           'ast_assignment',
           'ast_conditional',
           'ast_binary',
           'ast_unary',
           'ast_var',
           'ast_objectref',
           'ast_member',
           'ast_indexed',
           'ast_cast',
           'ast_undefined',
           'ast_string',
           'ast_int',
           'ast_float',
           'ast_funcall',
           'ast_slice',
          )

class Ast:
    def make(self, objscope, indices):
        '''objscope - method object for current debug context
        indices - a tuple for the DML indices values'''
        assert False, 'Ast make'

class ast_assignment(Ast):
    def __init__(self, src, tgt):
        self.src = src
        self.tgt = tgt
    def make(self, objscope, indices):
        return a_assignment(self.src.make(objscope, indices),
                            self.tgt.make(objscope, indices))
class ast_conditional(Ast):
    def __init__(self, cond, t_ast, f_ast):
        self.cond = cond
        self.t_ast = t_ast
        self.f_ast = f_ast
    def make(self, objscope, indices):
        return a_conditional(self.cond.make(objscope, indices),
                             self.t_ast.make(objscope, indices),
                             self.f_ast.make(objscope, indices))

class ast_binary(Ast):
    def __init__(self, lhs, op, rhs):
        self.op = op
        self.lhs = lhs
        self.rhs = rhs
    def make(self, objscope, indices):
        lh = self.lhs.make(objscope, indices)
        rh = self.rhs.make(objscope, indices)
        op = self.op
        if op == '&&':   return a_and(lh.as_bool(), rh.as_bool())
        elif op == '||': return a_or(lh.as_bool(), rh.as_bool())
        elif op == '<':  return a_lt(lh, rh)
        elif op == '<=': return a_le(lh, rh)
        elif op == '>':  return a_bt(lh, rh)
        elif op == '>=': return a_be(lh, rh)
        elif op == '==': return a_eq(lh, rh)
        elif op == '!=': return a_ne(lh, rh)
        elif op == '&':  return a_band(lh, rh)
        elif op == '|':  return a_bor(lh, rh)
        elif op == '^':  return a_bxor(lh, rh)
        elif op == '<<': return a_shl(lh, rh)
        elif op == '>>': return a_shr(lh, rh)
        elif op == '*':  return a_mul(lh, rh)
        elif op == '/':  return a_div(lh, rh)
        elif op == '%':  return a_mod(lh, rh)
        elif op == '+':  return a_add(lh, rh)
        elif op == '-':  return a_sub(lh, rh)
        else:
            assert False, 'bad binary operator %s' % self.op

class ast_unary(Ast):
    def __init__(self, op, opr):
        self.op = op
        self.opr = opr
    def make(self, objscope, indices):
        e = self.opr.make(objscope, indices)
        if   self.op == '!':  return a_lnot(e.as_bool())
        elif self.op == '~':  return a_bnot(e)
        elif self.op == '-':  return a_minus(e)
        elif self.op == '+':  return a_plus(e)
        elif self.op == '&':  return a_addressof(e)
        elif self.op == '*':  return a_deref(e)
        elif self.op == 'sizeof':
            #TODO
            assert False
        elif self.op == 'defined': return a_defined(e)
        elif self.op == '#': return a_stringify(e)
        else:
            assert False, 'bad unary operator %s' % self.op
 
class ast_var(Ast):
    def __init__(self, name):
        self.name = name
    def make(self, objscope, indices):
        return a_var(self.name, objscope)

class ast_objectref(Ast):
    def __init__(self, name):
        self.name = name
    def make(self, objscope, indices):
        return a_nodename(self.name, objscope)

class ast_member(Ast):
    def __init__(self, lhs, op, rhs):
        assert op in ('.', '->')
        self.op = op
        self.lhs = lhs
        self.rhs = rhs
    def make(self, objscope, indices):
        lhs = self.lhs.make(objscope, indices)
        if self.op == '.':
            return a_dot(lhs, self.rhs)
        return a_arrow(lhs, self.rhs)

class ast_indexed(Ast):
    def __init__(self, lh, rh):
        self.lh = lh
        self.rh = rh

    def make(self, objscope, indices):
        lh = self.lh.make(objscope, indices)
        rh = self.rh.make(objscope, indices)
        return a_indexed(lh, rh)

class ast_funcall(Ast):
    def __init__(self, fun, args):
        self.fun = fun
        self.args = args

    def make(self, objscope, indices):
        fun = self.fun.make(objscope, indices)
        args = [a.make(objscope, indices) for a in self.args]
        return a_funcall(fun, args)

class ast_cast(Ast):
    def __init__(self, expr, casttype):
        self.expr = expr
        self.typ = casttype
    def make(self, objscope, indices):
        return a_cast(self.expr.make(objscope, indices), self.typ)

class ast_undefined(Ast):
    def make(self, objscope, indices):
        return a_undefined()

class ast_string(Ast):
    def __init__(self, val):
        self.val = val
    def make(self, objscope, indices):
        return a_string(self.val)

class ast_int(Ast):
    def __init__(self, val):
        self.val = val
    def make(self, objscope, indices):
        return a_int(self.val)

class ast_float(Ast):
    def __init__(self, val):
        self.val = val
    def make(self, objscope, indices):
        return a_float(self.val)

class ast_slice(Ast):
    def __init__(self, src, msb, lsb):
        self.src = src
        self.msb = msb
        self.lsb = lsb
    def make(self, objscope, indices):
        return a_bitslice(self.src.make(objscope, indices),
                          self.msb.make(objscope, indices),
                          self.lsb.make(objscope, indices))
