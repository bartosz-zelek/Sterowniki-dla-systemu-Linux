#-----------------------------------------------------------------------------
# Copyright (c) 2004  Raymond L. Buvel
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#-----------------------------------------------------------------------------
'''crcmod is a Python module for generating objects that compute the Cyclic
Redundancy Check.  Any 8, 16, 32, or 64 bit polynomial can be used.  

The following are the public components of this module.

Crc -- a class that creates instances providing the same interface as the
md5 and sha modules in the Python standard library.  These instances also
provide a method for generating a C/C++ function to compute the CRC.

mkCrcFun -- create a Python function to compute the CRC using the specified
polynomial and initial value.  This provides a much simpler interface if
all you need is a function for CRC calculation.
'''

__all__ = '''mkCrcFun Crc
'''.split()

# Select the appropriate set of low-level CRC functions for this installation.
# If the extension module was not built, drop back to the Python implementation
# even though it is significantly slower.
try:
    import _crcfunext as _crcfun
    _usingExtension = True
except ImportError:
    import _crcfunpy as _crcfun
    _usingExtension = False

import sys, struct

#-----------------------------------------------------------------------------
class Crc(object):
    '''Compute a Cyclic Redundancy Check (CRC) using the specified polynomial.

    Instances of this class have the same interface as the md5 and sha modules
    in the Python standard library.  See the documentation for these modules
    for examples of how to use a Crc instance.

    The string representation of a Crc instance identifies the polynomial and
    the initial and current CRC values.  The print statement can be used to
    output this information.

    If you need to generate a C/C++ function for use in another application,
    use the generateCode method.  If you need to generate code for another
    language, subclass Crc and override the generateCode method.

    The following are the parameters supplied to the constructor.

    poly -- The generator polynomial to use in calculating the CRC.  The value
    is specified as a Python integer or long integer.  The bits in this integer
    are the coefficients of the polynomial.  The only polynomials allowed are
    those that generate 8, 16, 32, or 64 bit CRCs.

    initCrc -- Initial value used to start the CRC calculation.  Defaults to
    all bits set because that starting value will take leading zero bytes into
    account.  Starting with zero will ignore all leading zero bytes.

    rev -- A flag that selects a bit reversed algorithm when True.  Defaults to
    True because the bit reversed algorithms are more efficient.
    '''
    def __init__(self, poly, initCrc=~0, rev=True, initialize=True):
        if not initialize:
            # Don't want to perform the initialization when using new or copy
            # to create a new instance.
            return

        x = _mkCrcFun(poly, initCrc, rev)
        self._crc = x[0]
        self.digest_size = (x[1] + 7) //8
        self.initCrc = x[2]
        self.table = x[3]
        self.crcValue = self.initCrc
        self.reverse = rev
        self.poly = poly

    def __str__(self):
        lst = []
        lst.append('poly = 0x%X' % self.poly)
        lst.append('reverse = %s' % self.reverse)
        fmt = '0x%%0%dX' % (self.digest_size*2)
        lst.append('initCrc  = %s' % (fmt % self.initCrc))
        lst.append('crcValue = %s' % (fmt % self.crcValue))
        return '\n'.join(lst)

    def new(self, arg=None):
        '''Create a new instance of the Crc class initialized to the same
        values as the original instance.  The current CRC is set to the initial
        value.  If a string is provided in the optional arg parameter, it is
        passed to the update method.
        '''
        n = Crc(poly=None, initialize=False)
        n._crc = self._crc
        n.digest_size = self.digest_size
        n.initCrc = self.initCrc
        n.table = self.table
        n.crcValue = self.initCrc
        n.reverse = self.reverse
        n.poly = self.poly
        if arg is not None:
            n.update(arg)
        return n

    def copy(self):
        '''Create a new instance of the Crc class initialized to the same
        values as the original instance.  The current CRC is set to the current
        value.  This allows multiple CRC calculations using a common initial
        string.
        '''
        c = self.new()
        c.crcValue = self.crcValue
        return c

    def update(self, data):
        '''Update the current CRC value using the string specified as the data
        parameter.
        '''
        self.crcValue = self._crc(data, self.crcValue)

    def digest(self):
        '''Return the current CRC value as a string of bytes.  The length of
        this string is specified in the digest_size attribute.
        '''
        n = self.digest_size
        crc = self.crcValue
        lst = []
        while n > 0:
            lst.append(chr(crc & 0xFF))
            crc = crc >> 8
            n -= 1
        lst.reverse()
        return ''.join(lst)

    def hexdigest(self):
        '''Return the current CRC value as a string of hex digits.  The length
        of this string is twice the digest_size attribute.
        '''
        n = self.digest_size
        crc = self.crcValue
        lst = []
        while n > 0:
            lst.append('%02X' % (crc & 0xFF))
            crc = crc >> 8
            n -= 1
        lst.reverse()
        return ''.join(lst)

    def generateCode(self, functionName, out, dataType=None, crcType=None):
        '''Generate a C/C++ function.

        functionName -- String specifying the name of the function.

        out -- An open file-like object with a write method.  This specifies
        where the generated code is written.

        dataType -- An optional parameter specifying the data type of the input
        data to the function.  Defaults to UINT8.

        crcType -- An optional parameter specifying the data type of the CRC
        value.  Defaults to one of UINT8, UINT16, UINT32, or UINT64 depending
        on the size of the CRC value.
        '''
        if dataType is None:
            dataType = 'UINT8'

        if crcType is None:
            crcType = 'UINT%d' % (8*self.digest_size)

        if self.digest_size == 1:
            # Both 8-bit CRC algorithms are the same
            crcAlgor = 'table[*data ^ (%s)crc]'
        elif self.reverse:
            # The bit reverse algorithms are all the same except for the data
            # type of the crc variable which is specified elsewhere.
            crcAlgor = 'table[*data ^ (%s)crc] ^ (crc >> 8)'
        else:
            # The forward CRC algorithms larger than 8 bits have an extra shift
            # operation to get the high byte.
            shift = 8*(self.digest_size - 1)
            crcAlgor = 'table[*data ^ (%%s)(crc >> %d)] ^ (crc << 8)' % shift

        fmt = '0x%%0%dX' % (2*self.digest_size)
        if self.digest_size <= 4:
            fmt = fmt + 'U,'
        else:
            # Need the long long type identifier to keep gcc from complaining.
            fmt = fmt + 'ULL,'

        # Select the number of entries per row in the output code.
        n = {1:8, 2:8, 4:4, 8:2}[self.digest_size]

        lst = []
        for i, val in enumerate(self.table):
            if (i % n) == 0:
                lst.append('\n    ')
            lst.append(fmt % val)

        poly = 'polynomial: 0x%X' % self.poly
        if self.reverse:
            poly = poly + ', bit reverse algorithm'

        parms = {
            'dataType' : dataType,
            'crcType' : crcType,
            'name' : functionName,
            'crcAlgor' : crcAlgor % dataType,
            'crcTable' : ''.join(lst),
            'poly' : poly,
        }
        out.write(_codeTemplate % parms) 

