#!/usr/bin/env python

# © 2019 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

import random
import time

from progress.bar import (Bar, ChargingBar, FillingSquaresBar,
                          FillingCirclesBar, IncrementalBar, PixelBar,
                          ShadyBar)
from progress.spinner import (Spinner, PieSpinner, MoonSpinner, LineSpinner,
                              PixelSpinner)
from progress.counter import Counter, Countdown, Stack, Pie


def sleep():
    t = 0.01
    t += t * random.uniform(-0.1, 0.1)  # Add some variance
    time.sleep(t)


for bar_cls in (Bar, ChargingBar, FillingSquaresBar, FillingCirclesBar):
    suffix = '%(index)d/%(max)d [%(elapsed)d / %(eta)d / %(eta_td)s]'
    bar = bar_cls(bar_cls.__name__, suffix=suffix)
    for _ in bar.iter(range(200)):
        sleep()

for bar_cls in (IncrementalBar, PixelBar, ShadyBar):
    suffix = '%(percent)d%% [%(elapsed_td)s / %(eta)d / %(eta_td)s]'
    with bar_cls(bar_cls.__name__, suffix=suffix, max=200) as bar:
        for _ in range(200):
            bar.next()
            sleep()

for spin in (Spinner, PieSpinner, MoonSpinner, LineSpinner, PixelSpinner):
    for _ in spin(spin.__name__ + ' ').iter(range(100)):
        sleep()

for singleton in (Counter, Countdown, Stack, Pie):
    for _ in singleton(singleton.__name__ + ' ').iter(range(100)):
        sleep()

bar = IncrementalBar('Random', suffix='%(index)d')
for i in range(100):
    bar.goto(random.randint(0, 100))
    sleep()
bar.finish()
