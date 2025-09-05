# Simics-Agent-UEFI

The simics-agent-uefi repository contains a port of the simics-agent to a UEFI environment along with instructions on how to download, compile and run the binary.
This content was exported from the tianocore training that is described at https://gitpitch.com/tianocore-training/Platform_Build_Linux_Ovmf_Lab/master#/1

## Current functionalities supported

*Please check the **Run commands using matic objects** section below for how to use these commands*

* matic0.upload 
* matic0.upload-dir
* matic0.download
* matic0.download-dir
* matic0.change-directory
* matic0.list-files
* matic0.print-file
* matic0.print-working-directory

*To run a custom binary from Simics without closing the agent
* matic0.run <path_to_efi_file_on_target> 

*Please check the **Run commands available from UEFI Shell** section below for how to use these commands*

* SimicsAgent.efi --download
* SimicsAgent.efi --download-dir 
* SimicsAgent.efi --upload
* SimicsAgent.efi --upload-dir 
* SimicsAgent.efi --change-res <GOP MODE>

## How to build and compile the simics-agent for UEFI on Ubuntu OS

Pre-requisites Ubuntu
```
bash$ sudo apt-get install build-essential subversion uuid-dev iasl
```

Create the folder structure for your workspace
```
bash$ mkdir ~/workspace && cd ~/workspace
bash$ git clone https://github.com/tianocore/edk2
bash$ git clone https://github.com/tianocore/edk2-libc
bash$ cd ~/workspace/edk2
bash$ git submodule update --init
bash$ make -C BaseTools
bash$ . edksetup.sh
```

Export workspace and platform path (see script file setenv.sh)
```
export EDK_TOOLS_PATH=~/workspace/edk2/BaseTools
export PACKAGES_PATH=~/workspace/edk2:~/workspace/edk2-libc
```

Edit Conf\target.txt to change the platform, architecture and toolchain for the simics-agent (tested for both OvmfPkg and IntelFsp2Pkg)
```
bash$ cd ~/workspace/edk2
bash$ vim Conf/target.txt
ACTIVE_PLATFORM       = MdeModulePkg/MdeModulePkg.dsc
TOOL_CHAIN_TAG        = GCC5
TARGET_ARCH           = X64 # X64, IA32 X64 (which will build both architectures).
```

Link to the Simics-Base sources (installed at <simics>) and run the build
```
bash$ cd ~/workspace/edk2-libc/
bash$ ln -s <simics>/src/misc/simics-agent AppPkg/Applications/simics-agent-uefi
bash$ sudo vim AppPkg/AppPkg.dsc
```

Add the following line under the Components section
	[Components]
	AppPkg/Applications/simics-agent-uefi/simics-agent.inf

```	
bash$ build -p AppPkg/AppPkg.dsc -m AppPkg/Applications/simics-agent-uefi/simics-agent.inf
```
* You can find the binary at ~/workspace/edk2/Build/AppPkg/DEBUG_GCC5/X64/edk2-libc/AppPkg/Applications/simics-agent-uefi/simics-agent/OUTPUT/SimicsAgent.efi

* You can change the binary name in the simics-agent.inf by editing the BASE_NAME value
```
bash$ sudo vim ~/workspace/edk2-libc/AppPkg/Applications/simics-agent-uefi/simics-agent.inf
[Defines]
INF_VERSION = 1.25
BASE_NAME = SimicsAgent
```

# Run the binary in Simics UEFI Shell
* Copy the SimicsAgent.efi to the disk/usb image used in the Simics script

* Run Simics simulation and press F2 to enter BIOS. Navigate to EFI Shell, check your mounted disk/usb for the SimicsAgent.efi and run it.
E.g. `FS0:\SimicsAgent.efi <Enter>'

** For instructions on how to connect to the simics-agent from the host, please check the Simics documentation

# Run commands using matic objects.

*The current implementation requires the user to use a mix between DOS and Unix separators for paths
```
simics>matic0.download FS0:/filepath.ext
simics>matic0.download-dir FS0:/directory
simics>matic0.upload file_to_upload FS0:
simics>matic0.upload file_to_upload FS0:/directory
simics>matic0.upload-dir directory_to_upload FS0:
simics>matic0.upload-dir directory_to_upload FS0:/directory
simics>matic0.change-working-directory /FS0:/
/FS0:
simics>matic0.change-working-directory directory1 #names without leading / will be concatenated to the current working directory
/FS0:/directory1
simics>matic0.list-files #lists files from the current directory
simics>matic0.list-files /FS0:/directory/ #lists files from the given path
simics>matic0.print-file /FS0:/filepath.ext 
simics>matic0.print-working-directory
```

# Run commands available from UEFI Shell

*The upload/download commands for files/directories are also available from the UEFI Shell
```
SimicsAgent.efi --download <file_path_on_host>
SimicsAgent.efi --download-dir <directory_path_on_host>
SimicsAgent.efi --upload <file_path_on_target> C:\Users\John\Documents\
SimicsAgent.efi --upload-dir <directory_path_on_target> #will upload to Simics home directory
SimicsAgent.efi --upload-dir <directory_path_on_target> C:\Users\John\Documents\ #will upload to specified path on host machine
```

## [Optional] Create a FAT32 image (or a different format, depending on your host)
1. Create a file filled with zeros
`bash$ dd if=/dev/zero of=test.img count=50 bs=1M`

2. Create the partition (and partition table)
```bash$ fdisk test.img
Command (m for help): o
Building a new DOS disklabel with disk identifier <identifier>.
Command (m for help): n
Partition type:
  p primary (0 primary, 0 extended, 4 free)
  e extended
Select (default p): <Enter>
Using default response p
Partition number (1-4, default 1): <Enter>
First sector (2048-99999, default 2048):
Using default value 2048
Last sector, +sectors or +size{K,M,G} (2048-99999, default 99999): <Enter>
Using default value 99999
Partition 1 of type Linux and of size 47.8 MiB is set
Command (m for help): t
Selected partition 1
Hex code (type L to list all codes): c
Changed type of partition 'Linux' to 'W95 FAT32 (LBA)'
Command (m for help): w
The partition table has been altered!
Syncing disks
```

3.	Create the FAT file system in the image 
`bash$ mkfs.vfat test.img`

4. Create a directory for mounting
`bash$ sudo mkdir test`

5. Mount the image 
`$ sudo mount test.img test`

* Now you can copy/delete files in the test directory which will be written into the image file. 

6. After file operations unmount the image and delete the directory
`bash$ sudo umount test && sudo rmdir test`

7. [Optional] Command to convert img to Craff
Go to your Simics workspace. Type below command:
`bash$bin/.craff -o sample.craff test.img`
OR for Windows
`SimicsInstallation\win64\bin\craff.exe -o sample.craff test.img`

8. Use the new image in your Simics script
`$disk0_image = "path_to_your_new_image"`
