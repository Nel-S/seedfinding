# Seedfinding
This repository holds almost all of my seedfinding-related files and code.

## C/C++ Compiling
This repository is loosely split into three parts.
- The programs themselves.
- A [set of C/C++ `main()` templates](./Templates) to link the programs to.
- A [Utilities library](./Utilities) that will in many cases need to be linked as well. (This will eventually be replaced with a makefile.)
Furthermore, most of the C programs will need to link the [Cubiomes](https://github.com/Cubitect/cubiomes) library when being compiled, which can be done by including a reference to `libcubiomes.a` (a static compilation of the library). If `libcubiomes.a` in turn needs to be updated, that can be done by installing [CMake](https://cmake.org/), entering the `cubiomes` submodule in a terminal, and running `make libcubiomes` on Linux/`mingw32-make.exe libcubiomes` on [MinGW](https://www.mingw-w64.org/).

All #include paths in this repository are also standardized in relation to the parent directory `seedfinding/`: as such, when compiling the C programs one needs to specify that directory should be examined as well via `-I"<path to seedfinding/>"`.

As an example, compiling [`Extreme Climates/extremeClimateAtCoord.c`](./Extreme%20Climates/extremeClimateAtCoord.c) with pthreads would be done with
```bash
gcc "Extreme Climates/extremeClimateAtCoord.c" "Templates/Example main (pthreads).c" "Utilities/Climates.c" "Utilities/Spawn.c" "libcubiomes.a" -I"<path to seedfinding/>" -o <executable name>
```
with optional compilation flags (`-O3`, `-fwrapv`, `-Wall`, etc.) listed afterwards.

## On Seedfinding
This repository is also the place to raise issues relating to my reference guide "[On Seedfinding](https://docs.google.com/document/d/1V1wpuHcewQM3loxzVW0LQqASc17LkxeN6qugebkHkJ8)", or to suggest (evidence-based) additions, deletions, or changes to it. (Please ensure any changes you suggest abide by the guide's Legal page.)