# GDB extension: DML errors
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

__all__ = (
    'DMLError',
    'ICE',
    'EREF',
    'ESYNTAX',
    'EUNDEF',
    'EREGVAL',
    'ENALLOC',
    'EARRAY',
    'EDIVZ',
    'ENOPTR',
    'EOOB',
    'EVINDEX',
    )

class DMLError(Exception):
    def __init__(self, msg):
        self.msg = self.fmt % msg
    def __str__(self):
        return self.msg

class ESYNTAX(DMLError):
    def __init__(self, tok, msg):
        if msg:
            self.msg = ("syntax error at '%s':%d: %s"
                        % (tok.value, tok.lexpos, msg))
        elif tok:
            self.msg = "syntax error at '%s':%d" % (tok.value, tok.lexpos)
        else:
            self.msg = "syntax error"

class ICE(Exception):
    fmt = "internal error: %s"

class EREF(DMLError):
    fmt = "reference to unknown object: '%s'"
    def __init__(self, name, obj = None):
        if obj is None:
            ref = name
        else:
            ref = "%s.%s" % (obj, name)
        DMLError.__init__(self, ref)

class EREGVAL(DMLError):
    fmt = "cannot use a register with fields as a value: %s"

class EDIVZ(DMLError):
    fmt = "right-hand side operand of '%s' is zero"

class ENOPTR(DMLError):
    fmt = "not a pointer: %s"

class EOOB(DMLError):
    fmt = "array index out of bounds: %s"

class EUNDEF(DMLError):
    def __init__(self):
        self.msg = "undefined value"

class ENALLOC(DMLError):
    """Trying to read non-allocated registers/fields."""
    fmt = "object is not allocated at run-time: %s"

class EARRAY(DMLError):
    """ This is a reference to an array, but no array index has
    been specified."""
    fmt = "cannot use an array as a value: '%s'"

class EVINDEX(DMLError):
    fmt = "index variable error: '%s'"

class EASSIGN(DMLError):
    fmt = "not assignable: '%s'"
