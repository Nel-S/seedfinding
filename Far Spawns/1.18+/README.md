# 1.18+

In 1.18, Minecraft: Java Edition switched to using a fitness-based approach for its spawn algorithm: the game attempts to find a coordinate that balances being near (0,0), not having climate values beyond +/-1, and not having climate values liable to induce ocean or river generation. A fuller explanation of the algorithm is given here: https://nel-s.github.io/efc-calc/

The farthest theoretically-possible spawnpoint in this version is (1599, -2160) at 2687.45 blocks away from (0,0).<br>
The farthest spawnpoint found thus far under normal biomes (L2/Euclidean distance) is (-1343, -1136) at 1759.02 blocks, with the seed 2242180942571.<br>
The farthest spawnpoint found thus far along a single axis under normal biomes (L∞/Chebyshev distance) is x=1673, with the seed 2939217269032.<br>
The farthest spawnpoint found thus far under Large Biomes is (2363, -356) at 2389.67 blocks, with the seed 6015757257 found by Derm. (This is certainly improvable.)

All seeds between 0 and 3366025373172 have been checked.

While the Forward Checker is usable for "small" ranges of seeds, the only true way to find the farthest spawnpoint in the entire seedspace would be by deriving worldseeds from their Perlin octaves. With regards to that, the maximum/minimum possible Perlin sample has been found to be &pm;1.0363538112118025.

## Contributors
- Cubitect - implemented (port of) the spawn algorithm in [Cubiomes](https://github.com/Cubitect/cubiomes)
- NelS - created programs; checked 1.4 trillion seeds
- Jereaux - improved Cubiomes Viewer session; checked 19 billion seeds
- Meox - checked 58 billion seeds
- Matt - set up a microBOINC project for the Forward Checker
    - Phillip, BoySanic, Derm, Meox, SundayMC, acebase, MPM - collectively checked 1.89 trillion seeds under the project
- Derm - checked 6.2 billion Large Biomes seeds