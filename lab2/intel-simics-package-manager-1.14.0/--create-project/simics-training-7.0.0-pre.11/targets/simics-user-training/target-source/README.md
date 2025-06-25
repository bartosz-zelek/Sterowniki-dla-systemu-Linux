# target-source directory
For the QSP with ClearLinux!

This directory holds the target source code and built binaries for the target code.   It is used to build the binaries on the target, using the ClearLinux target build setup.  

To build the programs (assuming Clear Linux b28910):
* Start Simics using the script ```targets/new-user-training/z300-boot-build-setup.simics```
* For each program, build using the script ```targets/new-user-training/z301-build-sw-on-target.include```

To invoke the z301 script, from the Simics command-line... replacing ```demo-one``` with the name of each program in turn:

```simics> run-script "%simics%\targets\simics-user-training\z301-build-sw-on-target.include" matic = matic0 host_dir = "%simics%/targets/simics-user-training/target-source/demo-one/" target_build_area="/root/swbuild/"```

To make this work, the assumption is that each program contains a makefile that can be used to build it. 
