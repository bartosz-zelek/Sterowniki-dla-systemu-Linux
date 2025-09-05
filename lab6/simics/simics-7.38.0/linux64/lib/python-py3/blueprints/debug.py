# © 2024 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

import io
from typing import Any
from .types import State, List

def _tostr(v: Any) -> str:
    if isinstance(v, int) and v >= 10:
        return f"0x{v:x}"
    elif isinstance(v, list) or type(v) is tuple:
        return f"[{', '.join(_tostr(x) for x in v)}]"
    elif isinstance(v, tuple):
        return f"{v!r}"
    elif isinstance(v, str):
        return f"{v!r}"
    elif isinstance(v, State):
        return f"{v!r}"
    else:
        return f"{v!s}"

def _pretty_print(indent: str, val: Any, stream: io.TextIOBase, postfix=""):
    if isinstance(val, List) or type(val) is tuple:
        val = list(val)
    valstr = f"{_tostr(val)}{postfix}"
    if len(valstr) < 80 or not isinstance(val, list):
        print(f"{indent}{valstr}", file=stream)
    else:
        indent += "["
        for i, v in enumerate(val):
            _pretty_print(indent, v, stream,
                          postfix="]" if i == len(val) - 1 else ",")
            indent = " " * len(indent)

def _print_iface(iface: State, field_pat: str, stream: io.TextIOBase):
    for k in iface._keys:
        if not field_pat or field_pat in k:
            _pretty_print(f"  {k:24}", getattr(iface, k), stream)

def _print_state(state: list, stream: io.TextIOBase,
                 field_pat="", iface_name="", pat=""):
    for iface in state:
        # Lists have been displayed "inline"
        if isinstance(iface, List):
            continue
        iface_str = repr(iface)
        if field_pat and not any(field_pat in k for k in iface._keys):
            continue
        iname = type(iface).__name__
        if (not iface_name or iname == iface_name) and pat in iface_str:
            print(iface_str, file=stream)
            _print_iface(iface, field_pat, stream)