#-----------------------------------------------------------------------------
def mkCrcFun(poly, initCrc=~0, rev=True):
    '''Return a function that computes the CRC using the specified polynomial.

    poly -- integer representation of the generator polynomial
    initCrc -- default initial CRC value
    rev -- when true, indicates that the data is processed bit reversed.

    The returned function has the following user interface
    def crcfun(data, crc=initCrc):
    '''

    return _mkCrcFun(poly, initCrc, rev)[0]

#-----------------------------------------------------------------------------
# Naming convention:
# All function names ending with r are bit reverse variants of the ones
# without the r.

#-----------------------------------------------------------------------------
# Check the polynomial to make sure that it is acceptable and return the number
# of bits in the CRC.

def _verifyPoly(poly):
    msg = 'The degree of the polynomial must be 8, 16, 32 or 64'
    poly = int(poly) # Use a common representation for all operations
    for n in range(1, 65):
        low = 1<<n
        high = low*2
        if low <= poly < high:
            return n
    raise ValueError(msg)

#-----------------------------------------------------------------------------
# Bit reverse the input value.

def _bitrev(x, n):
    x = int(x)
    y = 0
    for i in range(n):
        y = (y << 1) | (x & 1)
        x = x >> 1
    if ((1<<n)-1) <= sys.maxsize:
        return int(y)
    return y

#-----------------------------------------------------------------------------
# The following functions compute the CRC for a single byte.  These are used
# to build up the tables needed in the CRC algorithm.  Assumes the high order
# bit of the polynomial has been stripped off.

