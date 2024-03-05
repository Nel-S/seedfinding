# Seedfinding
This repository holds almost all of my seedfinding-related files and code.

Most of the C programs will need to link the [Cubiomes](https://github.com/Cubitect/cubiomes) library when being compiled, which can be done by also including a reference to `libcubiomes.a` (a static compilation of the library). If `libcubiomes.a` in turn needs to be updated, that can be done by installing [CMake](https://cmake.org/), entering the `cubiomes` submodule in a terminal, and running `make cubiomes`.

These files are released under the MIT License.
If you use or cite any of these files, I would greatly appreciate a citation.