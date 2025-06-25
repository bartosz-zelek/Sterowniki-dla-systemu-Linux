#!/bin/bash

# © 2018 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

echo "Building kernel modules for Simics VMP"

function usage {
    echo "usage: vmp-kernel-build.sh [--help] [--quiet] [BUILD_DIR]"
    exit $1
}

function failure { echo "$@" ; exit 1 ; }

# We assume vmxmon source is distributed with Simics Base
SCRIPT_DIR=`dirname $0`
VMXMON_SRC_DIR=`realpath $SCRIPT_DIR/../vmxmon`
unset BUILD_DIR
unset QUIET

while [ "$1" ]; do
    case "$1" in
        --help|-h) usage 0 ;;
        --quiet|-q) QUIET=y ;;
        -*) usage 1 ;;
        *) test "$BUILD_DIR" && usage 1
           BUILD_DIR="$1" ;;
    esac
    shift
done

test "$BUILD_DIR" || BUILD_DIR="$VMXMON_SRC_DIR/build"

function check_result {
    # USAGE: check_result [logfile]
    test $? -eq 0 && { echo "ok" ; return 0 ; }
    echo "failed"
    test "$1" && cat "$1"
    failure "exiting"
}

# setup build directory (to check it's writable)
test ! -d "$BUILD_DIR" && {
    echo -n "using build directory $BUILD_DIR... "
    install -d "$BUILD_DIR"
    check_result $BUILD_DIR/build.log
}

# check that things are writable
{
    touch "$BUILD_DIR/check_writable"           \
        && rm -f "$BUILD_DIR/check_writable"    \
        && rm -f "$BUILD_DIR/build.log"
} >& /dev/null

test $? -eq 0 || {
    echo "Build directory $BUILD_DIR is not writable."
    echo "Give a writable directory as the first argument to this script"
    echo "to build from that location."
    exit 1
}

# perform the build
cd "$BUILD_DIR" || failure "exiting"

echo -n "setup build directory... "
cmake -S "$VMXMON_SRC_DIR" -B . -DDISABLE_TESTS=1 >& build.log
check_result build.log
echo -n "building vmxmon... "
cmake --build . --target vmxmon >& build.log
check_result build.log

# tell user about how to load/install the modules
if [ ! "$QUIET" ] ; then
    echo ""
    echo "To load the kernel modules, run the script:"
    echo "  $BUILD_DIR/scripts/load-modules"
    echo ""
    echo "To make kernel modules persistent across reboot, run the script:"
    echo "  $BUILD_DIR/scripts/install [--dry-run]"
fi
