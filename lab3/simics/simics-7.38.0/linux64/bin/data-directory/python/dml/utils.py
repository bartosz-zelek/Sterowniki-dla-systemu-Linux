# Utils
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

_indices_matcher = re.compile(r'[^\[]+((\[\ *[0-9]+\ *\])+)$')
_index_matcher = re.compile(r'\[\ *([0-9]+)\ *\]')

def read_dml_object_indices(obj_name):
    '''return an array of indices from the obj_name
    obj_name is DML object name without $'''
    return read_dml_object_list_indices(obj_name.split('.'))

def read_dml_object_list_indices(obj_names):
    '''return an array of indices from the obj_names
    obj_names is a split DML object name without $ or .'''
    indices = []
    for n in obj_names:
        m = _indices_matcher.match(n)
        if m:
            gm = m.group(1)
            indices.extend([int(i) for i in _index_matcher.findall(gm)])
    return indices
