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


import sys
import os
from os.path import exists, basename, dirname, join

def clean_up_cache(name):
    base = basename(name)
    cache = join(dirname(name), "cache")
    try:
        for f in os.listdir(cache):
            if f.startswith(base):
                try:
                    os.remove(join(cache, f))
                except OSError:
                    pass
    except OSError:
        pass

def find_unused_name(name):
    i = 0
    try_name = name
    while exists(try_name):
        try_name = "%s.%d.bak" % (name, i)
        i = i + 1
    return try_name

def move(name):
    base = basename(name)
    cache = join(dirname(name), "cache")
    if not exists(cache):
        os.makedirs(cache)
    new_name = find_unused_name(join(cache, base))
    os.rename(name, new_name)

def rm_or_move(name):
    clean_up_cache(name)
    if exists(name):
        try:
            os.remove(name)
        except OSError:
            move(name)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: rm_or_move <file> ...")
        sys.exit(1)
    else:
        for a in sys.argv[1:]:
            rm_or_move(a)
