# XIMEA-xiB-64-2-milk

Programm to read out the xiB-64 PCIe cameras by XIMEA. The frames are written directly into the memory of an NVIDIA GPU and packed into the shared memory structure of milk to be available in cacao.

cacao-repository:
https://github.com/cacao-org/cacao

milk-repository:
https://github.com/milk-org/milk

## Build and execution instructions:
**Requirements:**
- Linux OS
- gcc installed - c compiler
- g++ installed - c++ compiler
- nvcc installed - CUDA compiler by NVIDIA, uses g++ and gcc
- xiAPI installed -
 [software package by XIMEA](https://www.ximea.com/support/wiki/apis/xiAPI), includes the API of the camera.

| terminal command | Description |
| ------ | ------ |
| `make` | Builds the software and executes it automatically. Change the "AUTORUN_AFTER_BUILD" value in the makefile to false to avoid this. |
| `make clean` | Deletes the build directory. |
| `make run` | Executes the software after it has already been built. Shortcut for `./build/xiB-64-2-milk` |
