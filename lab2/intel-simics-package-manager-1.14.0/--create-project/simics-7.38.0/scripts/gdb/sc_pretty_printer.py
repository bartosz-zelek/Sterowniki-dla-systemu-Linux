# GDB SystemC pretty printer
# Copyright 2016-2021 Intel Corporation

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

import gdb.printing
import sys

def enum_name(e):
    return str(e).split('::')[-1]

def get_nodes(vector):
    first = vector['_M_impl']['_M_start']
    last = vector['_M_impl']['_M_finish']
    if first.is_optimized_out or last.is_optimized_out:
        yield []

    for i in range(int(last - first)):
        item = (first + i).dereference()
        if item:
            yield (i, item)

def get_num_nodes(vector):
    first = vector['_M_impl']['_M_start']
    last = vector['_M_impl']['_M_finish']
    return int(last - first)


class GenericPayloadPrinter:

    def __init__(self, obj):
        self._object = obj
        self.incorrect_byte_enable = False

    def children(self):
        for field in self._object.type.fields():
            name = field.name
            # _vptr.tlm_generic_payload_printer is added by GDB/CDT and
            # should not be displayed to users
            if not name == '_vptr.tlm_generic_payload':
                yield (name, self._object[name])

    def to_string(self):
        data = self.get_data()
        extensions = TlmArrayPrinter(self._object['m_extensions'])

        desc = ' @0x%x, %s, %s, %s%s'
        return desc % (self._object['m_address'],
                       enum_name(self._object['m_command']),
                       enum_name(self._object['m_response_status']),
                       '%s, ' % self.present_data(data) if data else '',
                       extensions.to_string())

    def get_data(self):
        data_addr = self._object['m_data']
        if not data_addr:
            return []

        mask = self.get_mask()
        data_length = int(self._object['m_length'])
        data_length = 32 if data_length > 32 else data_length
        return [int((data_addr + i).dereference())
                for i in range(data_length)
                if not mask or mask[i % len(mask)]]

    def get_mask(self):
        filter_addr = self._object['m_byte_enable']
        if not filter_addr:
            return []

        filter_length = int(self._object['m_byte_enable_length'])
        filter_length = filter_length if filter_length < 32 else 32
        mask = []
        for i in range(filter_length):
            val = int((filter_addr + i).dereference())
            if val == 0:
                mask.append(False)
            elif val == 255:
                mask.append(True)
            else:
                # Incorrect value in byte_enable
                self.incorrect_byte_enable = val
                return []
        return mask

    def present_data(self, data):
        length = len(data)
        if length <= 8:
            return self.present_hexnumber(data)
        else:
            return self.present_memory(data, length)

    def present_hexnumber(self, data):
        endian = sys.byteorder
        if endian == 'little':
            return '0x' + ''.join([format(d, '02X')
                                    for d in reversed(data)])
        elif endian == 'big':
            return '0x' + ''.join([format(d, '02X')
                                    for d in data])
        else:
            return '<unknown byte order>'

    def present_memory(self, data, length):
        desc = '['
        for x in range(length):
            if x % 4 == 0 and x > 0:
                desc += ' '
            desc += format(data[x], '02X')
        if int(self._object['m_length']) > 32 and length >= 32:
            # Inform user that there is more data in memory
            desc += '...'
        desc += ']'
        if self.incorrect_byte_enable:
            # Inform user about broken byte enable
            desc += (' Incorrect byte_enable, value %x not allowed'
                    % self.incorrect_byte_enable)
        return desc


class TlmArrayPrinter:

    def __init__(self, val):
        self._object = val

    def map_info(self, extension):
        map_info = extension['map_info_']
        return ': (base=%s, start=%s, length=%s, function=%s)' % \
            (map_info['base'],
             map_info['start'],
             map_info['length'],
             map_info['function'])

    def transaction(self, extension):
        status = extension['status_']['status_']
        debug = extension['transport_debug_']
        return ': (status=%s, debug=%s)' % (enum_name(status), debug)

    def is_sticky(self, ext_node):
        for (_, node) in get_nodes(self._object['m_entries']):
            if node == ext_node:
                return True
        return False

    def to_string(self):
        desc = '{ '

        for (index, node) in get_nodes(self._object):
            n = node.dereference()
            ext = n.cast(n.dynamic_type)
            name = ext.type.name
            desc += enum_name(name)
            desc += '[S]' if self.is_sticky(index) else ''

            if name == 'simics::systemc::iface::MapInfoExtension':
                desc += self.map_info(ext)
            elif name == 'simics::systemc::iface::TransactionExtension':
                desc += self.transaction(ext)
            else:
                try:
                    desc += ': (' + Extension(ext).to_string() + ')'
                except:
                    # Only display name if unknown extension
                    pass
            desc += ', '
        desc = desc[: -2] if desc.endswith(', ') else desc
        return desc + ' }'

    def children(self):
        result = []
        for (index, node) in get_nodes(self._object):
            ext = node.dereference()
            result.append(('[%d]' % index, ext.cast(ext.dynamic_type)))
        return result

    def display_hint(self):
        return 'array'


