# GDB extension: DML objects
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


import re
from . import utils

ID_PARAMETER = 0
ID_METHOD    = 1
ID_DATA      = 2
ID_REGISTER  = 3
ID_FIELD     = 4
ID_BANK      = 5
ID_CONNECT   = 6
ID_PORT      = 7
ID_ATTRIBUTE = 8
ID_EVENT     = 9
ID_IMPLEMENT = 10
ID_INTERFACE = 11
ID_GROUP     = 12
ID_DEVICE    = 13
ID_SUBDEVICE = 14
ID_HOOK      = 15

class DMLObject:
    def __init__(self, parent, name, arrinfo, subobjs):
        self.parent = parent
        self.name = name or ''
        # Verify structure of arrinfo
        for a in arrinfo:
            assert len(a) == 2
            a1, a2 = a
            assert a1 > 0
            assert a2
        arraylens, idxvars = list(zip(*arrinfo)) if arrinfo else ((), ())
        self.dimensions = len(arraylens)
        self._arraylens = arraylens
        self._idxvars = idxvars
        if parent:
            self.dimensions += parent.dimensions
        self._components = []
        self._component_dict = None
        if subobjs:
            for s in subobjs:
                self.add_component(mkobj(s, self))

    def __str__(self):
        return self.logname('static')

    def get_device(self):
        if isinstance(self, Device):
            return self
        return self.parent.get_device()

    def isindexed(self):
        return bool(self._arraylens)

    def arraylens(self):
        return self._arraylens

    def local_dimensions(self):
        return len(self._arraylens)

    def nonlocal_dimensions(self):
        return self.dimensions - self.local_dimensions()

    def logname(self, indices, relative='device'):
        """Return the qualified name of the object, relative to
        nearest ancestor of type 'relative'."""
        name = self.name or ""
        suff = ""
        if self.isindexed():
            suff += "".join('[%s]' % i for i in
                           indices[-self.local_dimensions():])
            suff += "".join('[%s]' % idxvar for idxvar in
                            self._idxvars[len(indices):])
            indices = indices[:-self.local_dimensions()]
        if self.parent and self.parent.objtype != relative:
            pname = self.parent.logname(indices, relative=relative)
            if not pname:
                pname = ''
            if not name:
                name = pname
            elif pname:
                name = pname + '.' + name
        return name + suff

    def add_component(self, comp):
        if comp.name:
            if not self._component_dict:
                self._component_dict = {}
            self._component_dict[comp.name] = comp
        self._components.append(comp)

    def get_component(self, name, objtype = None):
        if not self._component_dict:
            return None
        comp = self._component_dict.get(name)
        if comp and objtype and comp.objtype != objtype:
            comp = None
        return comp

    def get_components(self, *objtypes):
        if objtypes:
            comps = [ n for n in self._components if n.objtype in objtypes ]
        else:
            comps = [ n for n in self._components ]
        return comps

    def get_recursive_components(self, *objtypes):
        comps = []
        for n in self._components:
            if n.objtype in objtypes:
                comps.append(n)
            comps.extend(n.get_recursive_components(*objtypes))
        return comps

    def iter_components_rec(self, *objtypes):
        for s in self._components:
            if not objtypes or s.objtype in objtypes:
                yield s
            for ss in s.iter_components_rec(*objtypes):
                yield ss

class Device(DMLObject):
    objtype = 'device'
    def __init__(self, parent, id, name, structtype, subobjs):
        assert id == ID_DEVICE
        assert parent is None
        super().__init__(None, name, (), subobjs)
        self.structtype = structtype

    def search_objects(self, names):
        assert len(names) > 0
        indices = utils.read_dml_object_list_indices(names)
        exp = '.'.join(names)
        rets = []
        for c in self.iter_components_rec():
            lname = c.logname(indices)
            if lname == exp or lname.endswith('.' + exp):
                rets.append(c)
        return rets