def _bytecrc(crc, poly, n):
    crc = int(crc)
    poly = int(poly)
    mask = 1<<(n-1)
    for i in range(8):
        if crc & mask:
            crc = (crc << 1) ^ poly
        else:
            crc = crc << 1
    mask = (1<<n) - 1
    crc = crc & mask
    if mask <= sys.maxsize:
        return int(crc)
    return crc

def _bytecrc_r(crc, poly, n):
    crc = int(crc)
    poly = int(poly)
    for i in range(8):
        if crc & 1:
            crc = (crc >> 1) ^ poly
        else:
            crc = crc >> 1
    mask = (1<<n) - 1
    crc = crc & mask
    if mask <= sys.maxsize:
        return int(crc)
    return crc

#-----------------------------------------------------------------------------
# The following functions compute the table needed to compute the CRC.  The
# table is returned as a list.  Note that the array module does not support
# 64-bit integers on a 32-bit architecture as of Python 2.3.
#
# These routines assume that the polynomial and the number of bits in the CRC
# have been checked for validity by the caller.

def _mkTable(poly, n):
    mask = (1<<n) - 1
    poly = int(poly) & mask
    table = [_bytecrc(int(i)<<(n-8),poly,n) for i in range(256)]
    return table

def _mkTable_r(poly, n):
    mask = (1<<n) - 1
    poly = _bitrev(int(poly) & mask, n)
    table = [_bytecrc_r(int(i),poly,n) for i in range(256)]
    return table

#-----------------------------------------------------------------------------
# Map the CRC size onto the functions that handle these sizes.

_sizeMap = {
     8 : [_crcfun._crc8, _crcfun._crc8r],
    16 : [_crcfun._crc16, _crcfun._crc16r],
    32 : [_crcfun._crc32, _crcfun._crc32r],
    64 : [_crcfun._crc64, _crcfun._crc64r],
}

#-----------------------------------------------------------------------------
# Build a mapping of size to struct module type code.  This table is
# constructed dynamically so that it has the best chance of picking the best
# code to use for the platform we are running on.  This should properly adapt
# to 32 and 64 bit machines.

_sizeToTypeCode = {}

for typeCode in 'B H I L Q'.split():
    size = {1:8, 2:16, 4:32, 8:64}.get(struct.calcsize(typeCode),None)
    if size is not None and size not in _sizeToTypeCode:
        _sizeToTypeCode[size] = '256%s' % typeCode

del typeCode, size

#-----------------------------------------------------------------------------
# The following function returns a Python function to compute the CRC.  The
# returned function calls a low level function that is written in C if the
# extension module could be loaded.  Otherwise, a Python implementation is
# used.  In addition to this function, the size of the CRC, the initial CRC,
# and a list containing the CRC table are returned.

def _mkCrcFun(poly, initCrc, rev):
    size = _verifyPoly(poly)

    # Adjust the initial CRC to the correct data type (unsigned value).
    mask = (1<<size) - 1
    initCrc = int(initCrc) & mask
    if mask <= sys.maxsize:
        initCrc = int(initCrc)

    if size in [8, 16, 32, 64]:
        if rev:
            tableList = _mkTable_r(poly, size)
            _fun = _sizeMap[size][1]
        else:
            tableList = _mkTable(poly, size)
            _fun = _sizeMap[size][0]

        _table = tableList
        if _usingExtension:
            _table = struct.pack(_sizeToTypeCode[size], *tableList)

        def crcfun(data, crc=initCrc, table=_table, fun=_fun):
            return fun(data, crc, table)
    else:
        # Fall back on generic function
        assert not rev # TODO: implement bit-reversal
        tableList=None
        def crcfun(data, crc=initCrc, width=size, poly=poly):
            top_bit = 1 << (width - 1)

            for b in data:
                j = 0x80
                while j != 0:
                    bit = crc & top_bit
                    crc <<= 1

                    if (ord(b) & j):
                        bit ^= top_bit

                    if bit:
                        crc ^= poly
                    j >>= 1
            return crc & ((1 << width) - 1)

    return crcfun, size, initCrc, tableList

#-----------------------------------------------------------------------------
_codeTemplate = '''// Automatically generated CRC function
// %(poly)s
%(crcType)s
%(name)s(%(dataType)s *data, int len, %(crcType)s crc)
{
    static const %(crcType)s table[256] = {%(crcTable)s
    };
  
    while (len > 0)
    {
        crc = %(crcAlgor)s;
        data++;
        len--;
    }
    return crc;
}
'''

