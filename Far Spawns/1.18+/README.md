# 1.18+

In 1.18, Minecraft: Java Edition and Minecraft: Bedrock Edition switched to using a fitness-based approach for their spawn algorithms: the games attempt to find a coordinate that balances being near (0,0), not having climate values beyond +/-1, and not having climate values liable to induce ocean or river generation. A fuller explanation of the algorithm is given underneath [this tool](https://nel-s.github.io/efc-calc/).

| Criteria                             | **Theoretical** Farthest Spawnpoint from (0, 0) | Distance (blocks) |
| ------------------------------------ | ----------------------------------------------- | ----------------- |
| Java 1.18+, L2/Euclidean distance    | (1599, -2160)                                   | 2687.45           |
| Java 1.18+, L∞/Chebyshev distance    | x = 2647                                        | 2647              |
| Bedrock 1.18+, L2/Euclidean distance | (0, 2560)                                       | 2560              |
| Bedrock 1.18+, L∞/Chebyshev distance | x = 2560                                        | 2560              |

| Criteria                                           | Current Farthest Spawnpoint from (0, 0) | Distance (blocks) | Seed                | Discoverer |
| -------------------------------------------------- | --------------------------------------- | ----------------- | ------------------- | ---------- |
| Java 1.18+ normal biomes, L2/Euclidean distance    | (-1343, -1136)                          | 1759.02           | 2242180942571       | NelS       |
| Java 1.18+ normal biomes, L∞/Chebyshev distance    | x = -1728                               | 1728              | 690455132394644     | Meox       |
| Java 1.18+ Large Biomes, L2/Euclidean distance     | (2363, -356)                            | 2389.67           | 6015757257          | Derm       |
| Java 1.18+ Large Biomes, L∞/Chebyshev distance     | x = 2363                                | 2363              | 6015757257          | Derm       |
| Bedrock 1.18+ normal biomes, L2/Euclidean distance | (1515, 829)                             | 1726.98           | 8514982786202962122 | Jereaux    |
| Bedrock 1.18+ normal biomes, L∞/Chebyshev distance | x = -1714                               | 1714              | 690455132394644     | Meox       |

For Java normal biome spawns, all seeds between 0 and 3426926457792 have been checked.<br>
For Java Large Biome spawns, all seeds between 0 and 160265862227 have been checked.

While the Forward Checker is usable for "small" ranges of seeds, the only true way to find the farthest spawnpoint in the entire seedspace would be by deriving worldseeds from their Perlin octaves. With regards to that, the maximum/minimum possible Perlin sample has been found to be &pm;1.0363538112118025.

## Contributors
- Cubitect - implemented (port of) the spawn algorithm in [Cubiomes](https://github.com/Cubitect/cubiomes)
- NelS - created programs; checked 1.4 trillion normal biomes seeds, 160 billion Large Biomes seeds
- Jereaux - improved Cubiomes Viewer session; checked 682 billion normal biomes seeds
- Meox - checked 1.9 trillion normal biomes seeds
- Matt - set up a microBOINC project for the Forward Checker
    - Phillip, BoySanic, Derm, Meox, SundayMC, acebase, MPM - collectively checked 1.89 trillion normal biomes seeds under the project
- Derm - checked 6.2 billion Large Biomes seeds

## Relevant Links
- A [Desmos graph](https://www.desmos.com/calculator/6jefxtspjo) displaying the coordinates checked by the first two spawn stages
- A [Reddit post](https://www.reddit.com/r/minecraftseeds/comments/13kc14n/a_435_million_block_mushroom_island_and_a_spawn/) by me revealing 2242180942571 and 2939217269032
- A [compilation video](https://www.youtube.com/watch?v=hvKbkyQuZF8&t=38) by TheMisterEpic showcasing 2242180942571 (mostly accurate)
- A [compilation video](https://www.youtube.com/watch?v=_gx-fPZdrb8&t=348) by Rovant showcasing 2242180942571 (very, very inaccurate)