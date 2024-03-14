# Seedfinding
This repository holds almost all of my seedfinding-related files and code.

Most of the C programs will need to link the [Cubiomes](https://github.com/Cubitect/cubiomes) library when being compiled, which can be done by also including a reference to `libcubiomes.a` (a static compilation of the library). If `libcubiomes.a` in turn needs to be updated, that can be done by installing [CMake](https://cmake.org/), entering the `cubiomes` submodule in a terminal, and running `make libcubiomes` on Linux/`cmake.exe libcubiomes` on Windows.

This repository is also the place to raise issues relating to my "[On Seedfinding](https://docs.google.com/document/d/1V1wpuHcewQM3loxzVW0LQqASc17LkxeN6qugebkHkJ8)" reference guide, or to suggest (evidence-based) additions, deletions, or changes to it. (Please ensure any changes you suggest abide by the guide's Legal page.)

All of these files are released under the MIT License.
If you use or reference anything here, I would greatly appreciate a citation.