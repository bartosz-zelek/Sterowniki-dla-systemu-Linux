# This module tries to contain all the code that describes the
# encoding of device state in the C device structure.
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

from .objects import *

__all__ = (
    'cname',
    'cref',
    )

def cname(node):
    return node.name

def cref_rec(node):
    if node.objtype == 'device':
        return ''
    ref = cref_rec(node.parent)
    if ref and node.name:
        ref += '.' + cname(node)
    elif node.name:
        ref = cname(node)
    return ref

def cref(node, indices):
    if node.objtype == 'method':
        if node.parent.objtype == 'bank' and not node.parent.name:
            # This is a method on a nameless bank
            name = 'bank_'+cname(node)
        else:
            # This might actually conflict, but the chances are small.
            name = cref_rec(node.parent).replace('.', '__')
            # Hack around
            if node.parent.objtype == 'connect' and name.endswith("_obj"):
                name = name[:-4]
            if name:
                name += '__' + cname(node)
            else:
                name = cname(node)
        return name
    ref = cref_rec(node)
    if node.objtype in ('register', 'field') and not node.simple_storage:
        ref += '.__DMLfield'
    assert len(indices) == node.dimensions
    ref += ''.join(f'[{ind.read()}]' for ind in indices)

    return ref
