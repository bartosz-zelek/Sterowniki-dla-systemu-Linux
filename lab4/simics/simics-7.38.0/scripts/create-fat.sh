#!/bin/bash

# © 2010 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

function usage() {
    echo "Usage: $0 <imgfile> [<size-in-megs>]"
}

if [ x$1 = x ]
then
    usage
    exit 1
fi

IMG=$1
SIZE_MEGS=64

if [ x$2 != x ]
then
    SIZE_MEGS=$2
fi

# Create mtoolsrc
mtoolsrc=`mktemp`
echo "drive m:" >> $mtoolsrc
echo "  file=\"$IMG\"" >> $mtoolsrc
export MTOOLSRC=$mtoolsrc

echo "Creating $SIZE_MEGS MiB FAT filesystem in image $IMG"

dd if=/dev/zero of=$IMG count=$SIZE_MEGS bs=1M 2> /dev/null
mformat -t `expr $SIZE_MEGS '*' 4` -h 16 -n 32 m:

rm $mtoolsrc
