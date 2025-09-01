
# GDB extension: Simics commands
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

from . import utils
from .device import _search_dml_info_by_symtab

class SimicsCommand(gdb.Command):
    '''Command for showing information of DML device.'''
    def __init__(self):
        super(SimicsCommand, self).__init__('dml',
                                            gdb.COMMAND_DATA,
                                            gdb.COMPLETE_COMMAND,
                                            True)

    def invoke(self, arg, from_tty):
        if len(arg):
            print('Undefined dml command: "%s". Try "help dml".' % (arg,))
        else:
            print('"dml" must be followed by the name of a dml command.')
            print(gdb.execute('help dml', from_tty, True))

class SimicsDeviceCommand(gdb.Command):
    '''show the information of DML device in current context.

    Usage: dml device [name]
      name - name of interested DML object, it will show information
             of whole device if the object name hasn't been specified.

    The command will show all information when there isn't specified argument
'''
    def __init__(self):
        super(SimicsDeviceCommand, self).__init__(
            'dml device', gdb.COMMAND_DATA)

    def invoke(self, arg, from_tty):
        try:
            self._invoke(arg.replace(' ', ''))
        except KeyboardInterrupt:
            # Interrupting a long display by typing 'q' in a GDB console.
            # This should be suppressed.
            pass
        except Exception:
            # A traceback will be helpful when something went wrong.
            # GDB by default does not do that.
            import traceback
            traceback.print_exc()

    def _invoke(self, name):
        frame = gdb.selected_frame()
        info = None
        if frame:
            sal = frame.find_sal()
            if sal and sal.symtab:
                info = _search_dml_info_by_symtab(sal.symtab)
        if not info:
            return

        (m, _) = info.lookup_method(frame.name())
        if not m:
            return

        if name:
            rets = info.search_dml_objects(name.split('.'))
        else:
            rets = [info.dev]

        if rets:
            for c in rets:
                gdb.write('%s: %s\n' % (c.objtype, c.name))
                lst = []
                for s in c.get_components():
                    lst.append("%s: %s%s"
                               % (s.objtype, s.name, "".join(
                                   ["[%d]" % arrlen for arrlen
                                    in s.arraylens()])))
                for i in sorted(lst):
                    gdb.write('    %s\n' % i)
        else:
            print('no object named "%s"' % (name,))

SimicsCommand()
SimicsDeviceCommand()
