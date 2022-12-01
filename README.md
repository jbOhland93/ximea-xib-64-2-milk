# XIMEA-xiB-64-2-milk

Programm to read out the xiB-64 PCIe cameras by XIMEA. The frames are written directly into the memory of an NVIDIA GPU and packed into the shared memory structure of milk to be available in cacao.

cacao-repository:
https://github.com/cacao-org/cacao

milk-repository:
https://github.com/milk-org/milk

## Build and execution instructions:
make        # Builds the software and executes it automatically. Change "AUTORUN_AFTER_BUILD" in makefile to false to avoid this.
make run    # Executes the software after it has already been built. Equivalent to ./build/xiB-64-2-milk