class Extension:

    def __init__(self, val):
        self._object = val

    def get_value(self, any_type):
        v = any_type['value_']
        if v:
            return v.cast(v.dynamic_type).dereference()
        return None

    def to_string(self):
        desc = ''
        ret = self.get_value(self._object['method_return_'])
        if ret and 'value_' in ret.type:
            desc += 'return=' + ret['value_'] + ', '
        met = self.get_value(self._object['method_'])
        if met and 'value_' in met.type:
            desc += 'method=' + enum_name(met['value_']) + ', '
        nodes = get_nodes(self._object['method_input_'])
        if nodes:
            desc += 'input=('
        for (_, node) in nodes:
            inp = self.get_value(node)
            if 'value_' in inp.type:
                if inp['value_'].type.name.startswith('std::vector'):
                    desc += 'std::vector, '
                else:
                    desc += str(inp['value_']) + ', '
        if desc.endswith(', '):
            desc = desc[:-2]
        if nodes:
            desc += ')'
        return desc

    def children(self):
        result = []

        ret = self.get_value(self._object['method_return_'])
        if ret and 'value_' in ret.type:
            result.append(('return', ret['value_']))
        met = self.get_value(self._object['method_'])
        if met and 'value_' in met.type:
            result.append(('method', met['value_']))
        for (index, node) in get_nodes(self._object['method_input_']):
            inp = self.get_value(node)
            if 'value_' in inp.type:
                result.append(('param [%d]' % index, inp['value_']))
        return result


class MapIterator:

    def __init__(self, tree):
        self.size = tree['_M_t']['_M_impl']['_M_node_count']
        self.node = tree['_M_t']['_M_impl']['_M_header']['_M_left']
        self.count = 0

    def __iter__(self):
        return self

    def __len__(self):
        return int(self.size)

    def __next__(self):
        if self.count == self.size:
            raise StopIteration

        pair = self.node
        self.count = self.count + 1
        if self.count < self.size:
            # Reused from pretty-printers for libstdc++.
            node = self.node
            if node.dereference()['_M_right']:
                node = node.dereference()['_M_right']
                while node.dereference()['_M_left']:
                    node = node.dereference()['_M_left']
            else:
                parent = node.dereference()['_M_parent']
                while node == parent.dereference()['_M_right']:
                    node = parent
                    parent = parent.dereference()['_M_parent']
                if node.dereference()['_M_right'] != parent:
                    node = parent
            self.node = node

        return pair

    def next(self):
        return self.__next__()


class Sightings:

    def __init__(self, val):
        self._object = val

    def to_string(self):
        num = get_num_nodes(self._object)
        return '1 sighting' if num == 1 else '%d sightings' % num

    def children(self):
        result = []
        for (index, node) in get_nodes(self._object):
            result.append(('[%d]' % index, node))
        return result

    def display_hint(self):
        return 'array'

def get_value_from_Rb_tree_node(node):
    """Returns the value held in an _Rb_tree_node<_Val>"""
    try:
        member = node.type.fields()[1].name
        if member == '_M_value_field':
            # C++03 implementation, node contains the value as a member
            return node['_M_value_field']
        elif member == '_M_storage':
            # C++11 implementation, node stores value in __aligned_membuf
            p = node['_M_storage']['_M_storage'].address
            p = p.cast(node.type.template_argument(0).pointer())
            return p.dereference()
    except:
        pass
    raise ValueError("Unsupported implementation for %s" % str(node.type))

class TransactionTrackerExtension:

    class Iterator:
        def __init__(self, it, type):
            self.it = it
            self.count = 0
            self.type = type

        def __iter__(self):
            return self

        def __next__(self):
            if self.count % 2 == 0:
                n = next(self.it)
                node = n.cast(self.type).dereference()
                self.pair = get_value_from_Rb_tree_node(node)
                self.tool = self.pair['first']
                result = (self.tool.string(), self.tool)
            else:
                result = (self.tool.string() + '_sightings',
                          self.pair['second'])
            self.count += 1
            return result

        def next(self):
            return self.__next__()

    def lookup_inner_type(self, scope, name):
        s = scope.strip_typedefs()
        return gdb.lookup_type('%s::%s' % (s.unqualified(), name))

    def __init__(self, val):
        self._object = val

    def to_string(self):
        num = len(MapIterator(self._object['tool_sightings_']))
        res = 'transaction history by %d tool' % num
        return res if num == 1 else res + 's'

    def children(self):
        sightings = self._object['tool_sightings_']
        rep_type = self.lookup_inner_type(sightings.type, '_Rep_type')
        link_type = self.lookup_inner_type(
            rep_type, '_Link_type').strip_typedefs()
        return self.Iterator(MapIterator(sightings), link_type)

    def display_hint(self):
        return 'map'


def build_pretty_printer():
    pp = gdb.printing.RegexpCollectionPrettyPrinter('systemc')
    pp.add_printer('tlm::tlm_generic_payload', '^tlm::tlm_generic_payload$',
                    GenericPayloadPrinter)
    pp.add_printer('tlm::tlm_array', '^tlm::tlm_array<.*>$',
                    TlmArrayPrinter)
    pp.add_printer('simics::systemc::iface::Extension',
                   '^simics::systemc::iface::Extension<.*>$',
                    Extension)
    pp.add_printer('simics::systemc::tools::TransactionTrackerExtension::Sightings',
                   '^std::vector<simics::systemc::tools::TransactionTrackerExtension::Sighting,.*>$',
                    Sightings)
    pp.add_printer('simics::systemc::tools::TransactionTrackerExtension',
                   '^simics::systemc::tools::TransactionTrackerExtension$',
                    TransactionTrackerExtension)
    return pp

gdb.printing.register_pretty_printer(gdb.current_objfile(),
                                     build_pretty_printer(), replace=True)
