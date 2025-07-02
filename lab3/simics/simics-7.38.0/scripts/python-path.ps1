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

$script_path = split-path -parent $MyInvocation.MyCommand.Definition
$base = "$script_path\.."
$host_python = $false

# Make sure Simics modules can be found.
if ($env:MINIPYTHONPATH) {
       $env:MINIPYTHONPATH = "$base\win64;$base\win64\bin\py3" + "$env:MINIPYTHONPATH"
} else {
       $env:MINIPYTHONPATH = "$base\win64;$base\win64\bin\py3"
}

if ($env:SIMICS_PYTHON_PACKAGE -and (-not $env:SIMICS_PYTHON)) {
       $python_pkg = $env:SIMICS_PYTHON_PACKAGE
       $mini_python = "`"$python_pkg\win64\bin\py3\mini-python.exe`""
} elseif ((-not $env:SIMICS_PYTHON) -and (Test-Path "$base\win64\bin\py3\mini-python.exe" -PathType Leaf)) {
       $python_pkg = $base
       $mini_python = "`"$python_pkg\win64\bin\py3\mini-python.exe`""
} else {
       # Running directly in the package.

       # Heuristic: Python package installed next to Base package.
       # Any Python package version should work.
       if (Test-Path "$base\..\simics-python-7*\win64\bin\py3\mini-python.exe" -PathType Leaf) {
             $p = Resolve-Path "$base\..\simics-python-*\win64\bin\py3\mini-python.exe" | Select-Object -Last 1
             $d = Split-Path -parent $p.Path
             $mini_python = $p.Path
             $python_pkg = "$d\..\.."
       } elseif (Test-Path "$base\..\..\1033\*\win64\bin\mini-python.exe" -PathType Leaf) {
             $p = Resolve-Path "$base\..\..\1033\*\win64\bin\mini-python.exe" | Select-Object -Last 1
             $d = Split-Path -parent $p.Path
             $mini_python = $p.Path
             $python_pkg = "$d\..\.."
       } else {
             if (Test-Path "$base\.package-list" -PathType Leaf) {
                $pkg_list = "$base\.package-list"
             } else {
                $pkg_list = "$base\..\..\.package-list"
             }
             $keys = @()
             if (Test-Path HKLM:\SOFTWARE\Python\PythonCore) {
                $keys += Get-ChildItem HKLM:\SOFTWARE\Python\PythonCore
             }
             if (Test-Path HKCU:\SOFTWARE\Python\PythonCore) {
                $keys += Get-ChildItem HKCU:\SOFTWARE\Python\PythonCore
             }
             if ($env:SIMICS_PYTHON) {
                $python = "$env:SIMICS_PYTHON"
             } elseif ($env:SIMICS_BUILD_DEPS_ROOT) {
                # The Base CI needs to run from build-deps
                $python = "$env:SIMICS_BUILD_DEPS_ROOT\python\python.exe"
             } elseif ($keys) {
                $python_ver = $keys | Sort-Object -Property {[int](((($_.Name -split "\.")[0]) -split "\\")[4])}, {[int](($_.Name -split "\.")[1])} | Select-Object -Last 1
                $p = Get-ItemProperty Registry::$python_ver\InstallPath
                $python = $p.ExecutablePath
             } else {
                $python = ""
             }
             if ($python) {
                    $p = [System.Diagnostics.Process] @{
                        StartInfo = [System.Diagnostics.ProcessStartInfo] @{
                            FileName = $python
                            RedirectStandardError = $true
                            RedirectStandardOutput = $true
                            UseShellExecute = $false
                            Arguments = "-c `"import sys; print(sys.hexversion)`""
                            CreateNoWindow = $true
                        }
                    }
                    $p.Start() | Out-Null
                    $stdout = $p.StandardOutput.ReadToEnd()
                    $p.WaitForExit()
                    $version = $stdout.Trim()
                    # We require at least Python 3.10
                    if ([int]$version -lt 0x030a04f0) {
                        $python = ""
                    }
             }
             
             if ($python -and (-not $env:SIMICS_PYTHON) -and (Test-Path $pkg_list -PathType Leaf)) {
                    $info = [System.Diagnostics.ProcessStartInfo] @{
                        FileName = $python
                        RedirectStandardError = $true
                        RedirectStandardOutput = $true
                        UseShellExecute = $false
                        Arguments = "`"$base\scripts\lookup_file.py`" -f win64/bin/py3/mini-python.exe -p `"$pkg_list`""
                        CreateNoWindow = $true
                    }
                    # Workaround for bug in old Powershell
                    # EnvironmentVariables is null until is has been accessed
                    echo $info | out-null
                    echo $info.EnvironmentVariables | out-null
                    $info.EnvironmentVariables["PYTHONPATH"] = $env:MINIPYTHONPATH
                    $p = [System.Diagnostics.Process] @{
                        StartInfo = $info
                    }
                    $p.Start() | Out-Null
                    $stdout = $p.StandardOutput.ReadToEnd()
                    $p.WaitForExit()
                    $mini_python = $stdout.Trim()
                    [Console]::Error.WriteLine($p.StandardError.ReadToEnd())
             }
             if (-not $mini_python) {
                if (-not $env:SIMICS_PYTHON) {
                    [Console]::Error.WriteLine("Warning: mini-python not found.")
                    [Console]::Error.WriteLine("Install Simics package 1033 to obtain a Python.")
                }
                if ($python) {
                    if (-not $env:SIMICS_PYTHON) {
                       [Console]::Error.WriteLine("Falling back to host Python.")
                       [Console]::Error.WriteLine("Warning: you must set the SIMICS_PYTHON environment variable to the host Python path before running Simics and make sure it has required Python packages installed.")
                       [Console]::Error.WriteLine("For further information, see section 2.3.5 in the Simics User Guide.")
                    }
                    $mini_python = $python
                    $host_python = $true
                } else {
                    [Console]::Error.WriteLine("*** Error: no host Python 3.10 or greater found.")
                }
             }
      }
}

$env:_MINI_PYTHON = $mini_python

if ($host_python) {
    # Make sure Simics modules can be found.
    if ($env:PYTHONPATH) {
        $env:PYTHONPATH = "$base\win64;$base\win64\bin\py3" + "$env:PYTHONPATH"
    } else {
        $env:PYTHONPATH = "$base\win64;$base\win64\bin\py3"
    } 
}
