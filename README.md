# Seedfinding
This repository holds almost all of my seedfinding-related files and code.

## C/C++ Compiling
Most of the C/C++ files in this repository can be compiled by
1. Installing [CMake](https://cmake.org/download) and a Makefile generator of your choosing (such as [GNU Make](https://www.gnu.org/software/make/#download) for Linux or [MinGW](https://www.mingw-w64.org/downloads) for Windows).
2. Opening a terminal of your choice, and running
```bash
cmake -G "[Makefile generator to use]" -Dprog="[path/to/program/to/run]" -Dbackend="[Backend to use: Basic, Pthreads, or MPI]" .
```
3. After CMake finishes, running your makefile generator (e.g. `make` if using GNU Make or `mingw32-make.exe` if using MinGW).
4. Opening the `\bin` folder that will be created, and running the `seedfinding`/`seedfinding.exe` executable inside.

(A build script is planned to ultimately automate this process.)