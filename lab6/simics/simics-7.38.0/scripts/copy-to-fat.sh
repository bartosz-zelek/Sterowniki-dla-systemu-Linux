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
    echo "Usage: $0 <imgfile> [-r] <files> <destination>"
}

if [ x$1 = x ]
then
    usage
    exit 1
fi
IMG=$1

shift
if [ x$1 = x ]
then
    usage
    exit 1
fi

COPY_OPTIONS=""
if [ $1 = "-r" ]
then
    COPY_OPTIONS="-s"
    shift
fi

if [ x$2 = x ]
then
    usage
    exit 1
fi

LAST=""
FILES=""
while [ "$1" != "" ]
do
    FILES="$FILES $LAST"
    LAST=$1
    shift
done

DESTINATION=$LAST

# Create mtoolsrc
mtoolsrc=`mktemp`
echo "drive m:" >> $mtoolsrc
echo "  file=\"$IMG\"" >> $mtoolsrc
export MTOOLSRC=$mtoolsrc

MTOOLS_NO_VFAT=0 mcopy $COPY_OPTIONS $FILES m:$DESTINATION

rm $mtoolsrc
