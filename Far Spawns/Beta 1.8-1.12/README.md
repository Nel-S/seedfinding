# Beta 1.8 - 1.12

Between Beta 1.8 and 1.12, Minecraft: Java Edition searched for a valid biome in the area ((-256, -256), (256, 256)). In Beta 1.8 and 1.0, this comprised of forests, swamps, or taigas; after that this comprised of forests, plains, taigas, taiga hills, wooded hills, jungles, or jungle_hills Then the game looks for a grass block, jumping between -63 and 63 blocks in the x- and z-directions up to 1000 times in its attempts to find one.

- The farthest theoretically-possible spawnpoint in this version is not yet known, but is between (-4, -6357) at 6357.00 blocks, and (63000, 63000) at 89095.45 blocks (surely a gross overestimation).
- The farthest spawnpoint for 1.6 found thus far under normal biomes (L2/Euclidean distance) is (4087, -4413) at 6014.83 blocks, with the seed 3203190408255848708.
- The farthest spawnpoint for 1.12 found thus far under normal biomes (L2/Euclidean distance) is (-2621, 2812) at 3844.08 blocks, with the seed -3539678881786985216.

Due to terrain and algorithm differences, Beta 1.8, 1.0, 1.1-1.6, and 1.7-1.12 will likely have different farthest spawnpoints. Minor versions may also differ.

## Contributors
- Cubitect - implemented (port of) the spawn algorithm in [Cubiomes](https://github.com/Cubitect/cubiomes)
- NelS - created C/CUDA programs
- Andrew - determined at most 16647 state advancements can occur across consecutive nextInt(2) ... nextInt(16641) calls