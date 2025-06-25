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


import sys
import os
import shutil
import argparse
import tempfile

is_windows = sys.platform.startswith('win')
if sys.version_info[0] >= 3:
    # os.replace is atomic on all platforms
    from os import replace
elif is_windows:
    # Approximation of atomic file move on Windows
    def replace(src, dest):
        if os.path.exists(dest):
            os.remove(dest)
        try:
            os.rename(src, dest)
        except OSError:
            pass
else:
    # os.rename is atomic on Linux
    from os import rename as replace

def fail(msg):
    print(msg, file=sys.stderr)
    sys.exit(1)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description = "Copy file",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("src", help="Source file")
    parser.add_argument("dest", help="Destination file")

    args = parser.parse_args()
    if os.path.isdir(args.dest):
        fail("Destination must be a file")

    # Make sure temp file is on the same file system as destination
    (handle, temp_file) = tempfile.mkstemp(prefix=os.path.basename(args.dest),
                                           dir=os.path.dirname(args.dest))
    os.close(handle)

    try:
        shutil.copyfile(args.src, temp_file)
    except IOError as e:
        fail("Could not copy '%s' to '%s': %s"
             % (args.src, temp_file, str(e)))

    try:
        replace(temp_file, args.dest)
    except OSError as e:
        fail("Could not rename '%s' to '%s': %s"
             % (temp_file, args.dest, str(e)))
    else:
        shutil.copymode(args.src, args.dest)
        shutil.copystat(args.src, args.dest)
