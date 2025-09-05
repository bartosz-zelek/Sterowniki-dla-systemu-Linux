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

function usage {
    echo "usage: vmp-kernel-install.sh [--help] [--quiet] [--auto] [BUILD_DIR]"
    exit $1
}
function failure { echo "$@" ; exit 1 ; }

# We assume vmxmon source is distributed with Simics Base
SCRIPT_DIR=`dirname $0`
VMXMON_SRC_DIR=`realpath $SCRIPT_DIR/../vmxmon`
unset QUIET
unset BUILD_DIR
unset AUTO

while [ "$1" ]; do
    case "$1" in
        --help|-h) usage 0 ;;
        --quiet|-q) QUIET=y ;;
        --auto|-a) AUTO=y ;;
        -*) usage 1 ;;
        *) test "$BUILD_DIR" && usage 1
           BUILD_DIR="$1" ;;
    esac
    shift
done
test "$BUILD_DIR" || BUILD_DIR="$VMXMON_SRC_DIR/build"

test ! "$QUIET" && {
   echo ""
   echo "Setting up your system for Simics VMP"
   echo "================================================"
   echo ""
   echo "This step will be performed with root privilege through sudo:"
   echo ""
   echo "  - Loading of the vmxmon kernel module."
   echo ""
   echo "Unless this script is run as root, you will be prompted for the"
   echo "root password during those steps."
   echo ""
}

CHECK_BUILD="$BUILD_DIR/scripts/check-build"

# build modules (and utils) if necessary
if [ ! -x "$CHECK_BUILD" ] || ! "$CHECK_BUILD" >& /dev/null ; then
    $SCRIPT_DIR/vmp-kernel-build.sh --quiet "$BUILD_DIR" || exit $?
else
    echo "using previously built modules"
fi

if "$BUILD_DIR"/scripts/load-modules; then
    echo "loading kernel modules... ok"
else
    echo "exiting"
    exit 1
fi

if [[ -z "$AUTO" ]]; then
    test ! "$QUIET" && {
        echo ""
        echo "Your system is now ready to use Simics VMP."
        echo ""
        echo "To configure the system to load the modules automatically"
        echo "after a reboot, run the script"
        echo ""
        echo "   $BUILD_DIR/scripts/install [--dry-run]"
    }
else
    # Install script is chatty, ignore the --quiet condition
    echo "invoking install script @ $BUILD_DIR/scripts/install :"
    "$BUILD_DIR"/scripts/install || exit $?
    echo "install script... ok"
fi
