# XIMEA-xiB-64-2-milk

Program to read out the xiB-64 PCIe cameras by XIMEA. The frames are written into the shared memory structure of milk to be available in cacao.

cacao-repository:
https://github.com/cacao-org/cacao

milk-repository:
https://github.com/milk-org/milk

## Tested Models:
| Model Name | xiAPI Version | Remarks |
| ------ | ------ | ------ |
| CB013MG-LX-X8G3 | V4.27.07.00 | xiAPI initialization prints errors on reading routines of properties which are not available on this specific camera model. Can be ignored. |

## Build and Execution Instructions:
**Requirements:**
- Linux OS - tested on Ubuntu 20.04.6 LTS
- g++ installed - c++ compiler
- xiAPI installed -
 [software package by XIMEA](https://www.ximea.com/support/wiki/apis/xiAPI), includes the API of the camera.

| Terminal Command | Description |
| ------ | ------ |
| `make` | Builds the software. Change the "AUTORUN_AFTER_BUILD" value in the makefile to true if the built program shall be executed automatically. |
| `make clean` | Deletes the build directory. |
| `make run` | Builds and executes the software after it has already been built. Shortcut for `./build/xiB-64-2-milk` |

After building, export the build directory to the environmental variables:
``export XIMEA_TO_MILK_PATH=<path-to-repository>/build/``
