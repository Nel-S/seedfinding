# Forward Checker
This is a Minecraft seedfinding program for finding spawns within a given radius of a mushroom island. It is designed for Java 1.18+, and is not compatible with Bedrock Edition or with Java 1.17-.

See [the repository's parent README](../../README.md) for compilation instructions.

## Using
This program first emulates the first two stages of the spawn algorithm, ensuring at all moments that the spawnpoint's effective continentalness never rises above `MOST_POSITIVE_CONT`. Should that pass, the program then checks all biomes within a 2*`RADIUS`-block bounding box to ensure only ocean biomes, and at least one mushroom fields biome, are present.

## Acknowledgements
This program relies on my Utilities library, which in turn relies on the [Cubiomes](https://github.com/Cubitect/cubiomes) library; that was created by [Cubitect](https://github.com/Cubitect) and released under the MIT License.