class Subdevice(DMLObject):
    objtype = 'subdevice'
    def __init__(self, parent, id, name, arrinfo, subobjs):
        assert id == ID_SUBDEVICE
        super().__init__(parent, name, arrinfo, subobjs)

class Group(DMLObject):
    objtype = 'group'
    def __init__(self, parent, id, name, arrinfo, subobjs):
        assert id == ID_GROUP
        super().__init__(parent, name, arrinfo, subobjs)

class Bank(DMLObject):
    objtype = 'bank'
    def __init__(self, parent, id, name, arrinfo, subobjs):
        assert id == ID_BANK
        super().__init__(parent, name, arrinfo, subobjs)

class Register(DMLObject):
    objtype = 'register'
    def __init__(self, parent, id, name, arrinfo, subobjs):
        assert id == ID_REGISTER
        super().__init__(parent, name, arrinfo, subobjs)
        self.wholefield = True
    @property
    def simple_storage(self):
        return self.wholefield and \
               not bool(self.get_recursive_components('data'))

class Field(DMLObject):
    objtype = 'field'
    def __init__(self, parent, id, name, arrinfo, subobjs):
        assert id == ID_FIELD
        super().__init__(parent, name, arrinfo, subobjs)
        parent.wholefield = False
    @property
    def simple_storage(self):
        """Return true if the allocated value is stored in struct member
        __DMLfield."""
        return not bool(self.get_recursive_components('data'))

class Connect(DMLObject):
    objtype = 'connect'
    def __init__(self, parent, id, name, arrinfo, subobjs):
        assert id == ID_CONNECT
        super().__init__(parent, name, arrinfo, subobjs)

class Interface(DMLObject):
    objtype = 'interface'
    def __init__(self, parent, id, name, subobjs):
        assert id == ID_INTERFACE
        super().__init__(parent, name, (), subobjs)

class Attribute(DMLObject):
    objtype = 'attribute'
    def __init__(self, parent, id, name, arrinfo, subobjs):
        assert id == ID_ATTRIBUTE
        super().__init__(parent, name, arrinfo, subobjs)

class Event(DMLObject):
    objtype = 'event'
    def __init__(self, parent, id, name, arrinfo, subobjs):
        assert id == ID_EVENT
        super().__init__(parent, name, arrinfo, subobjs)

class Port(DMLObject):
    objtype = 'port'
    def __init__(self, parent, id, name, arrinfo, subobjs):
        assert id == ID_PORT
        super().__init__(parent, name, arrinfo, subobjs)

class Implement(DMLObject):
    objtype = 'implement'
    def __init__(self, parent, id, name, subobjs):
        assert id == ID_IMPLEMENT
        super().__init__(parent, name, (), subobjs)

class Data(DMLObject):
    objtype = 'data'
    def __init__(self, parent, id, name, ctype):
        assert id == ID_DATA
        super().__init__(parent, name, (), None)
        self.ctype = ctype

class Parameter(DMLObject):
    objtype = 'parameter'
    def __init__(self, parent, id, name, expr):
        assert id == ID_PARAMETER
        super().__init__(parent, name, (), None)
        self.pval = expr

class Method(DMLObject):
    objtype = 'method'
    def __init__(self, parent, id, name, funcs):
        assert id == ID_METHOD
        super().__init__(parent, name, (), None)
        self.funcs = funcs

class Hook(DMLObject):
    objtype = 'hook'
    def __init__(self, parent, id, name, arrinfo, msg_types):
        assert id == ID_HOOK
        super().__init__(parent, name, arrinfo, None)
        self.msg_types = msg_types

# Ordered by object code
# keep this consistent with g_backend.py!
obj_ctors = (
    Parameter,
    Method,
    Data,
    Register,
    Field,
    Bank,
    Connect,
    Port,
    Attribute,
    Event,
    Implement,
    Interface,
    Group,
    Device,
    Subdevice,
    Hook
)

def mkobj(mdata, parent):
    return obj_ctors[mdata[0]](parent, *mdata)
