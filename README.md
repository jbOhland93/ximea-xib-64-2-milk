# XIMEA-xiB-64-2-milk

Programm to read out the xiB-64 PCIe cameras by XIMEA. The frames are written into the shared memory structure of milk to be available in cacao.

cacao-repository:
https://github.com/cacao-org/cacao

milk-repository:
https://github.com/milk-org/milk

## Build and execution instructions:
**Requirements:**
- Linux OS
- g++ installed - c++ compiler
- xiAPI installed -
 [software package by XIMEA](https://www.ximea.com/support/wiki/apis/xiAPI), includes the API of the camera.

| terminal command | Description |
| ------ | ------ |
| `make` | Builds the software. Change the "AUTORUN_AFTER_BUILD" value in the makefile to true if the built programm shall be executed automatically. |
| `make clean` | Deletes the build directory. |
| `make run` | Builds and executes the software after it has already been built. Shortcut for `./build/xiB-64-2-milk` |
