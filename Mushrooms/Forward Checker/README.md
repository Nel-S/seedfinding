# Forward Checker
This is a Minecraft seedfinding program for finding spawns within a given radius of a mushroom island. It is designed for Java 1.18+; since modern-day Bedrock also uses climates for its biomes I _suspect_ the same will work for Bedrock, but I have not thoroughly looked into that yet and could very well be wrong. (This is not compatible with Java 1.17-.)

## Installing
This program can be used by downloading the folder, changing the values in [settings.h](C/settings.h) to set one's criteria, and then compiling the files with [GCC](https://gcc.gnu.org/) or another compiler of choice. The Cubiomes library will also need to be linked.

## Using
This program first emulates the first two stages of the spawn algorithm, ensuring at all moments that the spawnpoint's effective continentalness never rises above `MOST_POSITIVE_CONT`. Should that pass, the program then checks all biomes within a 2*`RADIUS`-block bounding box to ensure only ocean biomes, and at least one mushroom fields biome, are present.

This program supports the usage of one's own custom `main()` function. Any such `main` function, to work, must
- call `initConstants()` before beginning the search,
- create for each thread/process/etc. a struct of type `struct Data` whose sole attribute, `index`, is set to that thread/process/etc.'s index;
- call `checkSeeds()` while passing the memory address of the `struct Data` struct as a parameter.
A custom implementation must also implement the output method `outputValues(uint64_t seed, int dist)` (which receives the seed and minimum Chebyshev distance to the mushroom fields biome for each result). Otherwise, one may setup their main function as they wish; the settings `FILEPATH`, `NUMBER_OF_THREADS`, and `TIME_PROGRAM` are provided but may be ignored, and the variables `localStartSeed`, `localSeedsToCheck`, and `localNumberOfProcesses` (which are called by each thread/process/etc.) are provided but may be changed if one wishes.<br>
Example `main` implementations are provided above.

## Acknowledgements
This program relies heavily on the [Cubiomes](https://github.com/Cubitect/cubiomes) library, which was created by [Cubitect](https://github.com/Cubitect) and released under the MIT License.