# Forward Checker
This is a Minecraft seedfinding program for finding spawns beyond a given distance from the world origin. It is designed for Java 1.18+; since modern-day Bedrock also uses climates for its biomes I _suspect_ the same will work for Bedrock, but I have not thoroughly looked into that yet and could very well be wrong. (This is not compatible with Java 1.17-.)

## Installing
<!-- At present two versions of this program exist after downloading this repository:
- a [static version](Static%20Code) consisting of four `.c`/`.h` files. By changing the values in [settings.h](Static%20Code/settings.h), and then compiling with [GCC](https://gcc.gnu.org/) or another compiler of choice, this can take advantage of the optimizations that using strictly constant values in the program allows for.
- a [precompiled dynamic version](spawnFinder.exe) which can have its values and flags specified with command-line arguments in one's terminal of choice. This does not require a compiler, but will generally run slower than the compiled static version. (The [source code for that precompiled version](Precompiled%20Executable%20Source%20Code.zip) is also provided for those wish to compile it themselves.) -->
This program can be used by downloading the folder, changing the values in [settings.h](C/settings.h) to match one's criteria, and then compiling the files with [GCC](https://gcc.gnu.org/) or another compiler of choice. The Cubiomes library will also need to be linked.

## Using
The program support two search modes:
- one based on underlying first-round spawn rings (which broadly determine the rough spawn location and distance from the origin to within ~512 blocks on average); and
- one using specified distances from the origin (that models the second-round spawn ring calculations to narrow the spawnpoint down to at most 88sqrt(2) blocks, ensuring more precise results at the cost of speed). This search mode can also search based on distances from the origin alone a single axis.
The precompiled version also supports a mode that monotonically increases the radius based on each best result found. This cannot be added to the static version due to it treating the origin distance thresholds as constants and optimizing using that.

It should be emphasized that with the second search mode type, **this program does not calculate the exact spawnpoint of worlds**: doing that would require a block-by-block recreation of that part of the world. Instead, this program calculates the approximate spawnpoint after the second round of calculations, which may be up to 88sqrt(2) blocks off at most (or 88 blocks off at most in a single direction). The benefit of this approach is that
- the programs runs far quicker, and
- the programs are also version-independent, in that terrain differences between 1.18+ versions (or I suspect between Java and Bedrock, though again I have not thoroughly looked into that yet) will not affect the program's results.
The caveat, however, is that one must treat every world returning a distance within 88sqrt(2) blocks of the farthest distance found (or 88 blocks of the farthest distance along one axis if one uses that metric) as a potential candidate. This will still usually eliminate all but a handful of worlds, however, and on my machine I have found <!-- the static version of --> this program to run on average 518-692x faster than an equivalent default spawn search using [Cubiomes Viewer](https://github.com/Cubitect/cubiomes-viewer).

## Acknowledgements
This program relies heavily on the [Cubiomes](https://github.com/Cubitect/cubiomes) library, which was created by [Cubitect](https://github.com/Cubitect) and released under the MIT License.