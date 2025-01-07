# Seedfinding
This repository holds almost all of my seedfinding-related files and code.
<!-- 
## C/C++ Compiling
This repository is loosely split into three parts.
- The programs themselves.
- A [Utilities library](./Utilities) that will in many cases need to be linked as well. (This will eventually be replaced with a makefile.) Furthermore, most of the C programs using the Utilities library will need to link the [Cubiomes](https://github.com/Cubitect/cubiomes) library when being compiled, which can be done by including a reference to `libcubiomes.a` (a static compilation of the library). If `libcubiomes.a` in turn needs to be updated, that can be done by installing [CMake](https://cmake.org/), entering the `cubiomes` submodule in a terminal, and running `make libcubiomes` on Linux/`mingw32-make.exe libcubiomes` on [MinGW](https://www.mingw-w64.org/).

As an example, compiling [`Extreme Climates/Extreme Climate At Coordinate.c`](./Extreme%20Climates/Extreme%20Climate%20At%20Coordinate.c) with pthreads would be done with
```bash
gcc "Extreme Climates/Extreme Climate At Coordinate.c" "core/Example main (pthreads).c" "Utilities/U_Math.c" "Utilities/Climates.c" "Utilities/Spawn.c" "libcubiomes.a" -o "<executable name>"
```
with optional compilation flags (`-O3`, `-fwrapv`, `-Wall`, etc.) listed afterwards. -->