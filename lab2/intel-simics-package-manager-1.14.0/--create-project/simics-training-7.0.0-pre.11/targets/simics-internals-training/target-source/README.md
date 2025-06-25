# How to build RISC-V BSP based applications

## Prerequisites

You need `cmake` and you need the riscv toolchain from [here](https://github.com/riscv-collab/riscv-gnu-toolchain/releases). You can find the precompiled tool chains under "Assets" in a given release.
This was originally developed and tested with riscv64-elf-ubuntu-20.04-gcc-nightly-2024.03.01-nightly.tar.gz.
We assume you have this toolchain in `/path/to/riscv-gnu-toolchain`.

## Build

To build all applications configure your build as shown below.

```
cmake -S /path/to/mydev/trainings/targets/simics-internals-training/target-source/risc-v -B build-riscv -GNinja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=/path/to/riscv-gnu-toolchain/bin/riscv64-unknown-elf-gcc -DCMAKE_CXX_COMPILER=/path/to/riscv-gnu-toolchain/bin/riscv64-unknown-elf-g++ -DRISCV_IMAGES_OUTPUT_DIRECTORY=/path/to/output/directory
```

Then build via `cmake --build build-riscv`. 
Afterwards, you can find all executables in `/path/to/output/directory`. If you did not set `RISCV_IMAGES_OUTPUT_DIRECTORY` the images will be placed in `/path/to/mydev/trainings/targets/simics-internals-training/images` by default.

## Notes when creating a new application

1. Ensure you have `#include "risc-v-simple-bsp.h"`.
2. In your main function, the first thing should be a call to `init_board();`.
3. Instead of `printf` use `bsp_printf`.
4. Do not use `malloc` or `free`.
5. C++ will almost certainly not work, as we do not have dynamic memory in the BSP.
6. In your `CMakeLists.txt` file, add `add_subdirectory(/path/to/risc-v-simple-bsp risc-v-simple-bsp)`.
7. You need to have `target_link_libraries(your_target PRIVATE risc-v-simple-bsp c)`, where `your_target` needs to be replaced with whatever target you are defining.

