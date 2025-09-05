# © 2025 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

from setuptools import setup
from setuptools.command.install import install
from setuptools.command.build import build as _build
from setuptools.command.egg_info import egg_info as _egg_info
from pathlib import Path
import json
import tempfile

class GenerateSimicsBasePath(install):
    def run(self):
        simics_base = Path(__file__).absolute().parent

        # Path to simics modules inside site-packages
        for p in ("simicsutils", ):
            package_dir = Path(self.install_lib) / p
            package_dir.mkdir(exist_ok=True, parents=True)

            # Write config file importable from simics __init__.py
            (package_dir / "config.json").write_text(
                json.dumps({'simics_base': str(simics_base)}),
                encoding="utf-8")
        install.run(self)

# Allow the user to set the wheel build directory during pip install
class CustomBuildDir(_build):
    def initialize_options(self):
        super().initialize_options()
        self.build_base = tempfile.mkdtemp(suffix="pip-simics-build")

# Allow the user to set the wheel build directory during pip install
class CustomEggInfo(_egg_info):
    def initialize_options(self):
        super().initialize_options()
        self.egg_base = tempfile.mkdtemp(suffix="pip-simics-egg")

setup(cmdclass={
    'install': GenerateSimicsBasePath,
    'build': CustomBuildDir,
    'egg_info': CustomEggInfo,
})
